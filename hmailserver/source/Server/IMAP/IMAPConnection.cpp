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

// IMAP Utilities
#include "IMAPFolderUtilities.h"

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
   IMAPConnection::IMAPConnection(ConnectionSecurity connection_security,
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context) :
      TCPConnection(connection_security, io_service, context, std::shared_ptr<Event>(), ""),
      is_idling_(false),
      literal_data_to_receive_(0),
      pending_disconnect_(false),
      current_folder_read_only_(false),
      log_level_(0)
   {
      imap_folders_.reset();


      // The IMAP RFC states that the minimum connection
      // timeout is 30 minutes. If the load increases, the timeout
      // may go down to 15 minutes.

      TimeoutCalculator calculator;
      SetTimeout(calculator.Calculate(5 * 60, 30 * 60));

   }

   IMAPConnection::~IMAPConnection()
   {
      try
      {
         // Delete all command handles before we destroy ourselves.
         // Some of the command handlers may try to access (this)
         // on destruction, so it's important that we disconnect these
         // before terminating ourselves.
         mapCommandHandlers.clear();

         if (GetConnectionState() != StatePendingConnect)
            SessionManager::Instance()->OnSessionEnded(STIMAP);

         CloseCurrentFolder();
      }
      catch (...)
      {

      }
   }

   void
   IMAPConnection::OnConnected()
   {
      Initialize();

      if (GetConnectionSecurity() == CSNone ||
          GetConnectionSecurity() == CSSTARTTLSOptional ||
          GetConnectionSecurity() == CSSTARTTLSRequired)      
      {
         SendBanner_();
      }
   }

   void
   IMAPConnection::OnHandshakeCompleted()
   {
      if (GetConnectionSecurity() == CSSSL)      
         SendBanner_();
      else if (GetConnectionSecurity() == CSSTARTTLSOptional ||
               GetConnectionSecurity() == CSSTARTTLSRequired)
      {
         EnqueueRead();
      }
   }

   void 
   IMAPConnection::SendBanner_()
   {
      String sWelcome = Configuration::Instance()->GetIMAPConfiguration()->GetWelcomeMessage();

      String sData = "* OK ";
      if (sWelcome.IsEmpty())
         // Use default message.
         sData += "IMAPrev1";
      else
         sData += sWelcome;

      sData += "\r\n";

      SendAsciiData(sData);

      EnqueueRead();
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
      mapCommandHandlers[IMAP_UID] = std::shared_ptr<IMAPCommandUID>(new IMAPCommandUID());
      mapCommandHandlers[IMAP_APPEND] = std::shared_ptr<IMAPCommandAppend>(new IMAPCommandAppend());
      mapCommandHandlers[IMAP_SEARCH] = std::shared_ptr<IMAPCommandSEARCH>(new IMAPCommandSEARCH(false));
      mapCommandHandlers[IMAP_SORT] = std::shared_ptr<IMAPCommandSEARCH>(new IMAPCommandSEARCH(true));
      mapCommandHandlers[IMAP_IDLE] = std::shared_ptr<IMAPCommandIdle>(new IMAPCommandIdle(std::dynamic_pointer_cast<IMAPConnection>(shared_from_this())));

      mapStaticHandlers = StaticIMAPCommandHandlers::Instance()->GetStaticHandlers();

      notification_client_ = std::shared_ptr<IMAPNotificationClient>(new IMAPNotificationClient());
      notification_client_->SetConnection(std::dynamic_pointer_cast<IMAPConnection>(shared_from_this()));
   }

   void 
   IMAPConnection::ParseData(std::shared_ptr<ByteBuffer> pByteBuffer)
   {
      auto iterCommandHandler = mapCommandHandlers.find(IMAP_APPEND);
      IMAPCommand* pCommand = (*iterCommandHandler).second.get();
      IMAPCommandAppend * pCommandAppend  = static_cast<IMAPCommandAppend*>(pCommand);

      pCommandAppend->ParseBinary(std::dynamic_pointer_cast<IMAPConnection>(shared_from_this()), pByteBuffer);
   }

   void
   IMAPConnection::ParseData(const AnsiString &Request)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a client IMAP command.
   //---------------------------------------------------------------------------()
   {
      if (InternalParseData(Request))
         EnqueueRead();
   }

   bool 
   IMAPConnection::InternalParseData(const AnsiString &Request)
   {
      LogClientCommand_(Request);

      bool bHasLiterals = false;
      if (!command_buffer_.IsEmpty())
         bHasLiterals = true;

      command_buffer_ += Request + "\r\n";

      int iLineEnd = command_buffer_.Find(_T("\r\n"));
      int iSpace = command_buffer_.Find(_T(" "));
      String sTag = command_buffer_.Mid(0, iSpace);
      String sCommand = command_buffer_.Mid(iSpace+1, iLineEnd - (iSpace+1));

      std::shared_ptr<IMAPClientCommand> pCommand = std::shared_ptr<IMAPClientCommand>(new IMAPClientCommand);

      // Check if we should receive any literal data.
      if (literal_data_to_receive_ == 0)
      {
         if (AskForLiteralData_(sCommand))
         {
            // The client is not permitted to send the octets of the literal unless
            // the server indicates that it expects it. 
            return true;
         }
      }
      else
      {
         // Literal data has been received. Add it to the literal buffer
         // and check whether it's enough.
         literal_buffer_ += Request;

         if (literal_buffer_.GetLength() < literal_data_to_receive_)
         {
            // Tell the client that we expects more data.
            SendAsciiData("+ Ready for additional command text.\r\n");
            return true;
         }
         else
         {
            // The entire literal buffer has been received. It could be that
            // the client wants to send another literal buffer. Check if this
            // is the case.
            String sRemaining = literal_buffer_.Mid(literal_data_to_receive_);

            // Since the existing literal buffer has been parsed, destroy it now.
            literal_buffer_ = "";

            if (AskForLiteralData_(sRemaining))
            {
               return true;
            }
         }
      }

      if (bHasLiterals)
      {
         std::vector<String> vecCommand = StringParser::SplitString(command_buffer_, "\r\n");

         auto iterLine = vecCommand.begin();
         String sLine = (*iterLine);

         // Any literals?
         int iLiteralBytesToReceive = GetLiteralSize_(sLine);
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
               iLiteralBytesToReceive = GetLiteralSize_(sRemainingOnLine);

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

      command_buffer_.Empty();

      return AnswerCommand(pCommand);
   }

   bool 
   IMAPConnection::AskForLiteralData_(const String &sInput)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Checks if we should request literal data. If we should, we do it now and
   // retnn true
   //---------------------------------------------------------------------------()
   {
      literal_data_to_receive_ = GetLiteralSize_(sInput);

      if (literal_data_to_receive_ > 0)
      {
         // Tell the client that we are accepting data.
         SendAsciiData("+ Ready for additional command text.\r\n");

         return true;  
      }

      return false;
   }

   bool
   IMAPConnection::IsReceivingLiteralDataForLoginCommand_() const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // We need to know this to be able to mask passwords in literal data.
   //---------------------------------------------------------------------------()
   {
      if (literal_data_to_receive_ > 0)
      {
         int iCommandStartPos = command_buffer_.Find(_T(" "))+1;
         if (command_buffer_.FindNoCase(_T("LOGIN")) == iCommandStartPos)
         {
            return true;
         }
      }
      
      return false;
   }

   void 
   IMAPConnection::LogClientCommand_(const String &sClientData)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Logs one client command.
   //---------------------------------------------------------------------------()
   {
      if (!Logger::Instance()->GetLogIMAP())
         return;

      String sLogData = sClientData;

      if (IsReceivingLiteralDataForLoginCommand_())
      {
         // we're receiving literal data for the login command. 
         // check if we've received the passwords yet.
         int wordsFound = 1;
         int length = command_buffer_.GetLength();
         for (int i = 0; i < command_buffer_.GetLength(); i++)
         {
            wchar_t ch = command_buffer_.GetAt(i);
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

      LOG_IMAP(GetSessionID(), GetIPAddressString(), sLogData);      
   }

   int
   IMAPConnection::GetLiteralSize_(const String &sCommand)
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
      int iFullCommandStartPos = command_buffer_.Find(_T(" "))+1;
      if (command_buffer_.FindNoCase(_T("APPEND")) == iFullCommandStartPos)
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
   IMAPConnection::Login(std::shared_ptr<const Account> account)
   {
      account_ = account;

      RefreshIMAPFolders();
   }

   void 
   IMAPConnection::Logout(const String &goodbyeMessage)
   {
      CloseCurrentFolder();

      std::shared_ptr<const Account> account;
      SetAccount_(account),

      SendAsciiData(goodbyeMessage);   

      Disconnect_();
   }

   void
   IMAPConnection::Disconnect_()
   {
      pending_disconnect_ = true;

      EnqueueDisconnect();
   }

   void
   IMAPConnection::SendAsciiData(const AnsiString & sData)
   {
      if (Logger::Instance()->GetLogIMAP())
      {
         // Let's tame these logs a bit. Disables IMAP SENT
         // logging unless debug logging enabled or LogLevel > 2
         // for lines with FETCH, STATUS or short )-only lines < 5
         String sDataTmp = sData;
         int iDataLenTmp = sDataTmp.GetLength();
         log_level_ = IniFileSettings::Instance()->GetLogLevel();

         if ((Logger::Instance()->GetLogDebug()) || (log_level_ > 2) || (!(sDataTmp.Find(_T("FETCH")) > 0) && !(sDataTmp.Find(_T("STATUS")) > 0) && iDataLenTmp >= 5))
         {
            String sLogData = _T("SENT: ") + sData;
            sLogData.TrimRight(_T("\r\n"));

            LOG_IMAP(GetSessionID(),GetIPAddressString(), sLogData);
         }
         // Logging gets skipped otherwise
      }

      EnqueueWrite(sData);
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
      else if (sCommand == _T("STARTTLS"))
         return IMAP_STARTTLS;

      return IMAP_UNKNOWN;
   }

   bool
   IMAPConnection::AnswerCommand(std::shared_ptr<IMAPClientCommand> command)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Handles a single client command.
   //---------------------------------------------------------------------------()
   {
      // Find the command handler.

      String sCommandTag = command->Tag;
      String sCommandValue = command->Command;

      int iSpace = sCommandValue.Find(_T(" "));
      String sCommandName = "";
      if (iSpace >= 0)
         sCommandName = sCommandValue.Mid(0, iSpace);
      else
         sCommandName = sCommandValue;

      if (is_idling_)
      {
         EndIdleMode_();
         
         // Remove command
         return true;
      }

      if (sCommandTag.IsEmpty() || sCommandValue.IsEmpty())
      {
         // No space found in the command line.
         SendAsciiData(sCommandValue + " BAD NULL COMMAND\r\n");
         
         // Remove this command since it's no good.
         return true;
      }

      eIMAPCommandType eCommand = GetCommandType(sCommandName);

      bool bHandlerFound = false;
      
      auto iterCommandHandler = mapCommandHandlers.find(eCommand);

      if (iterCommandHandler != mapCommandHandlers.end())
         bHandlerFound = true;
      else
      {
         // Find handler in the static space.
         iterCommandHandler = mapStaticHandlers.find(eCommand);     

         if (iterCommandHandler != mapStaticHandlers.end())
            bHandlerFound = true;
      }

      if (!bHandlerFound)
      {
         // Should never happen. If the command is not known to hMailServer, it is classified as
         // IMAP_UNKNOWN. This command is set up in the static command handlers.
         throw std::logic_error(Formatter::FormatAsAnsi("Handler for {0} was not found.", sCommandName));
      }

      
      std::shared_ptr<IMAPCommand> pCommand = (*iterCommandHandler).second;

      std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument> (new IMAPCommandArgument);
      pArgument->Command(sCommandValue);
      pArgument->Tag(sCommandTag);
      
      // Add literals.
      auto iterStr = command->vecLiteralData.begin();
      while (iterStr != command->vecLiteralData.end())
      {
         pArgument->AddLiteral(*iterStr);
         iterStr++;
      }
      
      bool postReceive = false;
      
      // Report updates on the current folder.
      if (current_folder_)
      {
         NotifyFolderChange(eCommand);
      }

      IMAPResult result = pCommand->ExecuteCommand(std::dynamic_pointer_cast<IMAPConnection>(shared_from_this()),  pArgument);

      if (result.GetResult() == IMAPResult::ResultOK)
      {
         // If a delayed notification has been specified by the command, submit it now.
         if (delayed_change_notification_)
         {
            Application::Instance()->GetNotificationServer()->SendNotification(notification_client_, delayed_change_notification_);
            delayed_change_notification_.reset();
         }

         postReceive = true;
      }
      else if (result.GetResult() == IMAPResult::ResultOKSupressRead)
      {
         /* We're either disconnecting, or we're starting a TLS session */ 
      }
      else if (result.GetResult() == IMAPResult::ResultBad)
      {
         SendAsciiData(sCommandTag + " BAD " + result.GetMessage() + "\r\n");
         postReceive = true;
      }
      else if (result.GetResult() == IMAPResult::ResultNo)
      {
         SendAsciiData(sCommandTag + " NO " + result.GetMessage() + "\r\n");
         postReceive = true;
      }
      else
      {
         throw std::logic_error(Formatter::FormatAsAnsi("Unsupported IMAP Result: {0}", result.GetResult()));
      }


      return postReceive;
   }

   void 
   IMAPConnection::EndIdleMode_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Quits idle mode.
   //---------------------------------------------------------------------------()
   {
      if (!is_idling_)
      {
         assert(0);
         return;
      }

      auto iterCommandHandler = mapCommandHandlers.find(IMAP_IDLE);
      std::shared_ptr<IMAPCommand> pCommand = (*iterCommandHandler).second;
      std::shared_ptr<IMAPCommandIdle> pIdleCommand = std::static_pointer_cast<IMAPCommandIdle>(pCommand);
      pIdleCommand->Finish(true);
   }

   void
   IMAPConnection::NotifyFolderChange(eIMAPCommandType active_command)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Notify the client about other changes which have been made.
   //---------------------------------------------------------------------------()
   {
      /*
      IMAP RFC:
      An EXPUNGE response MUST NOT be sent when no command is in
      progress, nor while responding to a FETCH, STORE, or SEARCH
      command.

      IMAP IDLE RFC:
      Untagged EXPUNGE responses are not permitted while the server is
      responding to a SORT command, but are permitted during a UID SORT
      command.
      */

      bool send_expunge = active_command != IMAP_FETCH &&
                          active_command != IMAP_STORE &&
                          active_command != IMAP_SEARCH &&
                          active_command != IMAP_SORT;

      notification_client_->SendCachedNotifications(send_expunge);
   }

   void
   IMAPConnection::RefreshIMAPFolders()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Fetches the cached folders
   //---------------------------------------------------------------------------()
   {
      imap_folders_ = IMAPFolderContainer::Instance()->GetFoldersForAccount(account_->GetID());
      public_imap_folders_ = IMAPFolderContainer::Instance()->GetPublicFolders();
   }

   std::shared_ptr<IMAPFolder> 
   IMAPConnection::GetFolderByFullPath(const String &sFolderName)
   {
      String hierarchyDelimiter = Configuration::Instance()->GetIMAPConfiguration()->GetHierarchyDelimiter();
      std::vector<String> vecFolderPath = StringParser::SplitString(sFolderName, hierarchyDelimiter);

      return GetFolderByFullPath(vecFolderPath);
   }

   std::shared_ptr<IMAPFolder> 
   IMAPConnection::GetFolderByFullPath(std::vector<String> &vecFolderPath)
   {
      std::shared_ptr<IMAPFolder> pFolder;

      if (vecFolderPath.size() > 0)
      {
         bool bIsPublicFolder = IMAPFolderUtilities::IsPublicFolder(vecFolderPath);
         if (bIsPublicFolder)
         {
            // Remove the "public folder" dummy..
            vecFolderPath.erase(vecFolderPath.begin());
         }

         // Retrieve folder
         if (imap_folders_ && !bIsPublicFolder)
         {
            pFolder = imap_folders_->GetFolderByFullPath(vecFolderPath);
            if (pFolder)
               return pFolder;
         }

         if (public_imap_folders_ && bIsPublicFolder)
         {
            pFolder = public_imap_folders_->GetFolderByFullPath(vecFolderPath);

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
      if (!current_folder_)
         return;

      notification_client_->UnsubscribeMessageChanges();

      // Set the recent flag on all messages in the folder. Since the user has been notified
      // about these messages, they are no longer recent. This doesn't happen if the folder
      // is in read-only mode - if it has been selected using the EXAMINE command.
      if (!current_folder_read_only_)
      {
         current_folder_->GetMessages()->RemoveRecentFlags();
      }

      // Unload the folder.
      current_folder_.reset();
   }

   void
   IMAPConnection::SetCurrentFolder(std::shared_ptr<IMAPFolder> pFolder, bool readOnly)
   {
      // First close the currently set folder. This will cause an unsubscribe from the 
      // current folder to be made and \recent flags to be removed.
      CloseCurrentFolder();

      // Select the new folder
      current_folder_ = pFolder;
      current_folder_read_only_ = readOnly;

      // Subscribe to changes in the new folder.
      if (current_folder_)
      {
         notification_client_->SubscribeMessageChanges(current_folder_->GetAccountID(), pFolder->GetID());
      }
   }

   void
   IMAPConnection::SetCommandBuffer(const String &sval)
   {
      command_buffer_ = sval;
   }

   void 
   IMAPConnection::SetIsIdling(bool bNewVal)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Switch in or out idling mode. 
   //---------------------------------------------------------------------------()
   {
      is_idling_ = bNewVal;
   }

   bool
   IMAPConnection::GetIsIdling() const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Switch in or out idling mode. 
   //---------------------------------------------------------------------------()
   {
      return is_idling_;
   }

   void 
   IMAPConnection::SetDelayedChangeNotification(std::shared_ptr<ChangeNotification> pNotification)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Sets a delayed change notification. This will be performed when the
   // current command is being replied to.
   //---------------------------------------------------------------------------()
   {
      delayed_change_notification_ = pNotification;
   }

   bool 
   IMAPConnection::CheckPermission(std::shared_ptr<IMAPFolder> pFolder, int iPermission)
   {
      if (!Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPACL())
      {
         // IMAP ACL has been disabled. Allow everything.
         return true;
      }

	   ACLManager aclManager;
      std::shared_ptr<ACLPermission> pPermission = aclManager.GetPermissionForFolder(account_->GetID(), pFolder);
      if (!pPermission)
         return false;

      if (pPermission->GetAllow((ACLPermission::ePermission) iPermission))
         return true;

      return false;
   }

   void
   IMAPConnection::CheckFolderPermissions(std::shared_ptr<IMAPFolder> pFolder, bool &readAccess, bool &writeAccess)
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
      std::shared_ptr<ACLPermission> pPermission = aclManager.GetPermissionForFolder(account_->GetID(), pFolder);
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
      return account_ != 0;
   }

   void
   IMAPConnection::StartHandshake()
   {
      EnqueueHandshake();
   }

   void 
   IMAPConnection::SetRecentMessages(const std::set<__int64> &messages)
   {
      recent_messages_ = messages;
   }

   std::set<__int64>& 
   IMAPConnection::GetRecentMessages()
   {
      return recent_messages_;
   }
}

