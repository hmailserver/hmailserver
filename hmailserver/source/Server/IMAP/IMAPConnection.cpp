// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

// Application data
#include "IMAPConnection.h"
#include "IMAPResult.h"
#include "IMAPConfiguration.h"
#include "IMAPFolderContainer.h"

#include "../Common/Application/ACLManager.h"
#include "../Common/Application/TimeoutCalculator.h"

// Business data/storage
#include "../Common/BO/Account.h"
#include "../Common/BO/IMAPFolders.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/ACLPermission.h"
#include "../Common/Persistence/PersistentMessage.h"

// IMAP Utilities
#include "IMAPFolderUtilities.h"
#include "IMAPSimpleCommandParser.h"

// Common utilities
#include "../Common/Util/ByteBuffer.h"
#include "../Common/Util/PasswordRemover.h"

// Commands
#include "IMAPCommandAppend.h"
#include "IMAPCommandSearch.h"
#include "IMAPCommandUID.h"
#include "IMAPCommandIdle.h"

#include "StaticIMAPCommandHandlers.h"

#include "../Common/Application/SessionManager.h"

#include "../common/Tracking/NotificationServer.h"

#include "../Common/Application/IniFileSettings.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPConnection::IMAPConnection() :
      m_bIsIdling(false),
      m_iLiteralDataToReceive(0),
      m_bPendingDisconnect(false),
      _currentFolderReadOnly(false)
   {
      m_pIMAPFolders.reset();

      // The IMAP RFC states that the minimum connection
      // timeout is 30 minutes. If the load increases, the timeout
      // may go down to 15 minutes.

      TimeoutCalculator calculator;
      SetTimeout(calculator.Calculate(5 * 60, 30 * 60));
   }

   IMAPConnection::~IMAPConnection()
   {
      // Delete all command handles before we destroy ourselves.
      // Some of the command handlers may try to access (this)
      // on destruction, so it's important that we disconnect these
      // before terminating ourselves.
      mapCommandHandlers.clear();
   
      SessionManager::Instance()->OnDisconnect(STIMAP);

      CloseCurrentFolder();
   }

   void
   IMAPConnection::OnConnected()
   {

      Initialize();

      String sWelcome = Configuration::Instance()->GetIMAPConfiguration()->GetWelcomeMessage();

      String sData = "* OK ";
      if (sWelcome.IsEmpty())
         // Use default message.
         sData += "IMAPrev1";
      else
         sData += sWelcome;

      sData += "\r\n";

      SendAsciiData(sData);

      PostReceive();

   }

   AnsiString 
   IMAPConnection::GetCommandSeparator() const
   {
      return "\r\n";
   }

   void
   IMAPConnection::Initialize()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Connection handlers that needs to be connection-specific (nonstatic)
   //---------------------------------------------------------------------------()
   {
      mapCommandHandlers[IMAP_UID] = shared_ptr<IMAPCommandUID>(new IMAPCommandUID());
      mapCommandHandlers[IMAP_APPEND] = shared_ptr<IMAPCommandAppend>(new IMAPCommandAppend());
      mapCommandHandlers[IMAP_SEARCH] = shared_ptr<IMAPCommandSEARCH>(new IMAPCommandSEARCH(false));
      mapCommandHandlers[IMAP_SORT] = shared_ptr<IMAPCommandSEARCH>(new IMAPCommandSEARCH(true));
      mapCommandHandlers[IMAP_IDLE] = shared_ptr<IMAPCommandIdle>(new IMAPCommandIdle(shared_from_this()));

      mapStaticHandlers = StaticIMAPCommandHandlers::Instance()->GetStaticHandlers();

      _notificationClient = shared_ptr<IMAPNotificationClient>(new IMAPNotificationClient());
      _notificationClient->SetConnection(shared_from_this());
   }

   void 
   IMAPConnection::ParseData(shared_ptr<ByteBuffer> pByteBuffer)
   {
      std::map<eIMAPCommandType, shared_ptr<IMAPCommand> >::iterator iterCommandHandler = mapCommandHandlers.find(IMAP_APPEND);
      IMAPCommand* pCommand = (*iterCommandHandler).second.get();
      IMAPCommandAppend * pCommandAppend  = static_cast<IMAPCommandAppend*>(pCommand);

      pCommandAppend->ParseBinary(shared_from_this(), pByteBuffer);
   }

   void
   IMAPConnection::ParseData(const AnsiString &Request)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a client POP3 command.
   //---------------------------------------------------------------------------()
   {
      InternalParseData(Request);
      
      if (m_bPendingDisconnect == false)
         PostReceive();
   }

   void
   IMAPConnection::InternalParseData(const String &Request)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a client command. Handles literals as well.
   //---------------------------------------------------------------------------()
   {
      _LogClientCommand(Request);

      bool bHasLiterals = false;
      if (!m_sCommandBuffer.IsEmpty())
         bHasLiterals = true;

      m_sCommandBuffer += Request + "\r\n";

      int iLineEnd = m_sCommandBuffer.Find(_T("\r\n"));
      int iSpace = m_sCommandBuffer.Find(_T(" "));
      String sTag = m_sCommandBuffer.Mid(0, iSpace);
      String sCommand = m_sCommandBuffer.Mid(iSpace+1, iLineEnd - (iSpace+1));
   
      shared_ptr<IMAPClientCommand> pCommand = shared_ptr<IMAPClientCommand>(new IMAPClientCommand);
      
      // Check if we should receive any literal data.
      if (m_iLiteralDataToReceive == 0)
      {
         if (_AskForLiteralData(sCommand))
         {
            // The client is not permitted to send the octets of the literal unless
            // the server indicates that it expects it. 
            return;
         }
      }
      else
      {
         // Literal data has been received. Add it to the literal buffer
         // and check whether it's enough.
         m_sLiteralBuffer += Request;

         if (m_sLiteralBuffer.GetLength() < m_iLiteralDataToReceive)
         {
            // Tell the client that we expects more data.
            SendAsciiData("+ Ready for additional command text.\r\n");
            return;            
         }
         else
         {
            // The entire literal buffer has been received. It could be that
            // the client wants to send another literal buffer. Check if this
            // is the case.
            String sRemaining = m_sLiteralBuffer.Mid(m_iLiteralDataToReceive);

            // Since the existing literal buffer has been parsed, destroy it now.
            m_sLiteralBuffer = "";

            if (_AskForLiteralData(sRemaining))
               return ;
         }
      }
      
      if (bHasLiterals)
      {
         std::vector<String> vecCommand = StringParser::SplitString(m_sCommandBuffer, "\r\n");

         std::vector<String>::iterator iterLine = vecCommand.begin();
         String sLine = (*iterLine);

         // Any literals?
         int iLiteralBytesToReceive = _GetLiteralSize(sLine);
         String sCurrentLiteral;

         // Start the loop by jumping to the next line.
         for (iterLine++ ; iterLine != vecCommand.end(); iterLine++)
         {
            sLine = (*iterLine);

            if (iLiteralBytesToReceive <= sLine.GetLength())
            {
               sCurrentLiteral += sLine.Mid(0, iLiteralBytesToReceive);

               pCommand->vecLiteralData.push_back(sCurrentLiteral);

               sCurrentLiteral = "";

               String sRemainingOnLine = sLine.Mid(iLiteralBytesToReceive);
               iLiteralBytesToReceive = _GetLiteralSize(sRemainingOnLine);

               sCommand += sRemainingOnLine; 
            }
            else
            {
               sCurrentLiteral += sLine.Mid(0, sLine.GetLength());
               iLiteralBytesToReceive -= sLine.GetLength();

               // We have to take the remaining literal from the next line.
            }
         }

      }
 
      pCommand->Tag = sTag;
      pCommand->Command = sCommand;
      
      vecIncoming.push_back(pCommand);

      AnswerCommand();

      m_sCommandBuffer.Empty();
   }

   bool 
   IMAPConnection::_AskForLiteralData(const String &sInput)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Checks if we should request literal data. If we should, we do it now and
   // retnn true
   //---------------------------------------------------------------------------()
   {
      m_iLiteralDataToReceive = _GetLiteralSize(sInput);

      if (m_iLiteralDataToReceive > 0)
      {
         // Tell the client that we are accepting data.
         SendAsciiData("+ Ready for additional command text.\r\n");

         return true;  
      }

      return false;
   }

   bool
   IMAPConnection::_IsReceivingLiteralDataForLoginCommand() const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // We need to know this to be able to mask passwords in literal data.
   //---------------------------------------------------------------------------()
   {
      if (m_iLiteralDataToReceive > 0)
      {
         int iCommandStartPos = m_sCommandBuffer.Find(_T(" "))+1;
         if (m_sCommandBuffer.FindNoCase(_T("LOGIN")) == iCommandStartPos)
         {
            return true;
         }
      }
      
      return false;
   }

   void 
   IMAPConnection::_LogClientCommand(const String &sClientData) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Logs one client command.
   //---------------------------------------------------------------------------()
   {
      if (!Logger::Instance()->GetLogIMAP())
         return;

      String sLogData = sClientData;

      if (_IsReceivingLiteralDataForLoginCommand())
      {
         // we're receiving literal data for the login command. 
         // check if we've received the passwords yet.
         int wordsFound = 1;
         int length = m_sCommandBuffer.GetLength();
         for (int i = 0; i < m_sCommandBuffer.GetLength(); i++)
         {
            wchar_t ch = m_sCommandBuffer.GetAt(i);
            if ((ch == ' ' || ch == '\n') && i < length-1)
               wordsFound++;
            else if (ch == '}')
               wordsFound--;

            // 1 words - A01
            // 2 words - LOGIN
            // 3 words - test@test.com
            // 4 words - password.
         }

         if (wordsFound >= 3)
         {
            sLogData = "***";
         }
         
      }

      // Remove any password from the log.
      PasswordRemover::Remove(PasswordRemover::PRIMAP, sLogData);

      // Append
      sLogData = "RECEIVED: " + sLogData;
      sLogData.Replace(_T("\r\n"), _T("[nl]"));

      LOG_IMAP(GetSessionID(), GetIPAddressString(), sLogData);      
   }

   int
   IMAPConnection::_GetLiteralSize(const String &sCommand)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the size of the literal specified in the client command.
   //---------------------------------------------------------------------------()
   {
      // HACK:
      //
      // Typically the IMAPConnection takes care of parsing of literal values. This
      // is true for all commands except for the APPEND command. When it comes to
      // the append command, we take care of parsing up until the message literal
      // size. This means that if the folder name is sent as a separate literal,
      // the IMAPConnection class takes care of it.
      // 
      // The reason is that we don't want to keep the entire message in memory
      // at once (which IMAPCommandAppend handles for us) but we still want the 
      // normal parsing to take place prior to message receiving.
      //
      int iFullCommandStartPos = m_sCommandBuffer.Find(_T(" "))+1;
      if (m_sCommandBuffer.FindNoCase(_T("APPEND")) == iFullCommandStartPos)
      {
         // HACK:
         // we are currently parsing an APPEND command. No doubt about it. Try to
         // determine whether the folder name will be sent as a separate literal.
         //

         if (sCommand.FindNoCase(_T("APPEND {")) < 0)
         {
            // The current command does not indicate that the folder name
            // will be sent separately.
            return 0;
         }
      }
      
      if (sCommand.Right(1) != _T("}"))
         return 0;

      int iParStart = sCommand.ReverseFind(_T("{")) + 1;
      int iParEnd = sCommand.Find(_T("}"), iParStart);
      int iParLength = iParEnd - iParStart;

      int iSize = _ttoi(sCommand.Mid(iParStart, iParLength));
      
      return iSize;
   }


   void
   IMAPConnection::OnExcessiveDataReceived()
   {
      String sMessage = "* BYE Excessive amount of data sent to server.\r\n";
      SendAsciiData(sMessage);
   }

   void
   IMAPConnection::OnConnectionTimeout()
   {
      // We have timeout. The client is probably asleep.
      String sMessage = "* BYE You will be disconnected because of timeout (30 minutes).\r\n";
      SendAsciiData(sMessage);
   }

   void 
   IMAPConnection::Login(shared_ptr<const Account> account)
   {
      _account = account;

      RefreshIMAPFolders();
   }

   void 
   IMAPConnection::Logout(const String &goodbyeMessage)
   {
      CloseCurrentFolder();

      shared_ptr<const Account> account;
      _SetAccount(account),

      SendAsciiData(goodbyeMessage);   

      _Disconnect();
   }

   void
   IMAPConnection::_Disconnect()
   {
      m_bPendingDisconnect = true;

      ProtocolParser::PostDisconnect();
   }

   bool
   IMAPConnection::SendAsciiData(const AnsiString & sData)
   {
      if (Logger::Instance()->GetLogIMAP())
      {
         // Let's tame these logs a bit. Disables IMAP SENT
         // logging unless debug logging enabled or LogLevel > 2
         // for lines with FETCH, STATUS or short )-only lines < 5
         String sDataTmp = sData;
         int iDataLenTmp = sDataTmp.GetLength();
         m_iLogLevel = IniFileSettings::Instance()->GetLogLevel();

         if ((Logger::Instance()->GetLogDebug()) || (m_iLogLevel > 2) || (!(sDataTmp.Find(_T("FETCH")) > 0) && !(sDataTmp.Find(_T("STATUS")) > 0) && iDataLenTmp >= 5))
         {
            String sLogData = _T("SENT: ") + sData;
            sLogData.TrimRight(_T("\r\n"));
            sLogData.Replace(_T("\r\n"), _T("[nl]"));

            LOG_IMAP(GetSessionID(),GetIPAddressString(), sLogData);
         }
         // Logging gets skipped otherwise
      }

      return SendData(sData);
   }

   IMAPConnection::eIMAPCommandType 
   IMAPConnection::GetCommandType(String & sCommand)
   {
      sCommand.ToUpper();
      if (sCommand == _T("CAPABILITY"))
         return IMAP_CAPABILITY;
      else if (sCommand == _T("LOGIN"))
         return IMAP_LOGIN;
      else if (sCommand == _T("LIST"))
         return IMAP_LIST;
      else if (sCommand == _T("LSUB"))
         return IMAP_LSUB;
      else if (sCommand == _T("SELECT"))
         return IMAP_SELECT;
      else if (sCommand == _T("FETCH"))
         return IMAP_FETCH;
      else if (sCommand == _T("UID"))
         return IMAP_UID;
      else if (sCommand == _T("LOGOUT"))
         return IMAP_LOGOUT;
      else if (sCommand == _T("NOOP"))
         return IMAP_NOOP;
      else if (sCommand == _T("SUBSCRIBE"))
         return IMAP_SUBSCRIBE;
      else if (sCommand == _T("CREATE"))
         return IMAP_CREATE;
      else if (sCommand == _T("EXPUNGE"))
         return IMAP_EXPUNGE;
      else if (sCommand == _T("DELETE"))
         return IMAP_DELETE;
      else if (sCommand == _T("UNSUBSCRIBE"))
         return IMAP_UNSUBSCRIBE;
      else if (sCommand == _T("STATUS"))
         return IMAP_STATUS;
      else if (sCommand == _T("CLOSE"))
         return IMAP_CLOSE;
      else if (sCommand == _T("APPEND"))
         return IMAP_APPEND;
      else if (sCommand == _T("STORE"))
         return IMAP_STORE;
      else if (sCommand == _T("COPY"))
         return IMAP_COPY;
      else if (sCommand == _T("EXAMINE"))
         return IMAP_EXAMINE;
      else if (sCommand == _T("SEARCH"))
         return IMAP_SEARCH;
      else if (sCommand == _T("RENAME"))
         return IMAP_RENAME;
      else if (sCommand == _T("AUTHENTICATE"))
         return IMAP_AUTHENTICATE;
      else if (sCommand == _T("CHECK"))
         return IMAP_CHECK;
      else if (sCommand == _T("GETQUOTAROOT"))
         return IMAP_GETQUOTAROOT;
      else if (sCommand == _T("GETQUOTA"))
         return IMAP_GETQUOTA;
      else if (sCommand == _T("IDLE"))
         return IMAP_IDLE;
      else if (sCommand == _T("SORT"))
         return IMAP_SORT;
      else if (sCommand == _T("NAMESPACE"))
         return IMAP_NAMESPACE;
      else if (sCommand == _T("MYRIGHTS"))
         return IMAP_MYRIGHTS;
      else if (sCommand == _T("GETACL"))
         return IMAP_GETACL;
      else if (sCommand == _T("DELETEACL"))
         return IMAP_DELETEACL;
      else if (sCommand == _T("SETACL"))
         return IMAP_SETACL;
      else if (sCommand == _T("LISTRIGHTS"))
         return IMAP_LISTRIGHTS;

      return IMAP_UNKNOWN;
   }

   void 
   IMAPConnection::AnswerCommand()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Handles a single client command.
   //---------------------------------------------------------------------------()
   {
      std::vector<shared_ptr<IMAPClientCommand> >::iterator iterCommand = vecIncoming.begin();

      if (iterCommand != vecIncoming.end())
      {
         // Find the command handler.

         String sCommandTag = (*iterCommand)->Tag;
         String sCommandValue = (*iterCommand)->Command;

         int iSpace = sCommandValue.Find(_T(" "));
         String sCommandName = "";
         if (iSpace >= 0)
            sCommandName = sCommandValue.Mid(0, iSpace);
         else
            sCommandName = sCommandValue;

         if (m_bIsIdling)
         {
            _EndIdleMode();
            
            // Remove command
            vecIncoming.erase(iterCommand);
            
            return;
         }

         if (sCommandTag.IsEmpty() || sCommandValue.IsEmpty())
         {
            // No space found in the command line.
            SendAsciiData(sCommandValue + " BAD NULL COMMAND\r\n");
            
            // Remove this command since it's no good.
            vecIncoming.erase(iterCommand);

            return;
         }

         eIMAPCommandType eCommand = GetCommandType(sCommandName);

         bool bHandlerFound = false;
         
         std::map<eIMAPCommandType, shared_ptr<IMAPCommand> >::iterator iterCommandHandler = mapCommandHandlers.find(eCommand);

         if (iterCommandHandler != mapCommandHandlers.end())
            bHandlerFound = true;
         else
         {
            // Find handler in the static space.
            iterCommandHandler = mapStaticHandlers.find(eCommand);     

            if (iterCommandHandler != mapStaticHandlers.end())
               bHandlerFound = true;
         }

         if (bHandlerFound)
         {
            shared_ptr<IMAPCommand> pCommand = (*iterCommandHandler).second;

            shared_ptr<IMAPCommandArgument> pArgument = shared_ptr<IMAPCommandArgument> (new IMAPCommandArgument);

            pArgument->Command(sCommandValue);
            pArgument->Tag(sCommandTag);
            
            // Add literals.
            std::vector<String>::iterator iterStr = (*iterCommand)->vecLiteralData.begin();
            while (iterStr != (*iterCommand)->vecLiteralData.end())
            {
               pArgument->AddLiteral(*iterStr);
               iterStr++;
            }
            
            IMAPResult result = pCommand->ExecuteCommand(shared_from_this(),  pArgument);

            if (result.GetResult() == IMAPResult::ResultOK)
            {
               // If a delayed notification has been specified by the command, submit it now.
               if (m_pDelayedChangeNotification)
               {
                  Application::Instance()->GetNotificationServer()->SendNotification(_notificationClient, m_pDelayedChangeNotification);
                  m_pDelayedChangeNotification.reset();
               }
            }
            else if (result.GetResult() == IMAPResult::ResultBad)
            {
               SendAsciiData(sCommandTag + " BAD " + result.GetMessage() + "\r\n");
            }
            else if (result.GetResult() == IMAPResult::ResultNo)
            {
               SendAsciiData(sCommandTag + " NO " + result.GetMessage() + "\r\n");
            }

         }

         // Report updates on the current folder.
         if (m_pCurrentFolder)
            NotifyFolderChange();
      
         vecIncoming.erase(iterCommand);
      
      }

   }

   void 
   IMAPConnection::_EndIdleMode()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Quits idle mode.
   //---------------------------------------------------------------------------()
   {
      if (!m_bIsIdling)
      {
         assert(0);
         return;
      }

      std::map<eIMAPCommandType, shared_ptr<IMAPCommand> >::iterator iterCommandHandler = mapCommandHandlers.find(IMAP_IDLE);
      shared_ptr<IMAPCommand> pCommand = (*iterCommandHandler).second;
      shared_ptr<IMAPCommandIdle> pIdleCommand = boost::shared_static_cast<IMAPCommandIdle>(pCommand);
      pIdleCommand->Finish(true);
   }

   void
   IMAPConnection::NotifyFolderChange()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Notify the client about other changes which have been made.
   //---------------------------------------------------------------------------()
   {
      _notificationClient->SendCachedNotifications();
   }

   void
   IMAPConnection::RefreshIMAPFolders()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Fetches the cached folders
   //---------------------------------------------------------------------------()
   {
      m_pIMAPFolders = IMAPFolderContainer::Instance()->GetFoldersForAccount(_account->GetID());
      m_pPublicIMAPFolders = IMAPFolderContainer::Instance()->GetPublicFolders();
   }

   shared_ptr<IMAPFolder> 
   IMAPConnection::GetFolderByFullPath(const String &sFolderName)
   {
      String hierarchyDelimiter = Configuration::Instance()->GetIMAPConfiguration()->GetHierarchyDelimiter();
      std::vector<String> vecFolderPath = StringParser::SplitString(sFolderName, hierarchyDelimiter);

      return GetFolderByFullPath(vecFolderPath);
   }

   shared_ptr<IMAPFolder> 
   IMAPConnection::GetFolderByFullPath(std::vector<String> &vecFolderPath)
   {
      shared_ptr<IMAPFolder> pFolder;

      if (vecFolderPath.size() > 0)
      {
         bool bIsPublicFolder = IMAPFolderUtilities::IsPublicFolder(vecFolderPath);
         if (bIsPublicFolder)
         {
            // Remove the "public folder" dummy..
            vecFolderPath.erase(vecFolderPath.begin());
         }

         // Retrieve folder
         if (m_pIMAPFolders && !bIsPublicFolder)
         {
            pFolder = m_pIMAPFolders->GetFolderByFullPath(vecFolderPath);
            if (pFolder)
               return pFolder;
         }

         if (m_pPublicIMAPFolders && bIsPublicFolder)
         {
            pFolder = m_pPublicIMAPFolders->GetFolderByFullPath(vecFolderPath);

            if (pFolder)
               return pFolder;
         }
      }

      return pFolder;
   }

   void
   IMAPConnection::SendResponseString(const String &sTag, const String &sResponse, const String &sMessage) 
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Guess...
   //---------------------------------------------------------------------------()
   {
      String sEntireString = sTag + " " +sResponse + " " + sMessage + "\r\n";
      SendAsciiData(sEntireString);
   }

   void
   IMAPConnection::CloseCurrentFolder()
   {
      if (!m_pCurrentFolder)
         return;

      _notificationClient->UnsubscribeMessageChanges();

      // Set the recent flag on all messages in the folder. Since the user has been notified
      // about these messages, they are no longer recent. This doesn't happen if the folder
      // is in read-only mode - if it has been selected using the EXAMINE command.
      if (!_currentFolderReadOnly)
      {
         m_pCurrentFolder->GetMessages()->SetFlagRecentOnMessages(false);
      }

      // Unload the folder.
      m_pCurrentFolder.reset();
   }

   void
   IMAPConnection::SetCurrentFolder(shared_ptr<IMAPFolder> pFolder, bool readOnly)
   {
      // First close the currently set folder. This will cause an unsubscribe from the 
      // current folder to be made and \recent flags to be removed.
      CloseCurrentFolder();

      // Select the new folder
      m_pCurrentFolder = pFolder;
      _currentFolderReadOnly = readOnly;

      // Subscribe to changes in the new folder.
      if (m_pCurrentFolder)
      {
         _notificationClient->SubscribeMessageChanges(m_pCurrentFolder->GetAccountID(), pFolder->GetID());
      }
   }

   void 
   IMAPConnection::SetIsIdling(bool bNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Switch in or out idling mode. 
   //---------------------------------------------------------------------------()
   {
      m_bIsIdling = bNewVal;
   }

   bool
   IMAPConnection::GetIsIdling() const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Switch in or out idling mode. 
   //---------------------------------------------------------------------------()
   {
      return m_bIsIdling;
   }

   void 
   IMAPConnection::SetDelayedChangeNotification(shared_ptr<ChangeNotification> pNotification)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Sets a delayed change notification. This will be performed when the
   // current command is being replied to.
   //---------------------------------------------------------------------------()
   {
      m_pDelayedChangeNotification = pNotification;
   }

   void
   IMAPConnection::SetReceiveBinary(bool binary)
   {
      ProtocolParser::SetReceiveBinary(binary);
   }

   bool 
   IMAPConnection::CheckPermission(shared_ptr<IMAPFolder> pFolder, int iPermission)
   {
      if (!Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPACL())
      {
         // IMAP ACL has been disabled. Allow everything.
         return true;
      }

	   ACLManager aclManager;
      shared_ptr<ACLPermission> pPermission = aclManager.GetPermissionForFolder(_account->GetID(), pFolder);
      if (!pPermission)
         return false;

      if (pPermission->GetAllow((ACLPermission::ePermission) iPermission))
         return true;

      return false;
   }

   void
   IMAPConnection::CheckFolderPermissions(shared_ptr<IMAPFolder> pFolder, bool &readAccess, bool &writeAccess)
   {
      // Default no access.
      readAccess = false;
      writeAccess = false;

      if (!Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPACL())
      {
         // IMAP ACL has been disabled. Allow everything.
         readAccess = true;
         writeAccess = true;

         return;
      }

      ACLManager aclManager;
      shared_ptr<ACLPermission> pPermission = aclManager.GetPermissionForFolder(_account->GetID(), pFolder);
      if (!pPermission)
         return;

      readAccess = pPermission->GetAllow(ACLPermission::PermissionRead);
      writeAccess = pPermission->GetAllow(ACLPermission::PermissionWriteOthers) ||
         pPermission->GetAllow(ACLPermission::PermissionWriteSeen) ||
         pPermission->GetAllow(ACLPermission::PermissionWriteDeleted) ||
         pPermission->GetAllow(ACLPermission::PermissionInsert) ||
         pPermission->GetAllow(ACLPermission::PermissionExpunge);
   }

   bool 
   IMAPConnection::IsAuthenticated()
   {
      return _account;
   }
}

