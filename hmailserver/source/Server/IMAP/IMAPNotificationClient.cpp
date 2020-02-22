// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPNotificationClient.h"
#include "IMAPConnection.h"
#include "IMAPStore.h"

#include "../Common/Tracking/ChangeNotification.h"
#include "../common/Tracking/NotificationServer.h"

#include "../Common/BO/Messages.h"
#include "../Common/BO/IMAPFolder.h"

#include "../Common/TCPIP/DisconnectedException.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPNotificationClient::IMAPNotificationClient() :
      message_change_subscription_id_(0),
      folder_list_change_subscription_id_(0),
      account_id_(0),
      folder_id_(0)
   {

   }

   IMAPNotificationClient::~IMAPNotificationClient()
   {
      try
      {
         if (folder_list_change_subscription_id_ > 0)
         {
            std::shared_ptr<NotificationServer> notificationServer = Application::Instance()->GetNotificationServer();
            notificationServer->UnsubscribeFolderListChanges(account_id_, folder_list_change_subscription_id_);
         }
      }
      catch (...)
      {

      }
   }

   void 
   IMAPNotificationClient::SubscribeMessageChanges(__int64 accountID, __int64 folderID)
   {
      assert(accountID >= 0);
      assert(folderID > 0);

      account_id_ = accountID;
      folder_id_ = folderID;

      std::shared_ptr<NotificationServer> notificationServer = Application::Instance()->GetNotificationServer();
      message_change_subscription_id_ = notificationServer->SubscribeMessageChanges(account_id_, folder_id_, shared_from_this());
   }

   void
   IMAPNotificationClient::UnsubscribeMessageChanges()
   {
      assert(account_id_ >= 0);
      assert(folder_id_ > 0);
      assert(message_change_subscription_id_ > 0);

      // Since we don't want to look at he folder any more,
      // we're not interested in any updates.
      std::shared_ptr<NotificationServer> notificationServer = Application::Instance()->GetNotificationServer();
      notificationServer->UnsubscribeMessageChanges(account_id_, folder_id_, message_change_subscription_id_);

      // If there are cached updates for this folder but the client
      // don't want to look at the folder any more, the cached updates
      // will be gone.
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);
      cached_changes_.clear();
   }

   void 
   IMAPNotificationClient::SetConnection(std::weak_ptr<IMAPConnection> connection)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Called by the mailbox change notifier when something has happened to the mailbox.
   //---------------------------------------------------------------------------()
   {
      parent_connection_ = connection;
   }

   void 
   IMAPNotificationClient::OnNotification(std::shared_ptr<ChangeNotification> notification)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Called by the mailbox change notifier when something has happened to the mailbox.
   //---------------------------------------------------------------------------()
   {
      std::shared_ptr<IMAPConnection> parentConnection = parent_connection_.lock();

      if (!parentConnection)
         return;

      if (parentConnection->GetIsIdling())
      {
         try
         {
            SendChangeNotification_(notification);
         }
         catch (DisconnectedException&)
         {
            // We were unable to send the notifications to the client, because he has disconnected.
            // This is normal behavior, and not an error we want to log.
         }
      }
      else
         CacheChangeNotification_(notification);
   }

   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Cache this change. We'll send a notification later on.
   //---------------------------------------------------------------------------()
   void 
   IMAPNotificationClient::CacheChangeNotification_(std::shared_ptr<ChangeNotification> pChangeNotification)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);
      cached_changes_.push_back(pChangeNotification);
   }

   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Send a summary of all changes to the client...
   //---------------------------------------------------------------------------()
   void 
   IMAPNotificationClient::SendCachedNotifications(bool send_expunge)
   {
      std::shared_ptr<IMAPConnection> connection = parent_connection_.lock();

      if (!connection)
         return;

      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      int lastExists = -1;
      int lastRecent = -1;

      std::set<__int64> flagMessages;

      for(std::shared_ptr<ChangeNotification> changeNotification : cached_changes_)
      {
         switch (changeNotification->GetType())
         {
         case ChangeNotification::NotificationMessageAdded:
            {
               std::shared_ptr<Messages> pMessages = connection->GetCurrentFolder()->GetMessages();
               pMessages->Refresh(false);
               lastExists = pMessages->GetCount();
               lastRecent = (int)connection->GetRecentMessages().size();
               break;
            }
         case ChangeNotification::NotificationMessageDeleted:
            {
               if (send_expunge)
               {
                  // Send EXPUNGE
                  SendEXPUNGE_(changeNotification->GetAffectedMessages());

                  // Send EXISTS
                  std::shared_ptr<Messages> pMessages = connection->GetCurrentFolder()->GetMessages();
                  lastExists = pMessages->GetCount();
                  lastRecent = (int)connection->GetRecentMessages().size();

                  break;
               }
            }
         case ChangeNotification::NotificationMessageFlagsChanged:
            {
               // Send flag notification
               for(__int64 messageID : changeNotification->GetAffectedMessages())
               {
                  if (flagMessages.find(messageID) == flagMessages.end())
                     flagMessages.insert(messageID);
               }

               break;
            }
         }
      }

      if (flagMessages.size() > 0)
         SendFLAGS_(flagMessages);
      
      if (lastExists >= 0)
         SendEXISTS_(lastExists);

      if (lastRecent >= 0)
         SendRECENT_(lastExists);

      std::vector<std::shared_ptr<ChangeNotification> >::iterator iter = cached_changes_.begin();
      
      for (; iter != cached_changes_.end();)
      {
         std::shared_ptr<ChangeNotification> changeNotification = (*iter);

         switch (changeNotification->GetType())
         {
         case ChangeNotification::NotificationMessageDeleted:
            if (!send_expunge)
            {
               iter++;
               continue;
            }
         }

         iter = cached_changes_.erase(iter);
      }
   }

   void 
   IMAPNotificationClient::SendChangeNotification_(std::shared_ptr<ChangeNotification> pChangeNotification)
   {
      std::shared_ptr<IMAPConnection> connection = parent_connection_.lock();
      if (!connection)
         return;

      switch (pChangeNotification->GetType())
      {
      case ChangeNotification::NotificationMessageAdded:
         {
               std::shared_ptr<Messages> pMessages = connection->GetCurrentFolder()->GetMessages();
            SendEXISTS_(pMessages->GetCount());
            SendRECENT_((int)connection->GetRecentMessages().size());
            break;
         }
      case ChangeNotification::NotificationMessageDeleted:
         {
            // Send EXPUNGE
            SendEXPUNGE_(pChangeNotification->GetAffectedMessages());

            // Send EXISTS
               std::shared_ptr<Messages> pMessages = connection->GetCurrentFolder()->GetMessages();
            SendEXISTS_(pMessages->GetCount());
            SendRECENT_((int)connection->GetRecentMessages().size());

            break;
         }
      case ChangeNotification::NotificationMessageFlagsChanged:
         {
            // Send flag notification
            std::set<__int64> affectedMessages;
               for(__int64 messageID : pChangeNotification->GetAffectedMessages())
            {
               affectedMessages.insert(messageID);
            }
           
            SendFLAGS_(affectedMessages);

            break;
         }
      }
   }

   void 

   IMAPNotificationClient::SendEXPUNGE_(const std::vector<__int64> & vecMessages)
   {
      std::shared_ptr<IMAPConnection> connection = parent_connection_.lock();
      if (!connection)
         return;

      String sResponse;
      for(__int64 messageIndex : vecMessages)
         sResponse.AppendFormat(_T("* %I64d EXPUNGE\r\n"), messageIndex);

      connection->SendAsciiData(sResponse);

   }

   void 
   IMAPNotificationClient::SendFLAGS_(const std::set<__int64> & vecMessages)
   {
      std::shared_ptr<IMAPConnection> connection = parent_connection_.lock();
      if (!connection)
         return;

      for(__int64 messageID : vecMessages)
      {
         String sResponse;

         int foundIndex = 0;
         std::shared_ptr<Message> pMessage = connection->GetCurrentFolder()->GetMessages()->GetItemByDBID(messageID, foundIndex);

         if (!pMessage)
            return;

         connection->SendAsciiData(IMAPStore::GetMessageFlags(pMessage, foundIndex));
      }


   }


   void 
   IMAPNotificationClient::SendEXISTS_(int iExists)
   {
      std::shared_ptr<IMAPConnection> connection = parent_connection_.lock();
      if (!connection)
         return;

      String sResponse = GenerateExistsString(iExists);
      connection->SendAsciiData(sResponse);
   }

   void 
   IMAPNotificationClient::SendRECENT_(int recent)
   {
      std::shared_ptr<IMAPConnection> connection = parent_connection_.lock();
      if (!connection)
         return;

      String sResponse = GenerateRecentString(recent);

      connection->SendAsciiData(sResponse);
   }

   String
   IMAPNotificationClient::GenerateRecentString(int recent)
   {
      return Formatter::Format("* {0} RECENT\r\n", recent);
   }

   String
   IMAPNotificationClient::GenerateExistsString(int exists)
   {
      return Formatter::Format("* {0} EXISTS\r\n", exists);
   }
}
