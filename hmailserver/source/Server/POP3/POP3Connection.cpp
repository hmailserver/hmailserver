// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "../common/BO/Account.h"
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

   POP3Connection::POP3Connection() :
      m_CurrentState(AUTHENTICATION),
       m_oTransmissionBuffer(true),
      m_bPendingDisconnect(false)
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

   }

   POP3Connection::~POP3Connection()
   {
      OnDisconnect();

      SessionManager::Instance()->OnDisconnect(STPOP3);
   }

   void
   POP3Connection::OnConnected()
   {
      String sData;

      String sWelcome = Configuration::Instance()->GetPOP3Configuration()->GetWelcomeMessage();

      sData += "+OK ";
      if (sWelcome.IsEmpty())
         sData += _T("POP3");
      else
         sData += sWelcome;

      _SendData(sData);

      PostReceive();
   }

   AnsiString 
   POP3Connection::GetCommandSeparator() const
   {
      return "\r\n";
   }


   void 
   POP3Connection::_LogClientCommand(const String &sClientData) const
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
      case AUTHENTICATION:
         {
            switch (resolvedCommand)
            {
            case USER:
            case PASS:
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
      _LogClientCommand(Request);

      if (Request.GetLength() > 500)
      {
         // This line is to long... is this an evil user?
         _SendData("-ERR Line to long.");
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

      POP3Command command = GetCommand(m_CurrentState, sCommand);

      switch (command)
      {
         case NOOP:
            _SendData("+OK");
            return ResultNormalResponse;
         case HELP:
            _SendData("+OK Normal POP3 commands allowed");
            return ResultNormalResponse;
         case USER:
            _ProtocolUSER(sParameter);
            return ResultNormalResponse;
         case PASS:
            return _ProtocolPASS(sParameter);
         case STAT:
            _ProtocolSTAT(sParameter);
            return ResultNormalResponse;
         case LIST:
            _ProtocolLIST(sParameter);
            return ResultNormalResponse;
         case RETR:
            return _ProtocolRETR(sParameter);
         case DELE:
            _ProtocolDELE (sParameter);
            return ResultNormalResponse;
         case TOP:
            _ProtocolTOP(sParameter);
            return ResultNormalResponse;
         case RSET:
            _ProtocolRSET();
            return ResultNormalResponse;   
         case UIDL:
            _ProtocolUIDL(sParameter);
            return ResultNormalResponse;   
         case QUIT:
            _ProtocolQUIT();
            return ResultDisconnect;
         case CAPA:
            _SendData("+OK CAPA list follows\r\nUSER\r\nUIDL\r\nTOP\r\n.");
            return ResultNormalResponse;
         case INVALID:
            _SendData("-ERR Invalid command in current state." );
            return ResultNormalResponse;  
         default:
            assert(0); // What command is this?
            _SendData("-ERR Invalid command in current state." );
            return ResultNormalResponse;  
      }
   }

   void
   POP3Connection::OnConnectionTimeout()
   {
      // Fix for mailbox remailing locked even after timeout
      // http://www.hmailserver.com/forum/viewtopic.php?f=7&t=22361
      _UnlockMailbox();
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
   POP3Connection::_ProtocolRSET()
   {
      if (Application::Instance()->GetFolderManager()->GetInboxMessages((int) _account->GetID(), _messages))
      {
         _ResetMailbox();
         _SendData("+OK 0");
      }
      else
         _SendData("-ERR Unable to access mailbox.");


   }


   void
   POP3Connection::_ProtocolQUIT()
   {
      // NEED FOR BUG FIX: Unlock should not be allowed unless user was auth'd
      // because next pop check is allowed even if in-use!
      // Problem is people have grown accustomed to this & could cause more
      // issues if fixed.
      _SaveMailboxChanges();
      _UnlockMailbox();

      _SendData("+OK POP3 server saying goodbye...");
   }

   void
   POP3Connection::_ProtocolUSER(const String &Parameter)
   {
      // Apply domain aliases to the user name.
      shared_ptr<DomainAliases> pDA = ObjectCache::Instance()->GetDomainAliases();
      m_Username = pDA->ApplyAliasesOnAddress(Parameter);
      _SendData("+OK Send your password" );      
   }

   POP3Connection::ParseResult
   POP3Connection::_ProtocolPASS(const String &Parameter)
   {

      m_Password = Parameter;

      AccountLogon accountLogon;
      bool disconnect = false;
      _account = accountLogon.Logon(GetIPAddress(), m_Username, m_Password, disconnect);

      if (disconnect)
      {
         _SendData("-ERR Invalid user name or password. Too many invalid logon attempts.");
         return ResultDisconnect;
      }

      if (!_account)
      {
         if (m_Username.Find(_T("@")) == -1)
            _SendData("-ERR Invalid user name or password. Please use full email address as user name.");
         else
            _SendData("-ERR Invalid user name or password.");

         return ResultNormalResponse;
      }

      // Try to lock mailbox.
      if (!POP3Sessions::Instance()->Lock(_account->GetID()))
      {
         _SendData("-ERR Your mailbox is already locked");
         return ResultNormalResponse; 
      }
  
      if (!Application::Instance()->GetFolderManager()->GetInboxMessages((int) _account->GetID(), _messages))
      {
         _SendData("+ERR Server error: Failed to fetch messages in Inbox.");
         return ResultNormalResponse;
      }

      _ResetMailbox();

      _SendData("+OK Mailbox locked and ready" );
      m_CurrentState = TRANSACTION;
      
      return ResultNormalResponse;
   }

   bool
   POP3Connection::_ProtocolDELE(const String &Parameter)
   {
      String sInputParam;
      sInputParam = Parameter;
      sInputParam.TrimLeft();
      sInputParam.TrimRight();
      long lMessageID = _ttol(sInputParam);

      if (!_account)
      {
         _SendData("-ERR No such message (messages not loaded)");
         return true;
      }

      shared_ptr<Message> message = _GetMessage(lMessageID);
      if (message)
      {
         message->SetFlagDeleted(true);
         _SendData("+OK msg deleted"); 
      }
      else
         _SendData("-ERR No such message");

      return true;
   }

   bool
   POP3Connection::_ProtocolLIST(const String &sParameter)
   {
      if (!_account)
      {
         _SendData("-ERR Message list not loaded");
         return true;
      }

      String sResponse;

      if (sParameter.IsEmpty())
      {
         int iMessageCount = 0;
         __int64 iTotalBytes = 0;
         _GetMailboxContents(iMessageCount, iTotalBytes);

         // preallocate memory to speed up a bit.
         sResponse.reserve(iMessageCount * 10);

         sResponse.Format(_T("+OK %d messages (%I64d octets)"), iMessageCount, iTotalBytes);
         _SendData(sResponse);
      
      
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
         _SendData(sResponse);

      }
      else
      {
         // List only one single message.
         long messageIndex = _ttol(sParameter);

         shared_ptr<Message> pMessage = _GetMessage(messageIndex);

         if (pMessage)
            sResponse.Format(_T("+OK %d %d"), messageIndex, pMessage->GetSize());
         else
            sResponse = "-ERR No such message";

         _SendData(sResponse);
      }

      return true;
   }

   bool
   POP3Connection::_ProtocolUIDL(const String &Parameter)
   {
      // Display a list of all the messages (index and size)
      if (!_account)
      {
         _SendData("-ERR Message list not loaded");
         return true;
      }

      String sResponse;
      String sRow;
      if (Parameter.IsEmpty())
      {
         int iMessageCount = 0;
         __int64 iTotalBytes = 0;
         _GetMailboxContents(iMessageCount, iTotalBytes);

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
         shared_ptr<Message> pMessage = _GetMessage(messageIndex);

         if (pMessage)
            sResponse.Format(_T("+OK %d %u"), messageIndex, pMessage->GetUID());
         else
            sResponse = "-ERR No such message";
      }

      // --- Send data to client.
      _SendData(sResponse);

      return true;   
   }

   POP3Connection::ParseResult
   POP3Connection::_ProtocolRETR(const String &Parameter)
   {
      if (!_account)
      {
         _SendData("-ERR Message list not loaded");
         return ResultNormalResponse;
      }

      int iRequestedMessageIndex = _tstol(Parameter);

      shared_ptr<Message> pMessage = _GetMessage(iRequestedMessageIndex);

      if (!pMessage || pMessage->GetFlagDeleted())
      {
         _SendData("-ERR No such message");
         return ResultNormalResponse;
      }

      PersistentMessage::EnsureFileExistance(_account, pMessage);

      _StartSendFile(pMessage);

      return ResultStartSendMessage;
   }

   void 
   POP3Connection::_StartSendFile(shared_ptr<Message> message)
   {  
      String fileName = PersistentMessage::GetFileName(_account, message);

      m_oTransmissionBuffer.Initialize(this);
      
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

      m_oTransmissionBuffer.Append((BYTE*) responseString.GetBuffer(), responseString.GetLength());
      
	  _ReadAndSend();
   }

   void 
   POP3Connection::_ReadAndSend()
   {
      try
      {
         // Continue sending the file..
         int bufferSize = GetBufferSize();

         shared_ptr<ByteBuffer> pBuffer = _currentFile.ReadChunk(bufferSize);

         while (pBuffer)
         {
            m_oTransmissionBuffer.Append(pBuffer->GetBuffer(), pBuffer->GetSize());

            if (m_oTransmissionBuffer.Flush())
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
         m_oTransmissionBuffer.Flush(true);

         if (!m_oTransmissionBuffer.GetLastSendEndedWithNewline())
            _SendData(""); // Send a newline character now.

         _SendData(".");

         // Request new data from the client now.
         PostReceive();
      }
      catch (...)
      {
         String location = _T("An unknown error occurred while reading and sending file data.");
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5414, "POP3Connection::_ReadAndSend", location);

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

      _ReadAndSend();
   }


   bool
   POP3Connection::_ProtocolTOP(const String &Parameter)
   {
      if (!_account)
      {
         _SendData("-ERR Message list not loaded");
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

      shared_ptr<Message> pMessage = _GetMessage(iRequestedMessageIndex);

      if (!pMessage || pMessage->GetFlagDeleted())
      {
         _SendData("-ERR No such message");
      }
      else
      {
         String fileName = PersistentMessage::GetFileName(_account, pMessage);

         String sResponse;
         sResponse.Format(_T("+OK %d octets"), pMessage->GetSize());
         _SendData(sResponse);

         // --- Send the file to the recipient.
         _SendFileHeader(fileName, iNoOfLines);
         _SendData("\r\n.");  
      }
        

      return true;
   }

   void
   POP3Connection::OnDisconnect()
   {
      _UnlockMailbox();   
   }


   void
   POP3Connection::_UnlockMailbox()
   {
      if (!_account)
         return;

      POP3Sessions::Instance()->Unlock(_account->GetID());
      _account.reset();
   }

   bool
   POP3Connection::_SendFileHeader(const String &sFilename, int iNoOfLines)
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
              
               _SendDataDebugOnly(sLine);

               iCurNoOfLines++;

               if (iCurNoOfLines == iNoOfLines)
                  break;
            }
            else
            {
               if (sLine.IsEmpty())
                  bHeaderSent = true;

               _SendDataDebugOnly(sLine);
            
            }
        
         }

         bRetVal = true;

      }


      CloseHandle(handleFile);

      return bRetVal;
   
   }

   void
   POP3Connection::_SendData(const String &sData)
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
   POP3Connection::_SendDataDebugOnly(const String &sData)
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
   POP3Connection::_SaveMailboxChanges()
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
   POP3Connection::_ProtocolSTAT(const String &sParameter)
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

      _SendData(sResponse);

      return true;

   }

   void
   POP3Connection::_GetMailboxContents(int &iNoOfMessages, __int64 &iTotalBytes)
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
   POP3Connection::_GetMessage(unsigned int index)
   {
      shared_ptr<Message> result;
      if (index >= 1 && index <= _messages.size())
         result = _messages[index-1];

      return result;
   }

   void
   POP3Connection::_ResetMailbox()
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