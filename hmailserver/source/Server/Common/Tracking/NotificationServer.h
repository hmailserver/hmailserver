// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class NotificationClient;
   class ChangeNotification;
   class NotificationClientSubscription;

   class NotificationServer
   {
   public:
      NotificationServer();

      void SendNotification(std::shared_ptr<ChangeNotification> pChangeNotification);
      void SendNotification(std::shared_ptr<NotificationClient> source, std::shared_ptr<ChangeNotification> pChangeNotification);

      __int64 SubscribeMessageChanges(__int64 accountID, __int64 folderID, std::shared_ptr<NotificationClient> pChangeClient);
      void UnsubscribeMessageChanges(__int64 accountID, __int64 folderID, __int64 subscriptionKey);

      __int64 SubscribeFolderListChanges(__int64 accountID, std::shared_ptr<NotificationClient> pChangeClient);
      void UnsubscribeFolderListChanges(__int64 accountID, __int64 subscriptionKey);

   private:

      std::set<std::shared_ptr<NotificationClient> > GetClientsToNotify_(std::shared_ptr<NotificationClient> source, std::shared_ptr<ChangeNotification> pChangeNotification);

      std::multimap<std::pair<__int64, __int64>, std::shared_ptr<NotificationClientSubscription> > message_change_subscribers_;
      std::multimap<__int64, std::shared_ptr<NotificationClientSubscription> > folder_list_change_subscribers_;

      boost::recursive_mutex mutex_;

      __int64 subscription_counter_;
   };
}
