// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandExpunge.h"
#include "IMAPConnection.h"

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
   IMAPCommandEXPUNGE::ExecuteCommand(shared_ptr<IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      if (pConnection->GetCurrentFolderReadOnly())
      {
         return IMAPResult(IMAPResult::ResultNo, "Expunge command on read-only folder.");
      }

      // Iterate through mail boxes and delete messages marked for deletion.
      shared_ptr<IMAPFolder> pCurFolder = pConnection->GetCurrentFolder();   

      if (!pCurFolder)
         return IMAPResult(IMAPResult::ResultNo, "No folder selected.");

      if (!pConnection->CheckPermission(pCurFolder, ACLPermission::PermissionExpunge))
         return IMAPResult(IMAPResult::ResultBad, "ACL: Expunge permission denied (Required for EXPUNGE command).");

      std::vector<int> vecExpungedMessages = pCurFolder->Expunge();
      std::vector<int>::iterator iterExpunged = vecExpungedMessages.begin();

      String sResponse;
      while (iterExpunged != vecExpungedMessages.end())
      {
         String sTemp;
         sTemp.Format(_T("* %d EXPUNGE\r\n"), (*iterExpunged));
         sResponse += sTemp;
         iterExpunged++;
      }

      pConnection->SendAsciiData(sResponse);

      if (!vecExpungedMessages.empty())
      {
         std::vector<__int64> affectedMessages;
         boost_foreach(__int64 messageIndex, vecExpungedMessages)
         {
            affectedMessages.push_back(messageIndex);
         }

         // Messages have been expunged
         // Notify the mailbox notifier that the mailbox contents have changed.
         shared_ptr<ChangeNotification> pNotification = 
            shared_ptr<ChangeNotification>(new ChangeNotification(pCurFolder->GetAccountID(), pCurFolder->GetID(),  ChangeNotification::NotificationMessageDeleted, affectedMessages));

         Application::Instance()->GetNotificationServer()->SendNotification(pConnection->GetNotificationClient(), pNotification);
      }


      // We're done.
      sResponse = pArgument->Tag() + " OK EXPUNGE Completed\r\n";
      pConnection->SendAsciiData(sResponse);   

      return IMAPResult();
   }
}