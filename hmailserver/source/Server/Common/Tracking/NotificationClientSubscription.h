// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class NotificationClient;

   class NotificationClientSubscription
   {
   public:
      NotificationClientSubscription(__int64 subscriptionKey, weak_ptr<NotificationClient> client);
      virtual ~NotificationClientSubscription();

      __int64 GetSubscriptionKey();
      weak_ptr<NotificationClient> GetSubscribedClient();

   private:

      __int64 _subscriptionKey;
      weak_ptr<NotificationClient> _client;

   };
}
