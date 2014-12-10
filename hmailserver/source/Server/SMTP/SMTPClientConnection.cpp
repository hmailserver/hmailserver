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
      std::shared_ptr<Event> disconnected,
      AnsiString expected_remote_hostname) :
      TCPConnection(connection_security, io_service, context, disconnected, expected_remote_hostname),
      current_state_(HELO),
      use_smtpauth_(false),
      cur_recipient_(-1),
      session_ended_(false),
      transmission_buffer_(true)
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
         EnqueueRead();
      }
   }

   void
   SMTPClientConnection::OnHandshakeCompleted()
   {
      if (GetConnectionSecurity() == CSSSL)
         EnqueueRead();
      else if (GetConnectionSecurity() == CSSTARTTLSRequired ||
               GetConnectionSecurity() == CSSTARTTLSOptional)
      {
         ProtocolStateHELOEHLO_("");
         EnqueueRead();
      }
   }

   void
   SMTPClientConnection::OnHandshakeFailed()
   {
      HandleHandshakeFailed_();
   }

   void
   SMTPClientConnection::HandleHandshakeFailed_() 
   {
      if (GetConnectionSecurity() == CSSTARTTLSOptional)
      {
         for(std::shared_ptr<MessageRecipient> recipient : recipients_)
            recipient->SetDeliveryResult(MessageRecipient::ResultOptionalHandshakeFailed);
      }
   }

   AnsiString 
   SMTPClientConnection::GetCommandSeparator() const
   {
      return "\r\n";
   }

   int
   SMTPClientConnection::SetDelivery(std::shared_ptr<Message> pDelMsg, std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients)
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
         UpdateAllRecipientsWithError_(500, "Unexpected response from server (too long).", false);
         return;
      }

      if (Request.GetLength() > 3 && (Request.GetAt(3) == '-'))
      {
         // Multi-line response. Wait for full buffer.
         multi_line_response_buffer_ += "\r\n";
         EnqueueRead();
         return;
      }

      bool postReceive = InternalParseData(multi_line_response_buffer_);

      multi_line_response_buffer_.Empty();

      if (postReceive)
         EnqueueRead();
   }

   void 
   SMTPClientConnection::LogReceivedResponse_(const String &response)
   {
      String log_data = "RECEIVED: " + response;

      log_data.TrimRight(_T("\r\n"));

      LOG_SMTP_CLIENT(GetSessionID(), GetRemoteEndpointAddress().ToString(), log_data);
   }

   bool
   SMTPClientConnection::InternalParseData(const AnsiString &Request)
   {
      LogReceivedResponse_(Request);

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
            UpdateAllRecipientsWithError_(iCode, Request, false);
            SendQUIT_();
            return true;
         }
      }

      switch (current_state_)
      {
      case HELO:
         ProtocolStateHELOEHLO_(Request);  
         return true;
      case HELOSENT:
         ProtocolHELOSent_(Request);
         return true;
      case EHLOSENT:
         ProtocolEHLOSent_(iCode, Request);
         return true;
      case STARTTLSSENT:
         ProtocolSTARTTLSSent_(iCode);
         return false;
      case AUTHLOGINSENT:
         ProtocolSendUsername_();
         return true;
      case USERNAMESENT:
         ProtocolSendPassword_();
         return true;
      case PASSWORDSENT:
         ProtocolSendMailFrom_();
         return true;
      case MAILFROMSENT:
         ProtocolMailFromSent_();
         return true;
      case DATACOMMANDSENT:
         ProtocolData_();
         return false;
      case DATASENT:
         SendQUIT_();
         UpdateSuccessfulRecipients_();
         return true;
      case RCPTTOSENT:
         ProtocolRcptToSent_(iCode, Request);
         return true;
      case QUITSENT:
         // We just received a reply on our QUIT. Time to disconnect.
         EnqueueDisconnect();
         return false;
      }

      return true;
   }

   void
   SMTPClientConnection::ProtocolStateHELOEHLO_(const AnsiString &request)
   {
      bool use_esmtp  = GetConnectionSecurity() == CSSTARTTLSRequired ||
                        GetConnectionSecurity() == CSSTARTTLSOptional ||
                        use_smtpauth_;

      String computer_name = Utilities::ComputerName(); 

      if (use_esmtp)
      {
         EnqueueWrite_("EHLO " + computer_name);
         SetState_(EHLOSENT);
      }
      else
      {
         EnqueueWrite_("HELO " + computer_name);
         SetState_(HELOSENT);
      }
         
   }

   void
   SMTPClientConnection::ProtocolMailFromSent_()
   {
      std::shared_ptr<MessageRecipient> pRecipient = GetNextRecipient_();
      if (!pRecipient) 
      {
         SendQUIT_();
         return;
      }

      String sRecipient = pRecipient->GetAddress();
      String sData = "RCPT TO:<" + sRecipient + ">";
      EnqueueWrite_(sData);
      current_state_ = RCPTTOSENT;
   }
   
   void
   SMTPClientConnection::ProtocolRcptToSent_(int code, const AnsiString &request)
   {
      if (cur_recipient_ < recipients_.size())
      {
         if (IsPositiveCompletion(code))
         {
            actual_recipients_.insert(recipients_[cur_recipient_]);
         }
         else
         {
            UpdateRecipientWithError_(code, request, recipients_[cur_recipient_], false);
         }
      }

      std::shared_ptr<MessageRecipient> pRecipient = GetNextRecipient_();
      if (pRecipient)
      {
         // Send next recipient.
         EnqueueWrite_("RCPT TO:<" + pRecipient->GetAddress() + ">");
         current_state_ = RCPTTOSENT;
      }
      else
      {
         if (actual_recipients_.size() == 0)
         {
            SendQUIT_();
         }
         else
         {
            EnqueueWrite_("DATA");
            current_state_ = DATACOMMANDSENT;
         }
         
      }
   }


   void
   SMTPClientConnection::ProtocolData_()
   {
      // Send the data!
      const String fileName = PersistentMessage::GetFileName(delivery_message_);
      StartSendFile_(fileName);
   }

   void
   SMTPClientConnection::ProtocolHELOSent_(const AnsiString &request)
   {
      ProtocolSendMailFrom_();
   }

   void
   SMTPClientConnection::ProtocolEHLOSent_(int code, const AnsiString &request)
   {
      if (!IsPositiveCompletion(code))
      {
         bool ehlo_required = GetConnectionSecurity() == CSSTARTTLSRequired ||
                              use_smtpauth_;

         if (ehlo_required)
         {
            // hMailServer is configured to require EHLO, but the remote server does not support it.
            UpdateAllRecipientsWithError_(500, "Server does not support EHLO command.", false);
            SendQUIT_();
         }
         else
         {
            // Server does not support EHLO, but we do not require it. Switch to HELO.
            String computer_name = Utilities::ComputerName(); 
            EnqueueWrite_("HELO " + computer_name);
            SetState_(HELOSENT);
         }

         return;
      }

      if (GetConnectionSecurity() == CSSTARTTLSRequired || 
          GetConnectionSecurity() == CSSTARTTLSOptional)
      {
         if (!IsSSLConnection())
         {
            if (request.Contains("STARTTLS"))
            {
               EnqueueWrite_("STARTTLS");
               SetState_(STARTTLSSENT);
               return;
            }
            else
            {
               // Remote server does not support STARTTLS
               if (GetConnectionSecurity() == CSSTARTTLSRequired)
               {
                  UpdateAllRecipientsWithError_(500, "Server does not support STARTTLS.", false);
                  SendQUIT_();
                  return;
               }
            }
         }
      }

      if (use_smtpauth_)
      {
         // Ask the server to initiate login process.
         EnqueueWrite_("AUTH LOGIN");
         SetState_(AUTHLOGINSENT);
      }
      else
      {
         ProtocolSendMailFrom_();
      }
   }

   void
   SMTPClientConnection::ProtocolSTARTTLSSent_(int code)
   {
      if (IsPositiveCompletion(code))
      {
         EnqueueHandshake();
      }
      else
      {
         HandleHandshakeFailed_();
      }
   }

   void
   SMTPClientConnection::ProtocolSendMailFrom_()
   {
      String sFrom = delivery_message_->GetFromAddress();
      String sData = "MAIL FROM:<" + sFrom + ">";
      EnqueueWrite_(sData);
      current_state_ = MAILFROMSENT;
   }

   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Updates all the actual recipients with 'success'. This is done after the
   // message delivery has been accepted by the remote server.
   //---------------------------------------------------------------------------()
   void
   SMTPClientConnection::UpdateSuccessfulRecipients_()
   {
      for(std::shared_ptr<MessageRecipient> actualRecipient : actual_recipients_)
         actualRecipient->SetDeliveryResult(MessageRecipient::ResultOK);

   }

   void
   SMTPClientConnection::LogSentCommand_(const String &sData)
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

      LOG_SMTP_CLIENT(GetSessionID(), GetIPAddressString(), sLogData);
   }

   void
   SMTPClientConnection::EnqueueWrite_(const String &sData)
   {
      LogSentCommand_(sData);

      if (current_state_ == PASSWORDSENT)
         last_sent_data_ = _T("<Password removed>");
      else
      {
         last_sent_data_ = sData;         
      }

      EnqueueWrite(sData + "\r\n");
   }

   void
   SMTPClientConnection::OnConnectionTimeout()
   {
      if (session_ended_)
      {       
         LOG_DEBUG("Session has ended, but there was a timeout while waiting for a response from the remote server.");
         return; // The session has already ended, so any error which takes place now is not interesting.
      }

      UpdateAllRecipientsWithError_(0, "There was a timeout while talking to the remote server.", false);
   }


   void
   SMTPClientConnection::OnExcessiveDataReceived()
   {
      if (session_ended_)
         return; // The session has ended, so any error which takes place now is not interesting.

      UpdateAllRecipientsWithError_(0, "Excessive amount of data sent to server.", false);
   }
   
   void
   SMTPClientConnection::OnReadError(int errorCode)
   {
      if (session_ended_)
         return; // The session has ended, so any error which takes place now is not interesting.

      UpdateAllRecipientsWithError_(0, "Remote server closed connection.", false);
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
   SMTPClientConnection::UpdateAllRecipientsWithError_(int iErrorCode, const AnsiString &sResponse, bool bPreConnectError)
   {
      auto iterRecipient = recipients_.begin();
      while (iterRecipient != recipients_.end())
      {
         UpdateRecipientWithError_(iErrorCode, sResponse, (*iterRecipient), bPreConnectError);
         iterRecipient++;
      }
      
   }

   void 
   SMTPClientConnection::UpdateRecipientWithError_(int iErrorCode, const AnsiString &sResponse, std::shared_ptr<MessageRecipient> pRecipient, bool bPreConnectError)
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
            String(sResponse).c_str());
      }
      else
      {
         sData.Format(_T("   Error Type: SMTP\r\n")
            _T("   Remote server (%s) issued an error.\r\n")
            _T("   hMailServer sent: %s\r\n")
            _T("   Remote server replied: %s\r\n"),
            String(GetIPAddressString()).c_str(), String(last_sent_data_).c_str(), String(sResponse).c_str());

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
   SMTPClientConnection::SetState_(ConnectionState eState)
   {
      current_state_ = eState;
   }

   void
   SMTPClientConnection::ProtocolSendUsername_()
   {
      String sOut;
      StringParser::Base64Encode(username_, sOut);      
      EnqueueWrite_(sOut);
      
      SetState_(USERNAMESENT);
   }

   void
   SMTPClientConnection::ProtocolSendPassword_()
   {
      String sOut;
      StringParser::Base64Encode(password_, sOut);      
      
      SetState_(PASSWORDSENT);
      EnqueueWrite_(sOut);
   }

   void
   SMTPClientConnection::SendQUIT_()
   {
      // Disconnect from the remote SMTP server.
      session_ended_ = true;

      EnqueueWrite_("QUIT");
      SetState_(QUITSENT);
   }

   std::shared_ptr<MessageRecipient> 
   SMTPClientConnection::GetNextRecipient_()
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
            std::shared_ptr<MessageRecipient> pEmpty;
            return pEmpty;
         }
      }
   }

   void
   SMTPClientConnection::StartSendFile_(const String &sFilename)
   {
      try
      {
         current_file_.Open(sFilename, File::OTReadOnly);
      }
      catch (...)
      {
         String sErrorMsg;
         sErrorMsg.Format(_T("Could not send file %s via socket since it does not exist."), sFilename.c_str());

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5019, "SMTPClientConnection::_SendFileContents", sErrorMsg);

         return;
      }

      transmission_buffer_.Initialize(shared_from_this());

      std::shared_ptr<ByteBuffer> pBuf = current_file_.ReadChunk(GetBufferSize());

      if (pBuf->GetSize() == 0)
         return;

      BYTE *pSendBuffer = (BYTE*) pBuf->GetBuffer();
      size_t iSendBufferSize = pBuf->GetSize();

      // Append the transmission buffer
      transmission_buffer_.Append(pSendBuffer, iSendBufferSize);
      
	  ReadAndSend_();
   }

   void 
   SMTPClientConnection::ReadAndSend_()
   {
      // Continue sending the file..
      int bufferSize = GetBufferSize();
      std::shared_ptr<ByteBuffer> pBuffer = current_file_.ReadChunk(bufferSize);

      while (pBuffer->GetSize() > 0)
      {
         transmission_buffer_.Append(pBuffer->GetBuffer(), pBuffer->GetSize());

         if (transmission_buffer_.Flush())
         {
            // Data was sent. We'll wait with sending more data until
            // the current data has been sent.
            return; 
         }

         pBuffer = current_file_.ReadChunk(bufferSize);
      }

      // We're done sending!
      current_file_.Close();

      // No more data to send. Make sure all buffered data is flushed.
      transmission_buffer_.Flush(true);

      // We're ready to receive the Message accepted-response.
      // No \r\n on end because EnqueueWrite adds
      EnqueueWrite_("\r\n.");

      EnqueueRead();

      // State change moved to AFTER crlf.crlf to help with race condition
      current_state_ = DATASENT;
   }

   void
   SMTPClientConnection::OnDataSent()
   {
      // Are we currently sending a file to the client?
      if (!current_file_.IsOpen())
         return;

      ReadAndSend_();
   }


   void 
   SMTPClientConnection::OnCouldNotConnect(const AnsiString &sErrorDescription)
   {
      UpdateAllRecipientsWithError_(0, sErrorDescription, true);

      LOG_DEBUG("SMTPDeliverer - Message " + StringParser::IntToString(delivery_message_->GetID()) + " - Connection failed: " + String(sErrorDescription) );
   }
}

