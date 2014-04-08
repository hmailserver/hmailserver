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

   SMTPClientConnection::SMTPClientConnection() :
      m_CurrentState(HELO),
      m_bUseSMTPAuth(false),
      m_iCurRecipient(-1),
      m_bSessionEnded(false),
      m_bPendingDisconnect(false),
      _transmissionBuffer(true)
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
      PostReceive();
   }

   AnsiString 
   SMTPClientConnection::GetCommandSeparator() const
   {
      return "\r\n";
   }

   int
   SMTPClientConnection::SetDelivery(shared_ptr<Message> pDelMsg, std::vector<shared_ptr<MessageRecipient> > &vecRecipients)
   {
      m_pDeliveryMessage = pDelMsg;
      m_vecRecipients = vecRecipients;
      m_bSessionEnded = false;
      
      return 0;
   }

   void
   SMTPClientConnection::ParseData(const AnsiString &Request)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a server SMTP cmmand.
   //---------------------------------------------------------------------------()
   {
      InternalParseData(Request);

      if (m_bPendingDisconnect == false)
         PostReceive();
   }

   void
   SMTPClientConnection::InternalParseData(const AnsiString  &Request)
   {
      LOG_DEBUG("SMTPClientConnection::_ParseASCII()");

      String sData = "RECEIVED: " + Request;
      LOG_SMTP_CLIENT(GetSessionID(), GetIPAddress().ToString(), sData);

      // Below 3 lines is fix of the problem that occurs when the remote server answers
      // with 2 line in his welcome message.
      String sMinus = "-";
      if ((Request.GetLength() > 3) && (Request.GetAt(3) == sMinus.GetAt(0)))
      {
         LOG_DEBUG("SMTPClientConnection::~_ParseASCII() - 1");
         return;
      }

      int lFirstSpace = Request.Find(" ");
   
      AnsiString sFirstWordTemp;
      if (lFirstSpace < 0)
         sFirstWordTemp = Request;
      else
         sFirstWordTemp = Request.Mid(0, lFirstSpace);
      sFirstWordTemp.MakeUpper();
      int iCode = atoi(sFirstWordTemp);

      // We should not update all recipient's if we've just sent a
      // RCPT TO. We should only update the specific one we've just
      // sent to.
      // 
      // Also, we should not update state if we've just sent QUIT to
      // the server. At the time we send QUIT, the message has already
      // been either accepted for delivery and rejected. Any error after
      // this isn't relvat.
      if (m_CurrentState != RCPTTOSENT && m_CurrentState != QUITSENT)
      {
         if (IsPermanentNegative(iCode))
         {
            _UpdateAllRecipientsWithError(iCode, Request, false);
            _SendQUIT();
            return;
         }
         else if (IsTransientNegative(iCode))
         {
            _UpdateAllRecipientsWithError(iCode, Request, false);
            _SendQUIT();
            return;
         }
      }
   
      switch (m_CurrentState)
      {
      case SENDUSERNAME:
         _ProtocolSendUsername();
         break;
      case SENDPASSWORD:
         _ProtocolSendPassword();
         break;
      case PASSWORDCHECK:
         if (!_ProtocolPassswordCheck(iCode, Request))
         {
            // Authentication failed. We have just sent
            // a quit command.
            return;
         }

         break;
      }
      
      if (m_CurrentState == HELO)
      {
         if (iCode == 220)
         {
			   String sComputerName = Utilities::ComputerName(); 
      
            if (m_bUseSMTPAuth)
            {
               _SendData("EHLO " + sComputerName);
               _SetState(EHLOSENT);
            }
            else
            {
               _SendData("HELO " + sComputerName);
               _SetState(HELOSENT);
            }
        
            LOG_DEBUG("SMTPClientConnection::~_ParseASCII() - 2");

            return ;
         }
         else
         {
            LOG_DEBUG("SMTPClientConnection::~_ParseASCII() - 3");
            _UpdateAllRecipientsWithError(iCode, Request, false);
            _SendQUIT();
            return;
         }
      }

      if (m_CurrentState == EHLOSENT)
      {
         // Ask the server to initiate login process.
         _SendData("AUTH LOGIN");
         _SetState(SENDUSERNAME);
         return ;
      }

      if (m_CurrentState == HELOSENT)
      {
         if (IsPositiveCompletion(iCode))
         {
            // --- Server accepted HELO. Go to HEADER/MAILFROM state.
            _SetState(MAILFROM);
         }
	      else
         {
            _UpdateAllRecipientsWithError(iCode, Request, false);
         }
      }
   

      if (m_CurrentState == MAILFROM)
      {
         String sFrom = m_pDeliveryMessage->GetFromAddress();
         String sData = "MAIL FROM:<" + sFrom + ">";
         _SendData(sData);
         m_CurrentState = MAILFROMSENT;
         LOG_DEBUG("SMTPClientConnection::~_ParseASCII() - 4");
         return;
      }

      if (m_CurrentState == MAILFROMSENT)
      {
         if (IsPositiveCompletion(iCode))
         {
            // --- Server accepted mail from. Go to header/rcpt to state.
            m_CurrentState = RCPTTO;
         }
         else
         {
            LOG_DEBUG("SMTPClientConnection::~_ParseASCII() - 5");
            _UpdateAllRecipientsWithError(iCode, Request, false);
         }

      }

      if (m_CurrentState == RCPTTO)
      {
         LOG_DEBUG("SMTPClientConnection::~_ParseASCII() - 6");

         shared_ptr<MessageRecipient> pRecipient = _GetNextRecipient();
         if (!pRecipient) 
         {
            _SendQUIT();
            return;
         }
         
         String sRecipient = pRecipient->GetAddress();
         String sData = "RCPT TO:<" + sRecipient + ">";
         _SendData(sData);
         m_CurrentState = RCPTTOSENT;

         return;

      }

      if (m_CurrentState == RCPTTOSENT)
      {
         if (m_iCurRecipient < m_vecRecipients.size())
         {
            if (IsPositiveCompletion(iCode))
            {
               _actualRecipients.insert(m_vecRecipients[m_iCurRecipient]);
            }
            else
            {
               _UpdateRecipientWithError(iCode, Request, m_vecRecipients[m_iCurRecipient], false);
            }
         }

         shared_ptr<MessageRecipient> pRecipient = _GetNextRecipient();
         if (pRecipient)
         {
            // Send next recipient.
            _SendData("RCPT TO:<" + pRecipient->GetAddress() + ">");
            m_CurrentState = RCPTTOSENT;
         }
         else
         {
            if (_actualRecipients.size() == 0)
            {
               _SendQUIT();
               return;
            }
            
            m_CurrentState = DATAQUESTION;
         }
      }

      if (m_CurrentState == DATAQUESTION)
      {
         _SendData("DATA");
         m_CurrentState = DATA;
         LOG_DEBUG("SMTPClientConnection::~_ParseASCII() - 7");
         return;
      }

      if (m_CurrentState == DATA)
      {
         if (IsPositiveIntermediate(iCode))
         {
            // Send the data!
            const String fileName = PersistentMessage::GetFileName(m_pDeliveryMessage);
            LOG_DEBUG("SMTPClientConnection::~_BEFORE SendFile");
            _StartSendFile(fileName);
            LOG_DEBUG("SMTPClientConnection::~_AFTER SendFile");
            return;
         }
      }

      if (m_CurrentState == DATASENT)
      {
            LOG_DEBUG("SMTPClientConnection::~_BEFORE SendQUIT");
         _SendQUIT();
            LOG_DEBUG("SMTPClientConnection::~_AFTER SendQUIT");

         if (IsPositiveCompletion(iCode))
         {
            _UpdateSuccessfulRecipients();
            LOG_DEBUG("SMTPClientConnection::~_ParseASCII() - 9");
            return;
         }
         else
         {
            _UpdateAllRecipientsWithError(iCode, Request, false);
            LOG_DEBUG("SMTPClientConnection::~_ParseASCII() - 10");
         }

         return;
      }

      if (m_CurrentState == QUITSENT)
      {     
         // We just received a reply on our QUIT. Time to disconnect.
         m_bPendingDisconnect = true;
         PostDisconnect();
      }
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
   SMTPClientConnection::_LogSentCommand(const String &sData) const
   {
      if (!(Logger::Instance()->GetLogMask() & Logger::LSSMTP))
         return;

      String sLogData = sData;

      if (m_CurrentState == PASSWORDCHECK)
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

      if (m_CurrentState == PASSWORDCHECK)
         m_sLastSentData = _T("<Password removed>");
      else
      {
         m_sLastSentData = sData;         
      }

      SendData(sData + "\r\n");
   }

   void
   SMTPClientConnection::OnConnectionTimeout()
   {
      if (m_bSessionEnded)
      {       
         LOG_DEBUG("Session has ended, but there was a timeout while waiting for a response from the remote server.");
         return; // The session has already ended, so any error which takes place now is not interesting.
      }

      _UpdateAllRecipientsWithError(0, "There was a timeout while talking to the remote server.", false);
   }


   void
   SMTPClientConnection::OnExcessiveDataReceived()
   {
      if (m_bSessionEnded)
         return; // The session has ended, so any error which takes place now is not interesting.

      _UpdateAllRecipientsWithError(0, "Excessive amount of data sent to server.", false);
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
   SMTPClientConnection::IsTransientNegative(int lErrorCode)
   {
      if (lErrorCode >= 400 && lErrorCode <= 499)
         return true;
      else
         return false;
   }

   bool 
   SMTPClientConnection::IsPositiveCompletion(int lErrorCode)
   {
      if (lErrorCode >= 200 && lErrorCode <= 299)
         return true;
      else
         return false;
   }

   bool 
   SMTPClientConnection::IsPositiveIntermediate(int lErrorCode)
   {
      if (lErrorCode >= 300 && lErrorCode <= 399)
         return true;
      else
         return false;
   }

   void 
   SMTPClientConnection::_UpdateAllRecipientsWithError(int iErrorCode, const AnsiString &sResponse, bool bPreConnectError)
   {
      std::vector<shared_ptr<MessageRecipient> >::iterator iterRecipient = m_vecRecipients.begin();
      while (iterRecipient != m_vecRecipients.end())
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

      m_sLastSentData.TrimLeft("\r\n");
      m_sLastSentData.TrimRight("\r\n");

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
            String(GetIPAddressString()), String(m_sLastSentData), String(sResponse));

      }
      pRecipient->SetErrorMessage(sData);
   } 

   void 
   SMTPClientConnection::SetAuthInfo(const String &sUsername, const String &sPassword)
   {
      m_sUsername = sUsername;
      m_sPassword = sPassword;
      m_bUseSMTPAuth = true;
   }
   
   void 
   SMTPClientConnection::_SetState(ConnectionState eState)
   {
      m_CurrentState = eState;
   }

   void
   SMTPClientConnection::_ProtocolSendUsername()
   {
      String sOut;
      StringParser::Base64Encode(m_sUsername, sOut);      
      _SendData(sOut);
      
      _SetState(SENDPASSWORD);
   }

   void
   SMTPClientConnection::_ProtocolSendPassword()
   {
      String sOut;
      StringParser::Base64Encode(m_sPassword, sOut);      
      
      _SetState(PASSWORDCHECK);

      _SendData(sOut);
   }

   bool
   SMTPClientConnection::_ProtocolPassswordCheck(int iCode, const String &sServerLine)
   {
      if (!IsPositiveCompletion(iCode))
      {
         // Authentication failed with an permanent error.
         String sErrorMessage = "Authentication error. Server response: " + sServerLine;
         _UpdateAllRecipientsWithError(iCode, sErrorMessage, false);

         _SendQUIT();

         return false;
      }
      
      // The server accepted our authentication.
      _SetState(MAILFROM);
      return true;
     
   }

   void
   SMTPClientConnection::_SendQUIT()
   {
      // Disconnect from the remote SMTP server.
      m_bSessionEnded = true;

      _SendData("QUIT");
      _SetState(QUITSENT);
   }

   shared_ptr<MessageRecipient> 
   SMTPClientConnection::_GetNextRecipient()
   {
      while (1)
      {
         m_iCurRecipient ++;

         if (m_iCurRecipient < m_vecRecipients.size())
         {
            return m_vecRecipients[m_iCurRecipient];
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

      _transmissionBuffer.Initialize(this);

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

       LOG_DEBUG("SMTPClientConnection::~_SendFile done close file");
      // We're done sending!
      _currentFile.Close();

      // No more data to send. Make sure all buffered data is flushed.
      _transmissionBuffer.Flush(true);

      LOG_DEBUG("SMTPClientConnection::~_SendFile flushed buffer");

      // We're ready to receive the Message accepted-response.
      LOG_DEBUG("SMTPClientConnection::~_SendFile DATASENT set");

      // No \r\n on end because SendData adds
      _SendData("\r\n.");

      LOG_DEBUG("SMTPClientConnection::~_SendFile . sent");

      // State change moved to AFTER crlf.crlf to help with race condition
      m_CurrentState = DATASENT;
      LOG_DEBUG("SMTPClientConnection::~_SendFile DATASENT set");
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

      LOG_SMTP_CLIENT(0,"TCP","SMTPDeliverer - Message " + StringParser::IntToString(m_pDeliveryMessage->GetID()) + " - Connection failed: " + String(sErrorDescription) );
   }

}
