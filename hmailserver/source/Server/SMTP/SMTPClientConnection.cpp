// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "SMTPClientConnection.h"

#include "../Common/BO/MessageRecipient.h"
#include "../common/Util/Utilities.h"
#include "../common/Util/File.h"
#include "../common/Util/ByteBuffer.h"
#include "../Common/Util/TransparentTransmissionBuffer.h"
#include "../Common/Persistence/PersistentMessage.h"

#include "../Common/Application/TimeoutCalculator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   SMTPClientConnection::SMTPClientConnection(ConnectionSecurity connection_security,
      boost::asio::io_service& io_service, 
      boost::asio::ssl::context& context,
      shared_ptr<Event> disconnected,
      AnsiString expected_remote_hostname) :
      AnsiStringConnection(connection_security, io_service, context, disconnected),
      current_state_(HELO),
      use_smtpauth_(false),
      cur_recipient_(-1),
      session_ended_(false),
      _transmissionBuffer(true),
      expected_remote_hostname_(expected_remote_hostname)
   {
      
      /* RFC 2821:    
         An SMTP server SHOULD have a timeout of at least 5 minutes while it
         is awaiting the next command from the sender. 

         Since the DATA command has a timeout on 10 minutes, we can just
         as well set the entire timeout to 10.
      */
      
      TimeoutCalculator calculator;
      SetTimeout(calculator.Calculate(IniFileSettings::Instance()->GetSMTPCMinTimeout(), IniFileSettings::Instance()->GetSMTPCMaxTimeout()));
   }

   SMTPClientConnection::~SMTPClientConnection()
   {
      
   }

   void
   SMTPClientConnection::OnConnected()
   {
      if (GetConnectionSecurity() == CSNone || 
          GetConnectionSecurity() == CSSTARTTLSRequired ||
          GetConnectionSecurity() == CSSTARTTLSOptional)
      {
         PostReceive();
      }
   }

   void
   SMTPClientConnection::OnHandshakeCompleted()
   {
      if (GetConnectionSecurity() == CSSSL)
         PostReceive();
      else if (GetConnectionSecurity() == CSSTARTTLSRequired ||
               GetConnectionSecurity() == CSSTARTTLSOptional)
      {
         _ProtocolStateHELOEHLO("");
         PostReceive();
      }
   }

   void
   SMTPClientConnection::OnHandshakeFailed()
   {
      HandleHandshakeFailure_();
   }

   void
   SMTPClientConnection::HandleHandshakeFailure_() 
   {
      if (GetConnectionSecurity() == CSSTARTTLSOptional)
      {
         boost_foreach(shared_ptr<MessageRecipient> recipient, recipients_)
            recipient->SetDeliveryResult(MessageRecipient::ResultOptionalHandshakeFailed);
      }
   }

   AnsiString 
   SMTPClientConnection::GetCommandSeparator() const
   {
      return "\r\n";
   }

   int
   SMTPClientConnection::SetDelivery(shared_ptr<Message> pDelMsg, std::vector<shared_ptr<MessageRecipient> > &vecRecipients)
   {
      delivery_message_ = pDelMsg;
      recipients_ = vecRecipients;
      session_ended_ = false;
      
      return 0;
   }

   void
   SMTPClientConnection::ParseData(const AnsiString &Request)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a server SMTP cmmand.
   //---------------------------------------------------------------------------()
   {
      multi_line_response_buffer_ += Request;

      if (multi_line_response_buffer_.GetLength() > 10000)
      {
         _UpdateAllRecipientsWithError(500, "Unexpected response from server (too long).", false);
         return;
      }

      if (Request.GetLength() > 3 && (Request.GetAt(3) == '-'))
      {
         // Multi-line response. Wait for full buffer.
         multi_line_response_buffer_ += "\r\n";
         PostReceive();
         return;
      }

      bool postReceive = InternalParseData(multi_line_response_buffer_);

      multi_line_response_buffer_.Empty();

      if (postReceive)
         PostReceive();
   }

   bool
   SMTPClientConnection::InternalParseData(const AnsiString  &Request)
   {
      String sData = "RECEIVED: " + Request;
      LOG_SMTP_CLIENT(GetSessionID(), GetRemoteEndpointAddress().ToString(), sData);

      int lFirstSpace = Request.Find(" ");
   
      AnsiString sFirstWordTemp;
      if (lFirstSpace < 0)
         sFirstWordTemp = Request;
      else
         sFirstWordTemp = Request.Mid(0, lFirstSpace);

      sFirstWordTemp.MakeUpper();
      int iCode = atoi(sFirstWordTemp);
   
      bool ifFailureFailAllRecipientsAndQuit = 
         current_state_ == HELO ||
         current_state_ == HELOSENT ||
         current_state_ == AUTHLOGINSENT ||
         current_state_ == USERNAMESENT || 
         current_state_ == PASSWORDSENT ||
         current_state_ == MAILFROMSENT ||
         current_state_ == DATACOMMANDSENT ||
         current_state_ == DATASENT ||
         current_state_ == PASSWORDSENT;

      if (ifFailureFailAllRecipientsAndQuit)
      {
         if (!IsPositiveCompletion(iCode))
         {
            _UpdateAllRecipientsWithError(iCode, Request, false);
            _SendQUIT();
            return true;
         }
      }

      switch (current_state_)
      {
      case HELO:
         _ProtocolStateHELOEHLO(Request);  
         return true;
      case HELOSENT:
         _ProtocolHELOEHLOSent(Request);
         return true;
      case STARTTLSSENT:
         _ProtocolSTARTTLSSent(iCode);
         return false;
      case AUTHLOGINSENT:
         _ProtocolSendUsername();
         return true;
      case USERNAMESENT:
         _ProtocolSendPassword();
         return true;
      case PASSWORDSENT:
         _ProtocolSendMailFrom();
         return true;
      case MAILFROMSENT:
         _ProtocolMailFromSent();
         return true;
      case DATACOMMANDSENT:
         _ProtocolData();
         return true;
      case DATASENT:
         _SendQUIT();
         _UpdateSuccessfulRecipients();
         return true;
      case RCPTTOSENT:
         _ProtocolRcptToSent(iCode, Request);
         return true;
      case QUITSENT:
         // We just received a reply on our QUIT. Time to disconnect.
         PostDisconnect();
         return false;
      }

      return true;
   }

   void
   SMTPClientConnection::_ProtocolStateHELOEHLO(const AnsiString &request)
   {
      if (request.GetLength() > 0)
         remoteServerBanner_ = request;

      bool server_supports_esmtp = GetServerSupportsESMTP_();
      if (GetConnectionSecurity() == CSSTARTTLSRequired || use_smtpauth_)
      {
         if (!server_supports_esmtp)
         {
            _UpdateAllRecipientsWithError(500, "Remote server does not support ESMTP which is required for STARTTLS and authentication.", false);
            _SendQUIT();
            return;
         }
      }

      String computer_name = Utilities::ComputerName(); 

      if (GetServerSupportsESMTP_())
         _SendData("EHLO " + computer_name);
      else
         _SendData("HELO " + computer_name);
         
      _SetState(HELOSENT);
   }

   bool
   SMTPClientConnection::GetServerSupportsESMTP_()
   {
      if (remoteServerBanner_.Contains("ESMTP"))
         return true;

      return false;
   }

   void
   SMTPClientConnection::_ProtocolMailFromSent()
   {
      shared_ptr<MessageRecipient> pRecipient = _GetNextRecipient();
      if (!pRecipient) 
      {
         _SendQUIT();
         return;
      }

      String sRecipient = pRecipient->GetAddress();
      String sData = "RCPT TO:<" + sRecipient + ">";
      _SendData(sData);
      current_state_ = RCPTTOSENT;
   }
   
   void
   SMTPClientConnection::_ProtocolRcptToSent(int code, const AnsiString &request)
   {
      if (cur_recipient_ < recipients_.size())
      {
         if (IsPositiveCompletion(code))
         {
            _actualRecipients.insert(recipients_[cur_recipient_]);
         }
         else
         {
            _UpdateRecipientWithError(code, request, recipients_[cur_recipient_], false);
         }
      }

      shared_ptr<MessageRecipient> pRecipient = _GetNextRecipient();
      if (pRecipient)
      {
         // Send next recipient.
         _SendData("RCPT TO:<" + pRecipient->GetAddress() + ">");
         current_state_ = RCPTTOSENT;
      }
      else
      {
         if (_actualRecipients.size() == 0)
         {
            _SendQUIT();
         }
         else
         {
            _SendData("DATA");
            current_state_ = DATACOMMANDSENT;
         }
         
      }
   }


   void
   SMTPClientConnection::_ProtocolData()
   {
      // Send the data!
      const String fileName = PersistentMessage::GetFileName(delivery_message_);
      _StartSendFile(fileName);
   }

   void
   SMTPClientConnection::_ProtocolHELOEHLOSent(const AnsiString &request)
   {
      if (GetConnectionSecurity() == CSSTARTTLSRequired || 
          GetConnectionSecurity() == CSSTARTTLSOptional)
      {
         if (!IsSSLConnection())
         {
            if (request.Contains("STARTTLS"))
            {
               _SendData("STARTTLS");
               _SetState(STARTTLSSENT);
               return;
            }
            else
            {
               // Remote server does not support STARTTLS
               if (GetConnectionSecurity() == CSSTARTTLSRequired)
               {
                  _UpdateAllRecipientsWithError(500, "Server does not support STARTTLS.", false);
                  _SendQUIT();
                  return;
               }
            }
         }
      }

      if (use_smtpauth_)
      {
         // Ask the server to initiate login process.
         _SendData("AUTH LOGIN");
         _SetState(AUTHLOGINSENT);
      }
      else
      {
         _ProtocolSendMailFrom();
      }
   }

   void
   SMTPClientConnection::_ProtocolSTARTTLSSent(int code)
   {
      if (IsPositiveCompletion(code))
      {
         Handshake(expected_remote_hostname_);
      }
      else
      {
         HandleHandshakeFailure_();
      }
   }

   void
   SMTPClientConnection::_ProtocolSendMailFrom()
   {
      String sFrom = delivery_message_->GetFromAddress();
      String sData = "MAIL FROM:<" + sFrom + ">";
      _SendData(sData);
      current_state_ = MAILFROMSENT;
   }

   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates all the actual recipients with 'success'. This is done after the
   // message delivery has been accepted by the remote server.
   //---------------------------------------------------------------------------()
   void
   SMTPClientConnection::_UpdateSuccessfulRecipients()
   {
      boost_foreach(shared_ptr<MessageRecipient> actualRecipient, _actualRecipients)
         actualRecipient->SetDeliveryResult(MessageRecipient::ResultOK);

   }

   void
   SMTPClientConnection::_LogSentCommand(const String &sData)
   {
      if (!(Logger::Instance()->GetLogMask() & Logger::LSSMTP))
         return;

      String sLogData = sData;

      if (current_state_ == PASSWORDSENT)
      {
         // Password has been sent. Remove from log.
         sLogData = "***";
      }

      // Append
      sLogData = "SENT: " + sLogData;
      
      sLogData.TrimRight(_T("\r\n"));
      sLogData.Replace(_T("\r\n"), _T("[nl]"));

      LOG_SMTP_CLIENT(GetSessionID(), GetIPAddressString(), sLogData);
   }

   void
   SMTPClientConnection::_SendData(const String &sData)
   {
      _LogSentCommand(sData);

      if (current_state_ == PASSWORDSENT)
         last_sent_data_ = _T("<Password removed>");
      else
      {
         last_sent_data_ = sData;         
      }

      SendData(sData + "\r\n");
   }

   void
   SMTPClientConnection::OnConnectionTimeout()
   {
      if (session_ended_)
      {       
         LOG_DEBUG("Session has ended, but there was a timeout while waiting for a response from the remote server.");
         return; // The session has already ended, so any error which takes place now is not interesting.
      }

      _UpdateAllRecipientsWithError(0, "There was a timeout while talking to the remote server.", false);
   }


   void
   SMTPClientConnection::OnExcessiveDataReceived()
   {
      if (session_ended_)
         return; // The session has ended, so any error which takes place now is not interesting.

      _UpdateAllRecipientsWithError(0, "Excessive amount of data sent to server.", false);
   }
   
   void
   SMTPClientConnection::OnReadError(int errorCode)
   {
      if (session_ended_)
         return; // The session has ended, so any error which takes place now is not interesting.

      _UpdateAllRecipientsWithError(0, "Remote server closed connection.", false);
   }

   bool 
   SMTPClientConnection::IsPermanentNegative(int lErrorCode)
   {
      if (lErrorCode >= 500 && lErrorCode <= 599)
         return true;
      else
         return false;
   }

   bool 
   SMTPClientConnection::IsPositiveCompletion(int lErrorCode)
   {
      if ((lErrorCode >= 200 && lErrorCode <= 299 ||
           lErrorCode >= 300 && lErrorCode <= 399))
         return true;
      else
         return false;
   }

   void 
   SMTPClientConnection::_UpdateAllRecipientsWithError(int iErrorCode, const AnsiString &sResponse, bool bPreConnectError)
   {
      std::vector<shared_ptr<MessageRecipient> >::iterator iterRecipient = recipients_.begin();
      while (iterRecipient != recipients_.end())
      {
         _UpdateRecipientWithError(iErrorCode, sResponse, (*iterRecipient), bPreConnectError);
         iterRecipient++;
      }
      
   }

   void 
   SMTPClientConnection::_UpdateRecipientWithError(int iErrorCode, const AnsiString &sResponse, shared_ptr<MessageRecipient> pRecipient, bool bPreConnectError)
   {
      if (pRecipient->GetDeliveryResult() == MessageRecipient::ResultFatalError)
      {
         // This recipient is already marked as fatally failed. No need to
         // update it's status since it can't change from fatal to nonfatal.
         return;
      }

      bool bIsFatalError = IsPermanentNegative(iErrorCode);
      
      if (pRecipient->GetDeliveryResult() == MessageRecipient::ResultNonFatalError)
      {
         // If we have a non-fatal error,  but the new error is fatal, we should
         // update it's status. But if the error level is the same as before, we
         // should not. We should only update the delivery result if the result
         // has gotten worse than before. Overwriting with the same status may
         // remove useful error messages.

         if (!bIsFatalError)	
         {
            // No this is the same error level.
            return;
         }
      }

      // Update the delivery status
	   pRecipient->SetDeliveryResult(bIsFatalError ? MessageRecipient::ResultFatalError : MessageRecipient::ResultNonFatalError);

      last_sent_data_.TrimLeft("\r\n");
      last_sent_data_.TrimRight("\r\n");

      String sData;

      if (bPreConnectError)
      {
         sData.Format(_T("   Error Type: SMTP\r\n")
            _T("   Connection to recipients server failed.\r\n")
            _T("   Error: %s\r\n"),
            String(sResponse));
      }
      else
      {
         sData.Format(_T("   Error Type: SMTP\r\n")
            _T("   Remote server (%s) issued an error.\r\n")
            _T("   hMailServer sent: %s\r\n")
            _T("   Remote server replied: %s\r\n"),
            String(GetIPAddressString()), String(last_sent_data_), String(sResponse));

      }
      pRecipient->SetErrorMessage(sData);
   } 

   void 
   SMTPClientConnection::SetAuthInfo(const String &sUsername, const String &sPassword)
   {
      username_ = sUsername;
      password_ = sPassword;
      use_smtpauth_ = true;
   }
   
   void 
   SMTPClientConnection::_SetState(ConnectionState eState)
   {
      current_state_ = eState;
   }

   void
   SMTPClientConnection::_ProtocolSendUsername()
   {
      String sOut;
      StringParser::Base64Encode(username_, sOut);      
      _SendData(sOut);
      
      _SetState(USERNAMESENT);
   }

   void
   SMTPClientConnection::_ProtocolSendPassword()
   {
      String sOut;
      StringParser::Base64Encode(password_, sOut);      
      
      _SetState(PASSWORDSENT);
      _SendData(sOut);
   }

   void
   SMTPClientConnection::_SendQUIT()
   {
      // Disconnect from the remote SMTP server.
      session_ended_ = true;

      _SendData("QUIT");
      _SetState(QUITSENT);
   }

   shared_ptr<MessageRecipient> 
   SMTPClientConnection::_GetNextRecipient()
   {
      while (1)
      {
         cur_recipient_ ++;

         if (cur_recipient_ < recipients_.size())
         {
            return recipients_[cur_recipient_];
         }
         else
         {
            shared_ptr<MessageRecipient> pEmpty;
            return pEmpty;
         }
      }
   }

   void
   SMTPClientConnection::_StartSendFile(const String &sFilename)
   {
      if (!_currentFile.Open(sFilename, File::OTReadOnly))
      {
         String sErrorMsg;
         sErrorMsg.Format(_T("Could not send file %s via socket since it does not exist."), sFilename);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5019, "SMTPClientConnection::_SendFileContents", sErrorMsg);

         return;
      }

      _transmissionBuffer.Initialize(shared_from_this());

      shared_ptr<ByteBuffer> pBuf = _currentFile.ReadChunk(GetBufferSize());

      if (!pBuf)
         return;

      BYTE *pSendBuffer = (BYTE*) pBuf->GetBuffer();
      int iSendBufferSize = pBuf->GetSize();

      // Append the transmission buffer
      _transmissionBuffer.Append(pSendBuffer, iSendBufferSize);
      
	  _ReadAndSend();
   }

   void 
   SMTPClientConnection::_ReadAndSend()
   {
      LOG_DEBUG("SMTPClientConnection::~_Continue sendfile");
      // Continue sending the file..
      int bufferSize = GetBufferSize();
      shared_ptr<ByteBuffer> pBuffer = _currentFile.ReadChunk(bufferSize);

      while (pBuffer)
      {
         _transmissionBuffer.Append(pBuffer->GetBuffer(), pBuffer->GetSize());

         if (_transmissionBuffer.Flush())
         {
            // Data was sent. We'll wait with sending more data until
            // the current data has been sent.
            return; 
         }

         pBuffer = _currentFile.ReadChunk(bufferSize);
      }

      // We're done sending!
      _currentFile.Close();

      // No more data to send. Make sure all buffered data is flushed.
      _transmissionBuffer.Flush(true);

      // We're ready to receive the Message accepted-response.
      // No \r\n on end because SendData adds
      _SendData("\r\n.");

      // State change moved to AFTER crlf.crlf to help with race condition
      current_state_ = DATASENT;
      return;
   }

   void
   SMTPClientConnection::OnDataSent()
   {
      // Are we currently sending a file to the client?
      if (!_currentFile.IsOpen())
         return;

      _ReadAndSend();
   }


   void 
   SMTPClientConnection::OnCouldNotConnect(const AnsiString &sErrorDescription)
   {
      _UpdateAllRecipientsWithError(0, sErrorDescription, true);

      LOG_SMTP_CLIENT(0,"TCP","SMTPDeliverer - Message " + StringParser::IntToString(delivery_message_->GetID()) + " - Connection failed: " + String(sErrorDescription) );
   }
}

