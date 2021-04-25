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
         return IMAPResult(IMAPResult::ResultBad, "Failed to save copy of message.");

      MessagesContainer::Instance()->SetFolderNeedsRefresh(pFolder->GetID());

      // Set a delayed notification so that the any IMAP idle client is notified when this
      // command has been finished.
      std::shared_ptr<ChangeNotification> pNotification = 
         std::shared_ptr<ChangeNotification>(new ChangeNotification(pFolder->GetAccountID(), pFolder->GetID(), ChangeNotification::NotificationMessageAdded));

      pConnection->SetDelayedChangeNotification(pNotification);

      return IMAPResult();
   }
}
