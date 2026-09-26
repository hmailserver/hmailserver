// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandExpunge.h"
#include "IMAPCommandRangeAction.h"
#include "IMAPConnection.h"

#include "MessagesContainer.h"
#include "IMAPFolderView.h"
#include "IMAPNotificationClient.h"

#include "../Common/BO/Messages.h"
#include "../Common/BO/Message.h"

#include "../Common/BO/IMAPFolder.h"

#include "../Common/Tracking/ChangeNotification.h"
#include "../Common/Tracking/NotificationServer.h"
#include "../Common/BO/ACLPermission.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCommandEXPUNGE::IMAPCommandEXPUNGE() :
      is_uid_(false)
   {

   }

   IMAPCommandEXPUNGE::IMAPCommandEXPUNGE(const String &uid_set) :
      is_uid_(true),
      uid_set_(uid_set)
   {

   }

   IMAPResult
   IMAPCommandEXPUNGE::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      if (pConnection->GetCurrentFolderReadOnly())
      {
         return IMAPResult(IMAPResult::ResultNo, "Expunge command on read-only folder.");
      }

      // Iterate through mail boxes and delete messages marked for deletion.
      std::shared_ptr<IMAPFolder> pCurFolder = pConnection->GetCurrentFolder();   

      if (!pCurFolder)
         return IMAPResult(IMAPResult::ResultNo, "No folder selected.");

      if (!pConnection->CheckPermission(pCurFolder, ACLPermission::PermissionExpunge))
         return IMAPResult(IMAPResult::ResultBad, "ACL: Expunge permission denied (Required for EXPUNGE command).");

      auto view = pConnection->GetCurrentFolderView();

      if (!view)
         return IMAPResult(IMAPResult::ResultNo, "No folder selected.");

      auto messages = MessagesContainer::Instance()->GetMessages(pCurFolder->GetAccountID(), pCurFolder->GetID());

      // EXPUNGE may report new messages as well, so take them into the view first. The client
      // must be told about them before any EXPUNGE refers to their sequence numbers.
      if (view->AppendNewMessages(messages) > 0)
      {
         pConnection->SendAsciiData(IMAPNotificationClient::GenerateExistsString(view->GetMessageCount()) +
                                    IMAPNotificationClient::GenerateRecentString((int) pConnection->GetRecentMessageCount()));
      }

      std::set<__int64> messages_in_uid_set;
      if (is_uid_)
      {
         std::vector<std::pair<int, IMAPViewEntry>> targets;

         if (!IMAPCommandRangeAction::ResolveTargets(view, uid_set_, true, targets))
            return IMAPResult(IMAPResult::ResultBad, "Incorrect message set.");

         for (const auto &target : targets)
            messages_in_uid_set.insert(target.second.message_id);
      }

      auto entries = view->GetAllEntries();

      // Only messages this session knows about may be expunged. It hasn't been told about the
      // others, so their sequence numbers would mean nothing to the client.
      std::set<__int64> view_message_ids;
      std::set<__int64> candidate_ids;

      for (const auto &entry : entries)
      {
         view_message_ids.insert(entry.second.message_id);

         if (!is_uid_ || messages_in_uid_set.find(entry.second.message_id) != messages_in_uid_set.end())
            candidate_ids.insert(entry.second.message_id);
      }

      // Copies are only needed for the messages whose flags are read.
      auto candidates = messages->GetCopyByIds(candidate_ids);

      std::map<__int64, std::shared_ptr<Message>> live_messages;
      if (is_uid_)
         live_messages = messages->GetItemsByIds(view_message_ids);

      const auto &existing_messages = is_uid_ ? live_messages : candidates;

      std::set<__int64> messages_to_delete;

      for (const auto &entry : entries)
      {
         if (existing_messages.find(entry.second.message_id) == existing_messages.end())
         {
            // Expunged by another session. The client is told about the expunge the next
            // time we're allowed to send one.
            view->MarkVanished(entry.second.message_id);
            continue;
         }

         auto iter = candidates.find(entry.second.message_id);

         if (iter != candidates.end() && iter->second->GetFlagDeleted())
            messages_to_delete.insert(entry.second.message_id);
      }

      auto deleted_message_ids = messages->DeleteMessagesById(messages_to_delete);
      auto expunged_sequences = view->RemoveMessages(deleted_message_ids);

      pConnection->RemoveRecentMessages(deleted_message_ids);

      String sResponse;
      for (int sequence : expunged_sequences)
      {
         String sTemp;
         sTemp.Format(_T("* %d EXPUNGE\r\n"), sequence);
         sResponse += sTemp;
      }

      pConnection->SendAsciiData(sResponse);

      if (!deleted_message_ids.empty())
      {
         // Messages have been expunged
         // Notify the mailbox notifier that the mailbox contents have changed. The view is
         // updated first, and no connection lock is held: the notification is delivered
         // synchronously on this thread, into the other connections.
         std::shared_ptr<ChangeNotification> pNotification = 
            std::shared_ptr<ChangeNotification>(new ChangeNotification(pCurFolder->GetAccountID(), pCurFolder->GetID(), ChangeNotification::NotificationMessageDeleted, deleted_message_ids));

         Application::Instance()->GetNotificationServer()->SendNotification(pConnection->GetNotificationClient(), pNotification);
      }

      pConnection->SendAsciiData(pArgument->Tag() + (is_uid_ ? " OK UID EXPUNGE completed\r\n" : " OK EXPUNGE Completed\r\n"));

      return IMAPResult();
   }
}