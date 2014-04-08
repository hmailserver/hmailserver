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
      _type(type)
   {

   }

   ChangeNotification::ChangeNotification(__int64 accountID, __int64 folderID, NotificationType type) :
      _accountID(accountID),
      _folderID(folderID),
      _type(type)
   {

   }

   ChangeNotification::ChangeNotification(__int64 accountID, __int64 folderID, NotificationType type, const std::vector<__int64>& affectedMessages) :
      _accountID(accountID),
      _folderID(folderID),
      _type(type),
      _affectedMessages(affectedMessages)
   {

   }

   ChangeNotification::ChangeNotification(__int64 accountID, __int64 folderID, NotificationType type, __int64 data) :
      _accountID(accountID),
      _folderID(folderID),
      _type(type),
      _data(data)
   {

   }

   ChangeNotification::~ChangeNotification()
   {

   }

   __int64
   ChangeNotification::GetFolderID() const
   {
      return _folderID;
   }

   __int64
   ChangeNotification::GetAccountID() const
   {
      return _accountID;
   }

   __int64
   ChangeNotification::GetData() const
   {
      return _data;
   }

   ChangeNotification::NotificationType
   ChangeNotification::GetType() const
   {
      return _type;
   }

   const std::vector<__int64>&
   ChangeNotification::GetAffectedMessages() const
   {
      return _affectedMessages;
   }

}
