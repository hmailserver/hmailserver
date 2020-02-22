// Copyright (c) 2008 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2008-08-12

#include "StdAfx.h"
#include "IOOperationQueue.h"
#include "IOOperation.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IOOperationQueue::IOOperationQueue()
   {
      
   }

   IOOperationQueue::~IOOperationQueue(void)
   {

   }

   void
   IOOperationQueue::Push(std::shared_ptr<IOOperation> operation)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      queue_operations_.push_back(operation);
   }

   void
   IOOperationQueue::Pop(IOOperation::OperationType type)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      if (ongoing_operations_.size() == 0)
      {
         String message = Formatter::Format("Trying to pop empty operation list. Type: {0}", type);
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5131, "IOOperationQueue::Pop()", message);
         return;
      }

      auto iter = ongoing_operations_.begin();
      auto iterEnd = ongoing_operations_.end();

      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<IOOperation> oper = (*iter);

         if (oper->GetType() == type)
         {
            ongoing_operations_.erase(iter);
            return;
         }

      }

      ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5131, "IOOperationQueue::Pop()", "Trying to remove non-existant item from operation list.");
   }

   bool 
   IOOperationQueue::ContainsQueuedSendOperation()
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      if (queue_operations_.empty())
         return false;

      for(std::shared_ptr<IOOperation> operation : queue_operations_)
      {
         if (operation->GetType() == IOOperation::BCTWrite)
            return true;
      }

      return false;
   }

   std::shared_ptr<IOOperation>
   IOOperationQueue::Front()
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      // Do we have any items to process? If not, not much to do.
      if (queue_operations_.empty())
      {
         std::shared_ptr<IOOperation> empty;
         return empty;
      }

      std::shared_ptr<IOOperation> nextOperation = queue_operations_.front();

      if (ongoing_operations_.size() > 0)
      {
         IOOperation::OperationType pendingType = nextOperation->GetType();

         auto iter = ongoing_operations_.begin();
         auto iterEnd = ongoing_operations_.end();

         bool operation_can_be_processed = true;

         for (; iter != iterEnd; iter++)
         {
            std::shared_ptr<IOOperation> ongoingOperation = (*iter);

            IOOperation::OperationType ongoingType = ongoingOperation->GetType();

            switch (ongoingType)
            {
            case IOOperation::BCTWrite:
               {
                  switch (pendingType)
                  {
                     case IOOperation::BCTWrite:         // We can only send one item at a time.
                     case IOOperation::BCTRead:      // We can not start to process new incoming commands before old data has been sent.
                     case IOOperation::BCTDisconnect:   // We can't disconnect - we want timeout messages to be sent to client.
                     case IOOperation::BCTShutdownSend: // We can't disable send-mode while we're sending data. Makes no sense.
                     case IOOperation::BCTHandshake:    // We can't perform a SSL handshake while we're sending data.
                        std::shared_ptr<IOOperation> empty;
                        return empty;  

                  }
                  break;
               }
            case IOOperation::BCTRead:
               {
                  switch (pendingType)
                  {
                  case IOOperation::BCTWrite:         // We may send data while we're processing data (normal responses)
                  case IOOperation::BCTDisconnect:   // We may disconnect while we're processing data
                  case IOOperation::BCTShutdownSend: // It's OK to close the sending even though we're receiving data.
                     break;
                  case IOOperation::BCTRead:      // We can not start new receives while we're processing data. Concurrent receives are not supported.
                  case IOOperation::BCTHandshake:    // We can't perform a SSL handshake while we're processing data at the same time7.
                     std::shared_ptr<IOOperation> empty;
                     return empty;  
                  }
                  break;
               }
            case IOOperation::BCTDisconnect:   // If we're disconnecting we can't start any new operations.
            case IOOperation::BCTShutdownSend: // Shutting down Send and performing other operations at the same time is not supported.
            case IOOperation::BCTHandshake:    // Doing a handshake and sending/receiving other data at the same time is not supported
               {
                  std::shared_ptr<IOOperation> empty;
                  return empty;  
                  break;
               }
            }         
         }
      }


      ongoing_operations_.push_back(nextOperation);

      queue_operations_.pop_front();

      return nextOperation;
   }

   
}
