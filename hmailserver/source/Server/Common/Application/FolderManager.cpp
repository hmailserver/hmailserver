// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "FolderManager.h"
#include "../BO/Account.h"
#include "../BO/IMAPFolders.h"
#include "../BO/IMAPFolder.h"

#include "../../IMAP/IMAPFolderContainer.h"
#include "../../IMAP/MessagesContainer.h"
#include "../../IMAP/IMAPConfiguration.h"
#include "../../IMAP/IMAPFolderUtilities.h"

#include "../Tracking/ChangeNotification.h"
#include "../Tracking/NotificationServer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   FolderManager::FolderManager(void)
   {

   }

   FolderManager::~FolderManager(void)
   {

   }

   bool
   FolderManager::GetInboxMessages(int accountID, std::vector<std::shared_ptr<Message>> &result)
   {
      std::shared_ptr<IMAPFolders> folders = IMAPFolderContainer::Instance()->GetFoldersForAccount(accountID);
      if (!folders)
         return false;

      std::shared_ptr<IMAPFolder> inbox = folders->GetItemByName("Inbox");
      if (!inbox)
         return false;

      // get folder lock.
      std::set<__int64> recent_messages;
      auto messages = MessagesContainer::Instance()->GetMessages(inbox->GetAccountID(), inbox->GetID(), recent_messages, false);

      result = messages->GetCopy();

      return true;
   }

   bool 
   FolderManager::DeleteInboxMessages(int accountID, std::set<int> uids, const std::function<void()> &callbackEvery1000Message)
   {
      std::shared_ptr<IMAPFolders> folders = IMAPFolderContainer::Instance()->GetFoldersForAccount(accountID);
      if (!folders)
         return false;

      std::shared_ptr<IMAPFolder> folder = folders->GetFolderByName("Inbox", false);
      if (!folder)
         return false;

      __int64 inboxID = folder->GetID();

      std::vector<__int64> affectedMessages;

      int count = 0;

      std::function<bool(int, std::shared_ptr<Message>)> filter = [&uids, &affectedMessages, &count, &callbackEvery1000Message](int index, std::shared_ptr<Message> message)
         {
            count++;

            if ((count % 1000) == 0)
            {
               callbackEvery1000Message();
            }

            if (uids.find(message->GetUID()) != uids.end())
            {
               affectedMessages.push_back(index);
               return true;
            }
            else
            {
               return false;
            }
         };

      auto messages = MessagesContainer::Instance()->GetMessages(folder->GetAccountID(), folder->GetID());
      messages->DeleteMessages(filter);
      
      if (affectedMessages.size() > 0)
      {
         // Notify the mailbox notifier that the mailbox contents have changed.
         std::shared_ptr<ChangeNotification> pNotification = 
            std::shared_ptr<ChangeNotification>(new ChangeNotification(accountID, inboxID, ChangeNotification::NotificationMessageDeleted, affectedMessages));

         Application::Instance()->GetNotificationServer()->SendNotification(pNotification);
      }


      return true;
   }

   bool 
   FolderManager::UpdateMessageFlags(int accountID, int folderID, __int64 messageID, int flags)
   {
      std::shared_ptr<IMAPFolders> folders;
      if (accountID > 0)
         folders = IMAPFolderContainer::Instance()->GetFoldersForAccount(accountID);
      else
         folders = IMAPFolderContainer::Instance()->GetPublicFolders();

      if (!folders)
         return false;

      std::shared_ptr<IMAPFolder> folder = folders->GetItemByDBIDRecursive(folderID);
      if (!folder)
         return false;

      std::shared_ptr<Message> message = folder->GetMessages()->GetItemByDBID(messageID);

      if (!message)
         return false;

      message->SetFlags(flags);
      return PersistentMessage::SaveFlags(message);
   }


   
} 