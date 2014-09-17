// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class NotificationClient;

   class NotificationClientSubscription
   {
   public:
      NotificationClientSubscription(__int64 subscriptionKey, std::weak_ptr<NotificationClient> client);
      virtual ~NotificationClientSubscription();

      __int64 GetSubscriptionKey();
      std::weak_ptr<NotificationClient> GetSubscribedClient();

   private:

      __int64 subscription_key_;
      std::weak_ptr<NotificationClient> client_;

   };
}
