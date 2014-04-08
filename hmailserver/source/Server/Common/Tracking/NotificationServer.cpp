// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "NotificationServer.h"
#include "NotificationClient.h"
#include "ChangeNotification.h"
#include "NotificationClientSubscription.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   NotificationServer::NotificationServer() :
      _subscriptionCounter(0)
   {

   }

   void 
   NotificationServer::SendNotification(shared_ptr<ChangeNotification> pChangeNotification)
   {
      shared_ptr<NotificationClient> empty;
      SendNotification(empty, pChangeNotification);
   }

   void 
   NotificationServer::SendNotification(shared_ptr<NotificationClient> source, shared_ptr<ChangeNotification> pChangeNotification)
   {
      set<shared_ptr<NotificationClient> > clientsToNotify = _GetClientsToNotify(source, pChangeNotification);

      boost_foreach(shared_ptr<NotificationClient> client, clientsToNotify)
      {
         client->OnNotification(pChangeNotification);
      }
   }

   set<shared_ptr<NotificationClient> >
   NotificationServer::_GetClientsToNotify(shared_ptr<NotificationClient> source, shared_ptr<ChangeNotification> changeNotification)
   {
      set<shared_ptr<NotificationClient> > clientsToNotify;

      CriticalSectionScope scope(_criticalSection);

      switch (changeNotification->GetType())
      {
      case ChangeNotification::NotificationMessageAdded:
      case ChangeNotification::NotificationMessageDeleted:
      case ChangeNotification::NotificationMessageFlagsChanged:
         {
            // This is a message change notification.
            std::pair<__int64, __int64> folderSpecifier = std::make_pair(changeNotification->GetAccountID(), changeNotification->GetFolderID());

            // Locate subscribed client.
            std::multimap<std::pair<__int64, __int64>, shared_ptr<NotificationClientSubscription> >::iterator iter = _messageChangeSubscribers.find(folderSpecifier);
            if (iter == _messageChangeSubscribers.end())
               return clientsToNotify;

            std::multimap<std::pair<__int64, __int64>, shared_ptr<NotificationClientSubscription> >::iterator iterLast = _messageChangeSubscribers.upper_bound(folderSpecifier);

            while (iter != iterLast)
            {
               // Notify this client.
               shared_ptr<NotificationClientSubscription> subscription = (*iter).second;
               weak_ptr<NotificationClient> client = subscription->GetSubscribedClient();

               shared_ptr<NotificationClient> safeClient = client.lock();

               if (!safeClient)
               {
                  iter = _messageChangeSubscribers.erase(iter);
                  ErrorManager::Instance()->ReportError(ErrorManager::Low, 5341, "MailboxChangeNotifier::ReportChange", "A previous folder subscription unsubscription failed. Cleaning up.");
                  continue;
               }

               if (source == safeClient)
               {
                  // let's not notify ourselves.
                  iter++;
                  continue;
               }

               clientsToNotify.insert(safeClient);

               iter++;
            }
         }
         break;
      }

      return clientsToNotify;

   }

   __int64
   NotificationServer::SubscribeMessageChanges(__int64 accountID, __int64 folderID, shared_ptr<NotificationClient> pChangeClient)
   {
      try
      {
         std::pair<__int64, __int64> folderSpecifier = std::make_pair(accountID, folderID);

         CriticalSectionScope scope(_criticalSection);

         _subscriptionCounter++;
         shared_ptr<NotificationClientSubscription> subscription = 
            shared_ptr<NotificationClientSubscription>(new NotificationClientSubscription(_subscriptionCounter, pChangeClient));

         // Add subscription
         _messageChangeSubscribers.insert(std::make_pair(folderSpecifier, subscription));

         return _subscriptionCounter;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4307, "MailboxChangeNotifier::SubscribeFolderChanges", "An unknown error has occurred.");
         throw;
      }

      return 0;
   }

   void 
   NotificationServer::UnsubscribeMessageChanges(__int64 iAccountID, __int64 iFolderID, __int64 subscriptionKey)
   {
      try
      {
         std::pair<__int64, __int64> folderSpecifier = std::make_pair(iAccountID, iFolderID);

         CriticalSectionScope scope(_criticalSection);

         std::multimap<std::pair<__int64, __int64>, shared_ptr<NotificationClientSubscription> >::iterator iter = _messageChangeSubscribers.find(folderSpecifier);
         if (iter == _messageChangeSubscribers.end())
            return;

         std::multimap<std::pair<__int64, __int64>, shared_ptr<NotificationClientSubscription> >::iterator iterLast = _messageChangeSubscribers.upper_bound(folderSpecifier);

         for(; iter != iterLast; iter++)
         {
            shared_ptr<NotificationClientSubscription> subscription = (*iter).second;

            if (subscription->GetSubscriptionKey() == subscriptionKey)
            {
               // Unsubscribe
               iter =  _messageChangeSubscribers.erase(iter);
               return;
            }
         }

      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4308, "MailboxChangeNotifier::Unsubscribe", "An unknown error has occurred.");
         throw;
      }
   }     

   __int64
   NotificationServer::SubscribeFolderListChanges(__int64 accountID, shared_ptr<NotificationClient> pChangeClient)
   {
      try
      {
         CriticalSectionScope scope(_criticalSection);

         _subscriptionCounter++;
         shared_ptr<NotificationClientSubscription> subscription = 
            shared_ptr<NotificationClientSubscription>(new NotificationClientSubscription(_subscriptionCounter, pChangeClient));

         // Add subscription
         _folderListChangeSubscribers.insert(std::make_pair(accountID, subscription));

         return _subscriptionCounter;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4307, "MailboxChangeNotifier::SubscribeFolderChanges", "An unknown error has occurred.");
         throw;
      }

      return 0;
   }

   void 
   NotificationServer::UnsubscribeFolderListChanges(__int64 accountID, __int64 subscriptionKey)
   {
      try
      {
         CriticalSectionScope scope(_criticalSection);

         std::multimap<__int64, shared_ptr<NotificationClientSubscription> >::iterator iter = _folderListChangeSubscribers.find(accountID);
         if (iter == _folderListChangeSubscribers.end())
            return;

         std::multimap<__int64, shared_ptr<NotificationClientSubscription> >::iterator iterLast = _folderListChangeSubscribers.upper_bound(accountID);

         for(; iter != iterLast; iter++)
         {
            shared_ptr<NotificationClientSubscription> subscription = (*iter).second;

            if (subscription->GetSubscriptionKey() == subscriptionKey)
            {
               // Unsubscribe
               iter =  _folderListChangeSubscribers.erase(iter);
               return;
            }
         }

      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4308, "MailboxChangeNotifier::Unsubscribe", "An unknown error has occurred.");
         throw;
      }
   }     
}
