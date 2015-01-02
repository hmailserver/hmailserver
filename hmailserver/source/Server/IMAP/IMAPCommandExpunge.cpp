// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandExpunge.h"
#include "IMAPConnection.h"

#include "MessagesContainer.h"

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

      std::vector<__int64> expunged_messages_uid;
      std::vector<__int64> expunged_messages_index;

      std::function<bool(int, std::shared_ptr<Message>)> filter = [&expunged_messages_index, &expunged_messages_uid](int index, std::shared_ptr<Message> message)
      {
         if (message->GetFlagDeleted())
         {
            expunged_messages_index.push_back(index);
            expunged_messages_uid.push_back(message->GetID());
            return true;
         }

         return false;
      };

      auto messages = MessagesContainer::Instance()->GetMessages(pCurFolder->GetAccountID(), pCurFolder->GetID());
      messages->DeleteMessages(filter);

      auto iterExpunged = expunged_messages_index.begin();

      String sResponse;
      while (iterExpunged != expunged_messages_index.end())
      {
         String sTemp;
         sTemp.Format(_T("* %d EXPUNGE\r\n"), (*iterExpunged));
         sResponse += sTemp;
         iterExpunged++;
      }

      pConnection->SendAsciiData(sResponse);

      if (!expunged_messages_uid.empty())
      {
         auto recent_messages = pConnection->GetRecentMessages();

         for (__int64 messageUid : expunged_messages_uid)
         {
            auto recent_messages_it = recent_messages.find(messageUid);
            if (recent_messages_it != recent_messages.end())
               recent_messages.erase(recent_messages_it);
         }
         

         // Messages have been expunged
         // Notify the mailbox notifier that the mailbox contents have changed.
         std::shared_ptr<ChangeNotification> pNotification = 
            std::shared_ptr<ChangeNotification>(new ChangeNotification(pCurFolder->GetAccountID(), pCurFolder->GetID(), ChangeNotification::NotificationMessageDeleted, expunged_messages_index));

         Application::Instance()->GetNotificationServer()->SendNotification(pConnection->GetNotificationClient(), pNotification);
      }


      // We're done.
      sResponse = pArgument->Tag() + " OK EXPUNGE Completed\r\n";
      pConnection->SendAsciiData(sResponse);   

      return IMAPResult();
   }
}