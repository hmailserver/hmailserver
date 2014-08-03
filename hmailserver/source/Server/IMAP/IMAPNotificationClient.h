// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Common/Tracking/NotificationClient.h"

namespace HM
{
   class IMAPConnection;
   class ChangeNotification;

   class IMAPNotificationClient : public NotificationClient,
                                  public boost::enable_shared_from_this<IMAPNotificationClient>
   {
   public:

      IMAPNotificationClient();
      virtual ~IMAPNotificationClient();

      void SetConnection(weak_ptr<IMAPConnection> connection);
      virtual void OnNotification(shared_ptr<ChangeNotification> notification);

      void SendCachedNotifications();

      static String GenerateRecentString(int recentMessages);
      static String GenerateExistsString(int recentMessages);

      void SubscribeMessageChanges(__int64 accountID, __int64 folderID);

      void UnsubscribeMessageChanges();

   private:

      void CacheChangeNotification_(shared_ptr<ChangeNotification> pChangeNotification);
      void SendChangeNotification_(shared_ptr<ChangeNotification> pChangeNotification);

      void SendEXISTS_(int iExists);
      void SendRECENT_(int recent);
      void SendEXPUNGE_(const std::vector<__int64> & vecMessages);
      void SendFLAGS_(const std::set<__int64> & vecMessages);

      boost::recursive_mutex mutex_;
      vector<shared_ptr<ChangeNotification> > cached_changes_;
      
      weak_ptr<IMAPConnection> parent_connection_;

      __int64 account_id_;
      __int64 folder_id_;
      __int64 message_change_subscription_id_;
      __int64 folder_list_change_subscription_id_;

   };


}