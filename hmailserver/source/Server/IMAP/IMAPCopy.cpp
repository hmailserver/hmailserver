// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCopy.h"
#include "IMAPConnection.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/Account.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/Persistence/PersistentMessage.h"
#include "IMAPSimpleCommandParser.h"
#include "../Common/BO/ACLPermission.h"
#include "../Common/Tracking/ChangeNotification.h"
#include "../Common/Tracking/NotificationServer.h"
#include "../Common/BO/Messages.h"
#include "IMAPFolderView.h"
#include "IMAPNotificationClient.h"


#include "MessagesContainer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCopy::IMAPCopy()
   {

   }

   String
   IMAPCopy::GetResponseCode() const
   {
      // A client that can't read the destination must not learn its UIDs (RFC 4315 6).
      if (source_uids_.empty() || !destination_readable_)
         return String();

      String sResponseCode;
      sResponseCode.Format(_T("[COPYUID %u "), destination_folder_->GetUIDValidity());
      sResponseCode += FormatUIDSet_(source_uids_);
      sResponseCode += _T(" ");
      sResponseCode += FormatUIDSet_(destination_uids_);
      sResponseCode += _T("] ");

      return sResponseCode;
   }

   String
   IMAPCopy::GetUntaggedResponse(std::shared_ptr<IMAPConnection> pConnection)
   {
      if (copied_message_ids_.empty())
         return String();

      auto current_folder = pConnection->GetCurrentFolder();
      if (!current_folder || current_folder->GetID() != destination_folder_->GetID())
         return String();

      std::shared_ptr<Messages> messages = destination_folder_->GetMessages();

      // The copies are appended at the end, so this session's existing numbering is unaffected.
      auto view = pConnection->GetCurrentFolderView();
      if (view)
         view->AppendNewMessages(messages);

      int message_count = view ? view->GetMessageCount() : messages->GetCount();

      return IMAPNotificationClient::GenerateExistsString(message_count) +
             IMAPNotificationClient::GenerateRecentString((int) pConnection->GetRecentMessageCount());
   }

   String
   IMAPCopy::FormatUIDSet_(const std::vector<unsigned int> &uids)
   {
      // The order must be kept, since the two sets correspond element by element. So only
      // ascending runs of consecutive UIDs are written as ranges.
      String sResult;

      size_t start = 0;
      while (start < uids.size())
      {
         size_t end = start;
         while (end + 1 < uids.size() && uids[end + 1] == uids[end] + 1)
            end++;

         String sPart;
         if (end > start)
            sPart.Format(_T("%u:%u"), uids[start], uids[end]);
         else
            sPart.Format(_T("%u"), uids[start]);

         if (!sResult.IsEmpty())
            sResult += _T(",");

         sResult += sPart;

         start = end + 1;
      }

      return sResult;
   }


   IMAPResult
   IMAPCopy::DoAction(std::shared_ptr<IMAPConnection> pConnection, int messageIndex, std::shared_ptr<Message> pOldMessage, const std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pArgument || !pOldMessage)
         return IMAPResult(IMAPResult::ResultBad, "Invalid parameters");
      
      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());

      pParser->Parse(pArgument);
      
      if (pParser->WordCount() <= 0)
         return IMAPResult(IMAPResult::ResultNo, "The command requires parameters.");

      String sFolderName;
      if (pParser->Word(0)->Clammerized())
         sFolderName = pArgument->Literal(0);
      else
      {
         sFolderName = pParser->Word(0)->Value();
         IMAPFolder::UnescapeFolderString(sFolderName);
      }

      std::shared_ptr<IMAPFolder> pFolder = pConnection->GetFolderByFullPath(sFolderName);
      if (!pFolder)
         return IMAPResult(IMAPResult::ResultBad, "The folder could not be found.");

      std::shared_ptr<const Account> pAccount = pConnection->GetAccount();

      if (!pFolder->IsPublicFolder())
      {
         if (!pAccount->SpaceAvailable(pOldMessage->GetSize()))
            return IMAPResult(IMAPResult::ResultNo, "Your quota has been exceeded.");
      }

      // Check if the user has permission to copy to this destination folder
      if (!pConnection->CheckPermission(pFolder, ACLPermission::PermissionInsert))
         return IMAPResult(IMAPResult::ResultBad, "ACL: Insert permission denied (Required for COPY command).");

      std::shared_ptr<Message> pNewMessage = PersistentMessage::CopyToIMAPFolder(pAccount, pOldMessage, pFolder);

      if (!pNewMessage)
         return IMAPResult(IMAPResult::ResultBad, "Failed to copy message");

      // Check if the user has access to set the Seen flag, otherwise 
      if (!pConnection->CheckPermission(pFolder, ACLPermission::PermissionWriteSeen))
         pNewMessage->SetFlagSeen(false);  

      if (!PersistentMessage::SaveObject(pNewMessage))
      {
         // The file was copied, but no message refers to it.
         PersistentMessage::DeleteFile(pAccount, pNewMessage);
         return IMAPResult(IMAPResult::ResultBad, "Failed to save copy of message.");
      }

      destination_folder_ = pFolder;
      destination_readable_ = pConnection->CheckPermission(pFolder, ACLPermission::PermissionRead);
      source_uids_.push_back(pOldMessage->GetUID());
      destination_uids_.push_back(pNewMessage->GetUID());
      copied_message_ids_.insert(pNewMessage->GetID());

      MessagesContainer::Instance()->SetFolderNeedsRefresh(pFolder->GetID());

      // Set a delayed notification so that the any IMAP idle client is notified when this
      // command has been finished.
      std::shared_ptr<ChangeNotification> pNotification = 
         std::shared_ptr<ChangeNotification>(new ChangeNotification(pFolder->GetAccountID(), pFolder->GetID(), ChangeNotification::NotificationMessageAdded));

      pConnection->SetDelayedChangeNotification(pNotification);

      return IMAPResult();
   }

   void
   IMAPCopy::RollBack(std::shared_ptr<IMAPConnection> pConnection)
   {
      if (copied_message_ids_.empty())
         return;

      pConnection->SetDelayedChangeNotification(std::shared_ptr<ChangeNotification>());

      auto messages = MessagesContainer::Instance()->GetMessages(destination_folder_->GetAccountID(), destination_folder_->GetID());
      auto deleted_message_ids = messages->DeleteMessagesById(copied_message_ids_);

      // Other sessions may already have been told about the copies.
      if (!deleted_message_ids.empty())
      {
         std::shared_ptr<ChangeNotification> pNotification =
            std::shared_ptr<ChangeNotification>(new ChangeNotification(destination_folder_->GetAccountID(), destination_folder_->GetID(), ChangeNotification::NotificationMessageDeleted, deleted_message_ids));

         Application::Instance()->GetNotificationServer()->SendNotification(pConnection->GetNotificationClient(), pNotification);
      }

      source_uids_.clear();
      destination_uids_.clear();
      copied_message_ids_.clear();
   }
}
