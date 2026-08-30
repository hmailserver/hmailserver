// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandExpunge.h"
#include "IMAPConnection.h"

#include "MessagesContainer.h"
#include "IMAPFolderView.h"

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

      // EXPUNGE may report new messages as well, so take them into the view first.
      view->AppendNewMessages(messages);

      // Only messages this session knows about may be expunged. It hasn't been told about the
      // others, so their sequence numbers would mean nothing to the client.
      std::set<__int64> messages_to_delete;

      for (const auto &entry : view->GetAllEntries())
      {
         auto message = messages->GetItemByDBID(entry.second.message_id);

         if (message && message->GetFlagDeleted())
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

      // We're done.
      sResponse = pArgument->Tag() + " OK EXPUNGE Completed\r\n";
      pConnection->SendAsciiData(sResponse);   

      return IMAPResult();
   }
}