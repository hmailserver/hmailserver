// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "ChangeNotification.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   ChangeNotification::ChangeNotification(NotificationType type) :
      type_(type)
   {

   }

   ChangeNotification::ChangeNotification(__int64 accountID, __int64 folderID, NotificationType type) :
      account_id_(accountID),
      folder_id_(folderID),
      type_(type)
   {

   }

   ChangeNotification::ChangeNotification(__int64 accountID, __int64 folderID, NotificationType type, const std::vector<__int64>& affectedMessages) :
      account_id_(accountID),
      folder_id_(folderID),
      type_(type),
      affected_messages_(affectedMessages)
   {

   }

   ChangeNotification::ChangeNotification(__int64 accountID, __int64 folderID, NotificationType type, __int64 data) :
      account_id_(accountID),
      folder_id_(folderID),
      type_(type),
      data_(data)
   {

   }

   ChangeNotification::~ChangeNotification()
   {

   }

   __int64
   ChangeNotification::GetFolderID() const
   {
      return folder_id_;
   }

   __int64
   ChangeNotification::GetAccountID() const
   {
      return account_id_;
   }

   __int64
   ChangeNotification::GetData() const
   {
      return data_;
   }

   ChangeNotification::NotificationType
   ChangeNotification::GetType() const
   {
      return type_;
   }

   const std::vector<__int64>&
   ChangeNotification::GetAffectedMessages() const
   {
      return affected_messages_;
   }

}
