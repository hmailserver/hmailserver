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

      void SendNotification(shared_ptr<ChangeNotification> pChangeNotification);
      void SendNotification(shared_ptr<NotificationClient> source, shared_ptr<ChangeNotification> pChangeNotification);

      __int64 SubscribeMessageChanges(__int64 accountID, __int64 folderID, shared_ptr<NotificationClient> pChangeClient);
      void UnsubscribeMessageChanges(__int64 accountID, __int64 folderID, __int64 subscriptionKey);

      __int64 SubscribeFolderListChanges(__int64 accountID, shared_ptr<NotificationClient> pChangeClient);
      void UnsubscribeFolderListChanges(__int64 accountID, __int64 subscriptionKey);

   private:

      set<shared_ptr<NotificationClient> > _GetClientsToNotify(shared_ptr<NotificationClient> source, shared_ptr<ChangeNotification> pChangeNotification);

      std::multimap<std::pair<__int64, __int64>, shared_ptr<NotificationClientSubscription> > _messageChangeSubscribers;
      std::multimap<__int64, shared_ptr<NotificationClientSubscription> > _folderListChangeSubscribers;

      CriticalSection _criticalSection;

      __int64 _subscriptionCounter;
   };
}
