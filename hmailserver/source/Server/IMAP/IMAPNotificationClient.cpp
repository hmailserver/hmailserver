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


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPNotificationClient::IMAPNotificationClient() :
      _messageChangeSubscriptionID(0),
      _folderListChangeSubscriptionID(0),
      _accountID(0),
      _folderID(0)
   {

   }

   IMAPNotificationClient::~IMAPNotificationClient()
   {
      if (_folderListChangeSubscriptionID > 0)
      {
         shared_ptr<NotificationServer> notificationServer = Application::Instance()->GetNotificationServer();
         notificationServer->UnsubscribeFolderListChanges(_accountID, _folderListChangeSubscriptionID);
      }
   }

   void 
   IMAPNotificationClient::SubscribeMessageChanges(__int64 accountID, __int64 folderID)
   {
      assert(accountID >= 0);
      assert(folderID > 0);

      _accountID = accountID;
      _folderID = folderID;

      shared_ptr<NotificationServer> notificationServer = Application::Instance()->GetNotificationServer();
      _messageChangeSubscriptionID = notificationServer->SubscribeMessageChanges(_accountID, _folderID, shared_from_this());
   }

   void
   IMAPNotificationClient::UnsubscribeMessageChanges()
   {
      assert(_accountID >= 0);
      assert(_folderID > 0);
      assert(_messageChangeSubscriptionID > 0);

      // Since we don't want to look at he folder any more,
      // we're not interested in any updates.
      shared_ptr<NotificationServer> notificationServer = Application::Instance()->GetNotificationServer();
      notificationServer->UnsubscribeMessageChanges(_accountID, _folderID, _messageChangeSubscriptionID);

      // If there are cached updates for this folder but the client
      // don't want to look at the folder any more, the cached updates
      // will be gone.
      CriticalSectionScope scope(_critSec);
      _cachedChanges.clear();
   }

   void 
   IMAPNotificationClient::SetConnection(weak_ptr<IMAPConnection> connection)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Called by the mailbox change notifier when something has happened to the mailbox.
   //---------------------------------------------------------------------------()
   {
      _parentConnection = connection;
   }

   void 
   IMAPNotificationClient::OnNotification(shared_ptr<ChangeNotification> notification)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Called by the mailbox change notifier when something has happened to the mailbox.
   //---------------------------------------------------------------------------()
   {
      shared_ptr<IMAPConnection> parentConnection = _parentConnection.lock();

      if (!parentConnection)
         return;

      if (parentConnection->GetIsIdling())
         _SendChangeNotification(notification);
      else
         _CacheChangeNotification(notification);
   }

   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Cache this change. We'll send a notification later on.
   //---------------------------------------------------------------------------()
   void 
   IMAPNotificationClient::_CacheChangeNotification(shared_ptr<ChangeNotification> pChangeNotification)
   {
      CriticalSectionScope scope(_critSec);
      _cachedChanges.push_back(pChangeNotification);
   }

   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Send a summary of all changes to the client...
   //---------------------------------------------------------------------------()
   void 
   IMAPNotificationClient::SendCachedNotifications()
   {
      shared_ptr<IMAPConnection> connection = _parentConnection.lock();

      if (!connection)
         return;

      CriticalSectionScope scope(_critSec);

      int lastExists = -1;
      int lastRecent = -1;

      std::set<__int64> flagMessages;

      boost_foreach(shared_ptr<ChangeNotification> changeNotification, _cachedChanges)
      {
         switch (changeNotification->GetType())
         {
         case ChangeNotification::NotificationMessageAdded:
            {
               shared_ptr<Messages> pMessages = connection->GetCurrentFolder()->GetMessages();
               pMessages->Refresh();
               lastExists = pMessages->GetCount();
               lastRecent = pMessages->GetNoOfRecent();
               break;
            }
         case ChangeNotification::NotificationMessageDeleted:
            {
               // Send EXPUNGE
               _SendEXPUNGE(changeNotification->GetAffectedMessages());

               // Send EXISTS
               shared_ptr<Messages> pMessages = connection->GetCurrentFolder()->GetMessages();
               lastExists = pMessages->GetCount();
               lastRecent = pMessages->GetNoOfRecent();

               break;
            }
         case ChangeNotification::NotificationMessageFlagsChanged:
            {
               // Send flag notification
               boost_foreach(__int64 messageID, changeNotification->GetAffectedMessages())
               {
                  if (flagMessages.find(messageID) == flagMessages.end())
                     flagMessages.insert(messageID);
               }

               break;
            }
         }
      }

      if (flagMessages.size() > 0)
         _SendFLAGS(flagMessages);
      
      if (lastExists >= 0)
         _SendEXISTS(lastExists);

      if (lastRecent >= 0)
         _SendRECENT(lastExists);

      _cachedChanges.clear();
   }

   void 
   IMAPNotificationClient::_SendChangeNotification(shared_ptr<ChangeNotification> pChangeNotification)
   {
      shared_ptr<IMAPConnection> connection = _parentConnection.lock();
      if (!connection)
         return;

      try
      {
         switch (pChangeNotification->GetType())
         {
         case ChangeNotification::NotificationMessageAdded:
            {
               shared_ptr<Messages> pMessages = connection->GetCurrentFolder()->GetMessages();
               _SendEXISTS(pMessages->GetCount());
               _SendRECENT(pMessages->GetNoOfRecent());
               break;
            }
         case ChangeNotification::NotificationMessageDeleted:
            {
               // Send EXPUNGE
               _SendEXPUNGE(pChangeNotification->GetAffectedMessages());

               // Send EXISTS
               shared_ptr<Messages> pMessages = connection->GetCurrentFolder()->GetMessages();
               _SendEXISTS(pMessages->GetCount());
               _SendRECENT(pMessages->GetNoOfRecent());

               break;
            }
         case ChangeNotification::NotificationMessageFlagsChanged:
            {
               // Send flag notification
               std::set<__int64> affectedMessages;
               boost_foreach(__int64 messageID, pChangeNotification->GetAffectedMessages())
               {
                  affectedMessages.insert(messageID);
               }
              
               _SendFLAGS(affectedMessages);

               break;
            }
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4345, "IMAPMailboxChangeNotifier::OnMailboxChange", "An unknown error has occurred.");
         throw;
      }
   }

   void 
   IMAPNotificationClient::_SendEXPUNGE(const std::vector<__int64> & vecMessages)
   {
      shared_ptr<IMAPConnection> connection = _parentConnection.lock();
      if (!connection)
         return;

      String sResponse;
      boost_foreach(__int64 messageIndex, vecMessages)
         sResponse.AppendFormat(_T("* %I64d EXPUNGE\r\n"), messageIndex);

      connection->SendAsciiData(sResponse);

   }

   void 
   IMAPNotificationClient::_SendFLAGS(const std::set<__int64> & vecMessages)
   {
      shared_ptr<IMAPConnection> connection = _parentConnection.lock();
      if (!connection)
         return;

      boost_foreach(__int64 messageID, vecMessages)
      {
         String sResponse;

         int foundIndex = 0;
         shared_ptr<Message> pMessage = connection->GetCurrentFolder()->GetMessages()->GetItemByDBID(messageID, foundIndex);

         if (!pMessage)
            return;

         connection->SendAsciiData(IMAPStore::GetMessageFlags(pMessage, foundIndex));
      }


   }


   void 
   IMAPNotificationClient::_SendEXISTS(int iExists)
   {
      shared_ptr<IMAPConnection> connection = _parentConnection.lock();
      if (!connection)
         return;

      String sResponse = GenerateExistsString(iExists);
      connection->SendAsciiData(sResponse);
   }

   void 
   IMAPNotificationClient::_SendRECENT(int recent)
   {
      shared_ptr<IMAPConnection> connection = _parentConnection.lock();
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
