// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "../common/BO/Account.h"
#include "../common/BO/SecurityRange.h"
#include "../common/BO/Message.h"
#include "../common/BO/Messages.h"
#include "../common/util/file.h"
#include "../common/Util/AccountLogon.h"
#include "../common/util/ByteBuffer.h"
#include "../Common/Application/TimeoutCalculator.h"

#include "../IMAP/IMAPFolderUtilities.h"
#include "../IMAP/IMAPFolderContainer.h"
#include "../Common/BO/IMAPFolders.h"
#include "../Common/BO/IMAPFolder.h"

#include "../Common/Application/FolderManager.h"
#include "../Common/Application/ObjectCache.h"
#include "../Common/Application/SessionManager.h"
#include "../Common/BO/DomainAliases.h"
#include "../Common/Persistence/PersistentMessage.h"

#include "../Common/Tracking/ChangeNotification.h"
#include "../Common/Tracking/NotificationServer.h"

#include "POP3Sessions.h"
#include "POP3Connection.h"
#include "POP3Configuration.h"
#include "../Common/Util/PasswordRemover.h"

#include "../Common/Util/TransparentTransmissionBuffer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   POP3Connection::POP3Connection(ConnectionSecurity connection_security,
      boost::asio::io_service& io_service, 
      boost::asio::ssl::context& context) :
      AnsiStringConnection(connection_security, io_service, context, shared_ptr<Event>()),
      current_state_(AUTHORIZATION),
      transmission_buffer_(true),
      pending_disconnect_(false)
   {
      /*
        RFC 1939, Basic Operation
        A POP3 server MAY have an inactivity autologout timer.  Such a timer
        MUST be of at least 10 minutes' duration.

        Under very high load, the timeout will decrease below the values specified
        by the RFC. The reasoning is that it's better to disconnect slow clients
        than it is to disconnect everyone.
      */

      TimeoutCalculator calculator;
      SetTimeout(calculator.Calculate(IniFileSettings::Instance()->GetPOP3DMinTimeout(), IniFileSettings::Instance()->GetPOP3DMaxTimeout()));

      SessionManager::Instance()->OnCreate(STPOP3);
   }

   POP3Connection::~POP3Connection()
   {
      OnDisconnect();

      SessionManager::Instance()->OnDestroy(STPOP3);
   }

   void
   POP3Connection::OnConnected()
   {
      if (GetConnectionSecurity() == CSNone ||
          GetConnectionSecurity() == CSSTARTTLSOptional ||
          GetConnectionSecurity() == CSSTARTTLSRequired)
         SendBanner_();
   }

   void
   POP3Connection::OnHandshakeCompleted()
   {
      if (GetConnectionSecurity() == CSSSL)
         SendBanner_();
      else if (GetConnectionSecurity() == CSSTARTTLSOptional ||
               GetConnectionSecurity() == CSSTARTTLSRequired)
      {
         PostReceive();
      }
   }

   void 
   POP3Connection::SendBanner_()
   {
      String sData;

      String sWelcome = Configuration::Instance()->GetPOP3Configuration()->GetWelcomeMessage();

      sData += "+OK ";
      if (sWelcome.IsEmpty())
         sData += _T("POP3");
      else
         sData += sWelcome;

      SendData_(sData);

      PostReceive();
   }

   AnsiString 
   POP3Connection::GetCommandSeparator() const
   {
      return "\r\n";
   }


   void 
   POP3Connection::LogClientCommand_(const String &sClientData)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Logs one client command.
   //---------------------------------------------------------------------------()
   {
      if (!Logger::Instance()->GetLogPOP3())
         return;

      String sLogData = sClientData;

      // Remove any password from the log.
      PasswordRemover::Remove(PasswordRemover::PRPOP3, sLogData);

      // Append
      sLogData = "RECEIVED: " + sLogData;
      sLogData.Replace(_T("\r\n"), _T("[nl]"));

      LOG_POP3(GetSessionID(), GetIPAddressString(), sLogData);      
   }

   POP3Connection::POP3Command 
   POP3Connection::GetCommand(ConnectionState currentState, String command)
   {
      command.MakeUpper();

      POP3Command resolvedCommand = INVALID;

      if (command == _T("NOOP"))
         resolvedCommand = NOOP;
      if (command == _T("STLS"))
         resolvedCommand = STLS;
      else if (command == _T("USER"))
         resolvedCommand = USER;
      else if (command == _T("PASS"))
         resolvedCommand = PASS;
      else if (command == _T("HELP"))
         resolvedCommand = HELP;
      else if (command == _T("QUIT"))
         resolvedCommand = QUIT;
      else if (command == _T("STAT"))
         resolvedCommand = STAT;
      else if (command == _T("LIST"))
         resolvedCommand = LIST;
      else if (command == _T("RETR"))
         resolvedCommand = RETR;
      else if (command == _T("TOP"))
         resolvedCommand = TOP;
      else if (command == _T("RSET"))
         resolvedCommand = RSET;
      else if (command == _T("DELE"))
         resolvedCommand = DELE;
      else if (command == _T("UIDL"))
         resolvedCommand = UIDL;
      else if (command == _T("CAPA"))
         resolvedCommand = CAPA;
      
      // Some commands are always allowed, regardless of state.
      if (resolvedCommand == NOOP || resolvedCommand == HELP || resolvedCommand == QUIT || resolvedCommand == CAPA)
         return resolvedCommand;

      switch (currentState)
      {
      case AUTHORIZATION:
         {
            switch (resolvedCommand)
            {
            case USER:
            case PASS:
            case STLS:
               return resolvedCommand;
            default:
               return INVALID;
            }
         }
      case TRANSACTION:
         {
            switch (resolvedCommand)
            {
            case STAT:
            case LIST:
            case RETR:
            case TOP:
            case RSET:
            case DELE:
            case UIDL:
               return resolvedCommand;
            default:
               return INVALID;
            }
         }
      case UPDATE:
         {
           // Commands are not allowed within the UPDATE state.
           return INVALID;
         }
      }

      // What state are we in?
      assert(0);
      return INVALID;
   }

   void
   POP3Connection::ParseData(const AnsiString &Request)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a client POP3 command.
   //---------------------------------------------------------------------------()
   {
      // If we're currently processing a command, we should queue up the new command.
      ParseResult result = InternalParseData(Request);

      switch (result)
      {
      case ResultNormalResponse:
         PostReceive();
         break;
      case ResultStartSendMessage:
      case ResultStartTls:
         break;
      case ResultDisconnect:
         PostDisconnect();
         break;
      }

   }

   POP3Connection::ParseResult
   POP3Connection::InternalParseData(const AnsiString &Request)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a client POP3 command.
   //---------------------------------------------------------------------------()
   {
      LogClientCommand_(Request);

      if (Request.GetLength() > 500)
      {
         // This line is to long... is this an evil user?
         SendData_("-ERR Line to long.");
         return ResultNormalResponse;
      }
      
      String sCommand;
      String sParameter;

      if (Request.Find(" ")>-1)
      {
         sCommand = Request.Left(Request.Find(" "));
         sParameter = Request.Mid(Request.Find(" ") + 1);
      }
      else
         sCommand = Request;

      POP3Command command = GetCommand(current_state_, sCommand);

      switch (command)
      {
         case NOOP:
            SendData_("+OK");
            return ResultNormalResponse;
         case STLS:
            if (ProtocolSTLS_())
               return ResultStartTls;
            else
               return ResultNormalResponse;
         case HELP:
            SendData_("+OK Normal POP3 commands allowed");
            return ResultNormalResponse;
         case USER:
            ProtocolUSER_(sParameter);
            return ResultNormalResponse;
         case PASS:
            return ProtocolPASS_(sParameter);
         case STAT:
            ProtocolSTAT_(sParameter);
            return ResultNormalResponse;
         case LIST:
            ProtocolLIST_(sParameter);
            return ResultNormalResponse;
         case RETR:
            return ProtocolRETR_(sParameter);
         case DELE:
            ProtocolDELE_ (sParameter);
            return ResultNormalResponse;
         case TOP:
            ProtocolTOP_(sParameter);
            return ResultNormalResponse;
         case RSET:
            ProtocolRSET_();
            return ResultNormalResponse;   
         case UIDL:
            ProtocolUIDL_(sParameter);
            return ResultNormalResponse;   
         case QUIT:
            ProtocolQUIT_();
            return ResultDisconnect;
         case CAPA:
            ProtocolCAPA_();
            return ResultNormalResponse; 
         case INVALID:
            SendData_("-ERR Invalid command in current state." );
            return ResultNormalResponse;  
         default:
            assert(0); // What command is this?
            SendData_("-ERR Invalid command in current state." );
            return ResultNormalResponse;  
      }
   }

   void
   POP3Connection::OnConnectionTimeout()
   {
      // Fix for mailbox remailing locked even after timeout
      // http://www.hmailserver.com/forum/viewtopic.php?f=7&t=22361
      UnlockMailbox_();
      String sMessage = "-ERR Autologout; idle too long\r\n";
      SendData(sMessage);
   }

   void
   POP3Connection::OnExcessiveDataReceived()
   {
      String sMessage = "-ERR Excessive amount of data sent to server.\r\n";
      SendData(sMessage);
   }
   
   void
   POP3Connection::ProtocolRSET_()
   {
      if (Application::Instance()->GetFolderManager()->GetInboxMessages((int) _account->GetID(), _messages))
      {
         ResetMailbox_();
         SendData_("+OK 0");
      }
      else
         SendData_("-ERR Unable to access mailbox.");


   }

   void
   POP3Connection::ProtocolCAPA_()
   {
      String capabilities = "USER\r\nUIDL\r\nTOP\r\n";

      if (GetConnectionSecurity() == CSSTARTTLSOptional ||
          GetConnectionSecurity() == CSSTARTTLSRequired)
         capabilities+="STLS\r\n";

      String response = "+OK CAPA list follows\r\n" + capabilities + ".";
      SendData_(response);
   }

   void
   POP3Connection::ProtocolQUIT_()
   {
      // NEED FOR BUG FIX: Unlock should not be allowed unless user was auth'd
      // because next pop check is allowed even if in-use!
      // Problem is people have grown accustomed to this & could cause more
      // issues if fixed.
      SaveMailboxChanges_();
      UnlockMailbox_();

      SendData_("+OK POP3 server saying goodbye...");
   }

   void
   POP3Connection::ProtocolUSER_(const String &Parameter)
   {
      if (GetConnectionSecurity() == CSSTARTTLSRequired &&
          !IsSSLConnection())
      {
         SendData_("-ERR STLS is required.");
         return;
      }

      if (GetSecurityRange()->GetRequireTLSForAuth() && !IsSSLConnection())
      {
         SendData_("-ERR A SSL/TLS-connection is required for authentication.");
         return;
      }

      // Apply domain aliases to the user name.
      shared_ptr<DomainAliases> pDA = ObjectCache::Instance()->GetDomainAliases();
      username_ = pDA->ApplyAliasesOnAddress(Parameter);
      SendData_("+OK Send your password" );      
   }

   bool
   POP3Connection::ProtocolSTLS_()
   {
      if (IsSSLConnection())
      {
         SendData_("-ERR Command not permitted when TLS active");
         return false;
      }

      SendData_("+OK Begin TLS negotiation" );        
      Handshake();
      return true;
   }

   POP3Connection::ParseResult
   POP3Connection::ProtocolPASS_(const String &Parameter)
   {

      password_ = Parameter;

      AccountLogon accountLogon;
      bool disconnect = false;
      _account = accountLogon.Logon(GetRemoteEndpointAddress(), username_, password_, disconnect);

      if (disconnect)
      {
         SendData_("-ERR Invalid user name or password. Too many invalid logon attempts.");
         return ResultDisconnect;
      }

      if (!_account)
      {
         if (username_.Find(_T("@")) == -1)
            SendData_("-ERR Invalid user name or password. Please use full email address as user name.");
         else
            SendData_("-ERR Invalid user name or password.");

         return ResultNormalResponse;
      }

      // Try to lock mailbox.
      if (!POP3Sessions::Instance()->Lock(_account->GetID()))
      {
         SendData_("-ERR Your mailbox is already locked");
         return ResultNormalResponse; 
      }
  
      if (!Application::Instance()->GetFolderManager()->GetInboxMessages((int) _account->GetID(), _messages))
      {
         SendData_("+ERR Server error: Failed to fetch messages in Inbox.");
         return ResultNormalResponse;
      }

      ResetMailbox_();

      SendData_("+OK Mailbox locked and ready" );
      current_state_ = TRANSACTION;
      
      return ResultNormalResponse;
   }

   bool
   POP3Connection::ProtocolDELE_(const String &Parameter)
   {
      String sInputParam;
      sInputParam = Parameter;
      sInputParam.TrimLeft();
      sInputParam.TrimRight();
      long lMessageID = _ttol(sInputParam);

      if (!_account)
      {
         SendData_("-ERR No such message (messages not loaded)");
         return true;
      }

      shared_ptr<Message> message = GetMessage_(lMessageID);
      if (message)
      {
         message->SetFlagDeleted(true);
         SendData_("+OK msg deleted"); 
      }
      else
         SendData_("-ERR No such message");

      return true;
   }

   bool
   POP3Connection::ProtocolLIST_(const String &sParameter)
   {
      if (!_account)
      {
         SendData_("-ERR Message list not loaded");
         return true;
      }

      String sResponse;

      if (sParameter.IsEmpty())
      {
         int iMessageCount = 0;
         __int64 iTotalBytes = 0;
         GetMailboxContents_(iMessageCount, iTotalBytes);

         // preallocate memory to speed up a bit.
         sResponse.reserve(iMessageCount * 10);

         sResponse.Format(_T("+OK %d messages (%I64d octets)"), iMessageCount, iTotalBytes);
         SendData_(sResponse);
      
      
         sResponse = "";
         String sRow;

         int index = 0;
         boost_foreach(shared_ptr<Message> pMessage, _messages)
         {
            index++;
            if (!pMessage->GetFlagDeleted())
            {
               sRow.Format(_T("%d %d\r\n"), index, pMessage->GetSize());
               sResponse.append(sRow);
            }
         }

         sResponse += ".";
         
         // --- Send data to client.
         SendData_(sResponse);

      }
      else
      {
         // List only one single message.
         long messageIndex = _ttol(sParameter);

         shared_ptr<Message> pMessage = GetMessage_(messageIndex);

         if (pMessage)
            sResponse.Format(_T("+OK %d %d"), messageIndex, pMessage->GetSize());
         else
            sResponse = "-ERR No such message";

         SendData_(sResponse);
      }

      return true;
   }

   bool
   POP3Connection::ProtocolUIDL_(const String &Parameter)
   {
      // Display a list of all the messages (index and size)
      if (!_account)
      {
         SendData_("-ERR Message list not loaded");
         return true;
      }

      String sResponse;
      String sRow;
      if (Parameter.IsEmpty())
      {
         int iMessageCount = 0;
         __int64 iTotalBytes = 0;
         GetMailboxContents_(iMessageCount, iTotalBytes);

         // Allocate a reasonable size...
         sResponse.SetBuf(_messages.size() * 10);

         // Send number of messages.
         sResponse.Format(_T("+OK %d messages (%I64d octets)\r\n"), iMessageCount, iTotalBytes);
      

         // List the actual messages.
         int index = 0;
         boost_foreach(shared_ptr<Message> pMessage, _messages)
         {
            index++;
            if (!pMessage->GetFlagDeleted())
            {
               sRow.Format(_T("%d %u\r\n"), index, pMessage->GetUID());
               sResponse.append(sRow);
            }
         }

         sResponse += ".";
      }
      else
      {  
         // List only one single message.
         long messageIndex = _ttol(Parameter);
         shared_ptr<Message> pMessage = GetMessage_(messageIndex);

         if (pMessage)
            sResponse.Format(_T("+OK %d %u"), messageIndex, pMessage->GetUID());
         else
            sResponse = "-ERR No such message";
      }

      // --- Send data to client.
      SendData_(sResponse);

      return true;   
   }

   POP3Connection::ParseResult
   POP3Connection::ProtocolRETR_(const String &Parameter)
   {
      if (!_account)
      {
         SendData_("-ERR Message list not loaded");
         return ResultNormalResponse;
      }

      int iRequestedMessageIndex = _tstol(Parameter);

      shared_ptr<Message> pMessage = GetMessage_(iRequestedMessageIndex);

      if (!pMessage || pMessage->GetFlagDeleted())
      {
         SendData_("-ERR No such message");
         return ResultNormalResponse;
      }

      PersistentMessage::EnsureFileExistance(_account, pMessage);

      StartSendFile_(pMessage);

      return ResultStartSendMessage;
   }

   void 
   POP3Connection::StartSendFile_(shared_ptr<Message> message)
   {  
      String fileName = PersistentMessage::GetFileName(_account, message);

      transmission_buffer_.Initialize(shared_from_this());
      
      if (!_currentFile.Open(fileName, File::OTReadOnly))
      {
         String sErrorMessage;
         sErrorMessage.Format(_T("Could not send file %s via socket since it does not exist."), fileName);
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5063, "POP3Connection::_SendFile", sErrorMessage);
         return;
      }

      String responseTemp;
      responseTemp.Format(_T("+OK %d octets\r\n"), message->GetSize());
      AnsiString responseString = responseTemp;

      transmission_buffer_.Append((BYTE*) responseString.GetBuffer(), responseString.GetLength());
      
	  ReadAndSend_();
   }

   void 
   POP3Connection::ReadAndSend_()
   {
      try
      {
         // Continue sending the file..
         int bufferSize = GetBufferSize();

         shared_ptr<ByteBuffer> pBuffer = _currentFile.ReadChunk(bufferSize);

         while (pBuffer)
         {
            transmission_buffer_.Append(pBuffer->GetBuffer(), pBuffer->GetSize());

            if (transmission_buffer_.Flush())
            {
               // Data was sent. We'll wait with sending more data until
               // the current data has been sent.
               return; 
            }

            // No data was sent. Send some more...
            pBuffer = _currentFile.ReadChunk(bufferSize);
         }

         // We're done. Cleanup...
         _currentFile.Close();

         // No more data to send. Make sure all buffered data is flushed.
         transmission_buffer_.Flush(true);

         if (!transmission_buffer_.GetLastSendEndedWithNewline())
            SendData_(""); // Send a newline character now.

         SendData_(".");

         // Request new data from the client now.
         PostReceive();
      }
      catch (...)
      {
         String location = _T("An unknown error occurred while reading and sending file data.");
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5414, "POP3Connection::ReadAndSend_", location);

         throw;
      }
   }
   
   void
   POP3Connection::OnDataSent()
   {
      // Are we currently sending a file to the client?
      if (!_currentFile.IsOpen())
      {
         // No. Nothing to do now.
         return;
      }

      ReadAndSend_();
   }


   bool
   POP3Connection::ProtocolTOP_(const String &Parameter)
   {
      if (!_account)
      {
         SendData_("-ERR Message list not loaded");
         return true;
      }

      String Msg;
   
      long iNoOfLines = 0;
      int iSpacePos = Parameter.Find(_T(" "));
      if (iSpacePos >= 0)
      {
         Msg = Parameter.Mid(0, Parameter.Find(_T(" ")));

         String sNoOfLines;
         sNoOfLines = Parameter.Mid(iSpacePos + 1);

         if (!sNoOfLines.IsEmpty())
         {
            iNoOfLines = _ttoi(sNoOfLines);
         }
      }
      else
         Msg = Parameter;

      bool bMsgFound = false;

      int iRequestedMessageIndex = _tstol(Msg);

      shared_ptr<Message> pMessage = GetMessage_(iRequestedMessageIndex);

      if (!pMessage || pMessage->GetFlagDeleted())
      {
         SendData_("-ERR No such message");
      }
      else
      {
         String fileName = PersistentMessage::GetFileName(_account, pMessage);

         String sResponse;
         sResponse.Format(_T("+OK %d octets"), pMessage->GetSize());
         SendData_(sResponse);

         // --- Send the file to the recipient.
         SendFileHeader_(fileName, iNoOfLines);
         SendData_("\r\n.");  
      }
        

      return true;
   }

   void
   POP3Connection::OnDisconnect()
   {
      UnlockMailbox_();   
   }


   void
   POP3Connection::UnlockMailbox_()
   {
      if (!_account)
         return;

      POP3Sessions::Instance()->Unlock(_account->GetID());
      _account.reset();
   }

   bool
   POP3Connection::SendFileHeader_(const String &sFilename, int iNoOfLines)
   {

      bool bRetVal = false;

      HANDLE handleFile;

      handleFile = CreateFile(sFilename, 
                        GENERIC_READ, 
                        FILE_SHARE_READ, 
                        NULL, // LPSECURITY_ATTRIBUTES
                        OPEN_ALWAYS, // -- open or create.
                        FILE_ATTRIBUTE_NORMAL, // attributes
                        NULL // file template
                        );

      if (handleFile == INVALID_HANDLE_VALUE) 
         return false;

      SetFilePointer(handleFile,0,0,FILE_BEGIN);

      if (handleFile > 0)
      {
      
         BYTE buf[1024];
         memset(buf, 0, 1024);

         unsigned long nbytes = 0;
         BOOL bMoreData = TRUE;
         int nBytesSent = 0;

         bool bHeaderSent = false;
         int iCurNoOfLines = 0;
         while (bMoreData)
         {
         
            String sLine;
            bMoreData = FileUtilities::ReadLine(handleFile, sLine);

            if (bHeaderSent)
            {
               if (iNoOfLines <= 0)
                  break;
              
               SendData_DebugOnly(sLine);

               iCurNoOfLines++;

               if (iCurNoOfLines == iNoOfLines)
                  break;
            }
            else
            {
               if (sLine.IsEmpty())
                  bHeaderSent = true;

               SendData_DebugOnly(sLine);
            
            }
        
         }

         bRetVal = true;

      }


      CloseHandle(handleFile);

      return bRetVal;
   
   }

   void
   POP3Connection::SendData_(const String &sData)
   {
      if (Logger::Instance()->GetLogPOP3())
      {
         String sLogData = "SENT: " + sData;
         sLogData.TrimRight(_T("\r\n"));
         sLogData.Replace(_T("\r\n"), _T("[nl]"));

         LOG_POP3(GetSessionID(),GetIPAddressString(), sLogData);
      }

      SendData(sData + "\r\n");
   }

   void
   POP3Connection::SendData_DebugOnly(const String &sData)
   {
      // Logs are crazy huge for clients that do a lot of TOP's so
      // let's not log every email line unless loglevel is high enough
      if (IniFileSettings::Instance()->GetLogLevel() >= 8)
      {
         String sLogData = "SENT: " + sData;
         sLogData.TrimRight(_T("\r\n"));
         sLogData.Replace(_T("\r\n"), _T("[nl]"));

         LOG_POP3(GetSessionID(),GetIPAddressString(), sLogData);
      }

      SendData(sData + "\r\n");
   }

   void
   POP3Connection::SaveMailboxChanges_()
   {
      // Delete messages that has the delete flag on.
      if (_account)
      {
         std::set<int> messagesToDelete;
         boost_foreach(shared_ptr<Message> message, _messages)
         {
            if (message->GetFlagDeleted())
               messagesToDelete.insert(message->GetUID());
         }

         Application::Instance()->GetFolderManager()->DeleteInboxMessages((int) _account->GetID(), messagesToDelete, boost::bind(&POP3Connection::UpdateLogoutTimer, this));
      }
   }

   bool 
   POP3Connection::ProtocolSTAT_(const String &sParameter)
   {
      int iMessageCount = 0;
      // Fix for negative STAT results when box over 2GB
      __int64 iTotalSize = 0;

      std::vector<shared_ptr<Message> >::iterator iter = _messages.begin();
      std::vector<shared_ptr<Message> >::iterator iterEnd = _messages.end();

      for (; iter != iterEnd; iter++)
      {
         shared_ptr<Message> pMessage = (*iter);

         if (!pMessage->GetFlagDeleted())
         {
            iMessageCount++;
            iTotalSize += pMessage->GetSize();
         }
      }

      // --- Display basic status (number of messages and total mailbox size)
      String sResponse; 
      sResponse.Format(_T("+OK %d %I64d"), iMessageCount, iTotalSize);

      SendData_(sResponse);

      return true;

   }

   void
   POP3Connection::GetMailboxContents_(int &iNoOfMessages, __int64 &iTotalBytes)
   {
      iNoOfMessages = 0;
      iTotalBytes = 0;

      std::vector<shared_ptr<Message> >::iterator iter = _messages.begin();
      std::vector<shared_ptr<Message> >::iterator iterEnd = _messages.end();

      for (; iter != iterEnd; iter++)
      {
         shared_ptr<Message> pMessage = (*iter);

         if (!pMessage->GetFlagDeleted())
         {
            iNoOfMessages++;
            iTotalBytes += pMessage->GetSize();
         }
      }
   }

   shared_ptr<Message>
   POP3Connection::GetMessage_(unsigned int index)
   {
      shared_ptr<Message> result;
      if (index >= 1 && index <= _messages.size())
         result = _messages[index-1];

      return result;
   }

   void
   POP3Connection::ResetMailbox_()
   {
         /* 
         Reset the mailbox state. The POP3 rfc specifies the following:

            After the POP3 server has opened the maildrop, it assigns a message-
            number to each message, and notes the size of each message in octets.
            The first message in the maildrop is assigned a message-number of
            "1", the second is assigned "2", and so on, so that the nth message
            in a maildrop is assigned a message-number of "n". 

         If there are messages marked for deletion in the mailbox, this woulnd't work.

      */
      boost_foreach(shared_ptr<Message> message, _messages)
         message->SetFlagDeleted(false);
   }
}