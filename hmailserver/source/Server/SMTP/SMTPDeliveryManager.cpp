// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "SMTPDeliveryManager.h"
#include "SMTPConfiguration.h"
#include "StatisticsSender.h"

#include "DeliveryTask.h"

#include "../common/Application/IniFileSettings.h"
#include "../common/Application/Property.h"
#include "../common/Util/ServerStatus.h"
#include "../common/Persistence/PersistentMessage.h"

#include "../common/BO/Message.h"


#include "../common/Cache/MessageCache.h"

#include "../Common/Threading/WorkQueueManager.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SMTPDeliveryManager::SMTPDeliveryManager() :
      Task("SMTPDeliveryManager"),
      queue_name_("SMTP delivery queue"),
      uncache_pending_messages_(false)
   {
      cur_number_of_sent_ = 0;

      int iMaxNumberOfThreads = Configuration::Instance()->GetSMTPConfiguration()->GetMaxNoOfDeliveryThreads();
      queue_id_ = WorkQueueManager::Instance()->CreateWorkQueue(iMaxNumberOfThreads, queue_name_);

   }  

   SMTPDeliveryManager::~SMTPDeliveryManager()
   {
      try
      {
         WorkQueueManager::Instance()->RemoveQueue(queue_name_);
      }
      catch (...)
      {

      }
   }

   void 
   SMTPDeliveryManager::SetDeliverMessage()
   {
      deliver_messages_.Set();
   }

   const String &
   SMTPDeliveryManager::GetQueueName() const
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Return the name of the delivery queue.
   //---------------------------------------------------------------------------
   {
      return queue_name_;
   }

   void
   SMTPDeliveryManager::DoWork()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Responsible for creating threads to deliver messages.
   //---------------------------------------------------------------------------()
   {
      LOG_DEBUG("SMTPDeliveryManager::Start()");

      SetIsStarted();

      // Unlock all messages
      PersistentMessage::UnlockAll();

      std::shared_ptr<WorkQueue> pQueue = WorkQueueManager::Instance()->GetQueue(GetQueueName());

      boost::mutex deliver_mutex;

      while (1)
      {
         // Deliver all pending messages
         std::shared_ptr<Message> pMessage;
         while (pMessage = GetNextMessage_())
         {
            // Lock this message
            if (!PersistentMessage::LockObject(pMessage))
            {
               // Failed to lock the message.
               ErrorManager::Instance()->ReportError(ErrorManager::Critical, 4216, "SMTPDeliveryManager::DoWork", "Failed to lock message.");
               continue;
            }

            std::shared_ptr<DeliveryTask> pDeliveryTask = std::shared_ptr<DeliveryTask>(new DeliveryTask(pMessage));
            WorkQueueManager::Instance()->AddTask(queue_id_, pDeliveryTask);

            cur_number_of_sent_++;

            SendStatistics_();

            ServerStatus::Instance()->OnMessageProcessed();
         }

         deliver_messages_.WaitFor(boost::chrono::minutes(1));
      }
      
      return;
   }

   void
   SMTPDeliveryManager::SendStatistics_(bool bIgnoreMessageCount)
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Sends statistics to hMailServer.com
   //---------------------------------------------------------------------------
   {
      if (cur_number_of_sent_ > 1000 || (bIgnoreMessageCount && cur_number_of_sent_ > 5))
      {
         if (Configuration::Instance()->GetSendStatistics())
         {
            // Send statistics to server.
            StatisticsSender Sender;
            
            Sender.SendStatistics(cur_number_of_sent_);

            // Always reset the number. If the statistics sender can't connect
            // to hMailServer for some reason, we should not try immediately again
            // and instead just give up.
            cur_number_of_sent_ = 0;
         }
         else
            cur_number_of_sent_ = 0;
      }

   }

   void
   SMTPDeliveryManager::LoadPendingMessageList_()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Loads a list of messages that should be delivered from the database
   //---------------------------------------------------------------------------
   {
      HM::DatabaseSettings::SQLDBType DBType = IniFileSettings::Instance()->GetDatabaseType();

      // Tweak to prioritize small & newer emails in queue delivery order
      // Assumption is that large emails will take longer to deliver tying up queue
      // vs just getting rid of smaller emails 1st and messages with high # of
      // tries are not likely to go anyway so why tie up newer emails in queue
      String sql = Formatter::Format("select * from hm_messages where messagetype = 1 and messagelocked = 0 and messagenexttrytime <= {0} order by messagesize, messagecurnooftries, messageid asc", SQLStatement::GetCurrentTimestamp());
      // String sql = Formatter::Format("select * from hm_messages where messagetype = 1 and messagelocked = 0 and messagenexttrytime <= {0} order by messageid asc", SQLStatement::GetCurrentTimestamp());
      SQLCommand command(sql);

      pending_messages_ = Application::Instance()->GetDBManager()->OpenRecordset(command);
   }

   std::shared_ptr<Message>
   SMTPDeliveryManager::GetNextMessage_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Retrives the first unlcoked message from the database and tries to lock it.
   // 
   // 0 is returned if no messages exists.
   //---------------------------------------------------------------------------()
   {
      if (!pending_messages_ || pending_messages_->IsEOF() || uncache_pending_messages_)
      {
         uncache_pending_messages_ = false;

         LoadPendingMessageList_();
      }

      std::shared_ptr<Message> pRetMessage;
      if (!pending_messages_ || pending_messages_->IsEOF())
         return pRetMessage;
      
      // Fetch the ID of the first message
      __int64 iMessageID = pending_messages_->GetInt64Value("messageid");

      // Try to read this message from the message cache. Might fail.
      pRetMessage = MessageCache::Instance()->GetMessage(iMessageID);
         
      if (!pRetMessage) 
      {
         // Message was not found in cache. Read from database. Will
         // require 1 extra statement towards the database, since we
         // need to read recipients 
         pRetMessage = std::shared_ptr<Message> (new Message(false));
         PersistentMessage::ReadObject(pending_messages_, pRetMessage);
      }

      // Move to the next message in the cache.
      pending_messages_->MoveNext();

      return pRetMessage;
   }

   void 
   SMTPDeliveryManager::UncachePendingMessages()
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Tells the delivery manager to uncache it's list of messages. This is 
   // normally only required if the delivery queue should be clear
   //---------------------------------------------------------------------------
   {
      uncache_pending_messages_ = true;
   }

   void
   SMTPDeliveryManager::OnPropertyChanged(std::shared_ptr<Property> pProperty)
   {
      String sPropertyName = pProperty->GetName();
      
      if (sPropertyName == PROPERTY_MAXDELIVERYTHREADS)
      {
         std::shared_ptr<WorkQueue> pWorkQueue = WorkQueueManager::Instance()->GetQueue(GetQueueName());
         
         if (!pWorkQueue)
            return;

         pWorkQueue->SetMaxSimultaneous(pProperty->GetLongValue());
      }
   }

}
