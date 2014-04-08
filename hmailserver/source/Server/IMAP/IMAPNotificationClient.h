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

      void _CacheChangeNotification(shared_ptr<ChangeNotification> pChangeNotification);
      void _SendChangeNotification(shared_ptr<ChangeNotification> pChangeNotification);

      void _SendEXISTS(int iExists);
      void _SendRECENT(int recent);
      void _SendEXPUNGE(const std::vector<__int64> & vecMessages);
      void _SendFLAGS(const std::set<__int64> & vecMessages);
      void _SendMessage(int iExists);

      CriticalSection _critSec;
      vector<shared_ptr<ChangeNotification> > _cachedChanges;
      
      weak_ptr<IMAPConnection> _parentConnection;

      __int64 _accountID;
      __int64 _folderID;
      __int64 _messageChangeSubscriptionID;
      __int64 _folderListChangeSubscriptionID;

   };


}