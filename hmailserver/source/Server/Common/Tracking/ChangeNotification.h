// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ChangeNotification
   {
   public:

      enum NotificationType
      {
         NotificationMessageAdded = 1,
         NotificationMessageDeleted = 2,
         NotificationMessageFlagsChanged = 3,
      };


      ChangeNotification(NotificationType type);
      ChangeNotification(__int64 accountID, __int64 folderID, NotificationType type);
      ChangeNotification(__int64 accountID, __int64 folderID, NotificationType type, const std::vector<__int64>& affectedMessages);
      ChangeNotification(__int64 accountID, __int64 folderID, NotificationType type, __int64 data);

      virtual ~ChangeNotification();

      const std::vector<__int64>& GetAffectedMessages() const;
      __int64 GetFolderID() const;
      __int64 GetAccountID() const;
      NotificationType GetType() const;
      __int64 GetData() const;

   private:

      __int64 account_id_;
      __int64 folder_id_;
      __int64 data_;
      std::vector<__int64> affected_messages_;
      NotificationType type_;

   };
}
