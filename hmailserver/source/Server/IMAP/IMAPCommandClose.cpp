// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandClose.h"
#include "IMAPConnection.h"

#include "MessagesContainer.h"

#include "../Common/BO/Messages.h"
#include "../Common/BO/Message.h"
#include "../Common/Tracking/ChangeNotification.h"
#include "../Common/Tracking/NotificationServer.h"

#include "../Common/BO/ACLPermission.h"
#include "../Common/BO/IMAPFolder.h"
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPResult
   IMAPCommandCLOSE::ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");
      
      // Iterate through mail boxes and delete messages marked for deletion.
      std::shared_ptr<IMAPFolder> pCurFolder = pConnection->GetCurrentFolder();   

      if (!pCurFolder)
         return IMAPResult(IMAPResult::ResultBad, "No folder selected.");

      // Check if user has permission to expunge. If so, do that.
      if (!pConnection->GetCurrentFolderReadOnly() &&
          pConnection->CheckPermission(pConnection->GetCurrentFolder(), ACLPermission::PermissionExpunge))
      {
         std::function<bool(std::shared_ptr<Message>)> filter = [](std::shared_ptr<Message> message)
            {
               return message->GetFlagDeleted();
            };

         auto messages = MessagesContainer::Instance()->GetMessages(pCurFolder->GetAccountID(), pCurFolder->GetID());
         auto deleted_message_ids = messages->DeleteMessages(filter);

         if (!deleted_message_ids.empty())
         {
            // CLOSE sends no EXPUNGE responses to this session (RFC 3501 6.4.2), but the other
            // sessions still need to know that the messages are gone.
            std::shared_ptr<ChangeNotification> pNotification =
               std::shared_ptr<ChangeNotification>(new ChangeNotification(pCurFolder->GetAccountID(), pCurFolder->GetID(), ChangeNotification::NotificationMessageDeleted, deleted_message_ids));

            Application::Instance()->GetNotificationServer()->SendNotification(pConnection->GetNotificationClient(), pNotification);
         }
      }
      
      pConnection->CloseCurrentFolder();

      String sResponse = pArgument->Tag() + " OK CLOSE completed\r\n";
      pConnection->SendAsciiData(sResponse);  

      return IMAPResult();

   }
}