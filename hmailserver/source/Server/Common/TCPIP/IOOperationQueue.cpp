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
   IOOperationQueue::Push(shared_ptr<IOOperation> operation)
   {
      CriticalSectionScope scope(_criticalSection);

      _queueOperations.push_back(operation);
   }

   void
   IOOperationQueue::Pop(IOOperation::OperationType type)
   {
      CriticalSectionScope scope(_criticalSection);

      if (_ongoingOperations.size() == 0)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5131, "IOOperationQueue::Pop()", "Trying to pop operation list.");
         return;
      }

      std::vector<shared_ptr<IOOperation >>::iterator iter = _ongoingOperations.begin();
      std::vector<shared_ptr<IOOperation >>::iterator iterEnd = _ongoingOperations.end();

      for (; iter != iterEnd; iter++)
      {
         shared_ptr<IOOperation> oper = (*iter);

         if (oper->GetType() == type)
         {
            _ongoingOperations.erase(iter);
            return;
         }

      }

      ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5131, "IOOperationQueue::Pop()", "Trying to remove non-existant item from operation list.");
   }

   bool 
   IOOperationQueue::ContainsQueuedSendOperation()
   {
      CriticalSectionScope scope (_criticalSection);

      if (_queueOperations.empty())
         return false;

      boost_foreach(shared_ptr<IOOperation> operation, _queueOperations)
      {
         if (operation->GetType() == IOOperation::BCTSend)
            return true;
      }

      return false;
   }

   shared_ptr<IOOperation>
   IOOperationQueue::Front()
   {
      CriticalSectionScope scope(_criticalSection);

      // Do we have any items to process? If not, not much to do.
      if (_queueOperations.empty())
      {
         shared_ptr<IOOperation> empty;
         return empty;
      }

      shared_ptr<IOOperation> nextOperation = _queueOperations.front();

      if (_ongoingOperations.size() > 0)
      {
         IOOperation::OperationType pendingType = nextOperation->GetType();

         std::vector<shared_ptr<IOOperation >>::iterator iter = _ongoingOperations.begin();
         std::vector<shared_ptr<IOOperation >>::iterator iterEnd = _ongoingOperations.end();

         for (; iter != iterEnd; iter++)
         {
            shared_ptr<IOOperation> ongoingOperation = (*iter);

            IOOperation::OperationType ongoingType = ongoingOperation->GetType();

            if (ongoingType == pendingType)
            {
               /*
                  We already have a pending operation of this type. We don't allow another one.
                  Case in point:
                  1) It does not make sense to have two Receive/Disconnect/Shutdown at the same time.
                  2) Having multiple sends at the same time might cause corruption problems, since the
                     delivery order is not guaranteed.
               */

               shared_ptr<IOOperation> empty;
               return empty;         
            }

            switch (ongoingType)
            {
            case IOOperation::BCTSend:
               {
                  switch (pendingType)
                  {
                     case IOOperation::BCTSend:         // We can only send one item at a time.
                     case IOOperation::BCTReceive:      // We can not start to process new incoming commands before old data has been sent.
                     case IOOperation::BCTDisconnect:   // We can't disconnect - we want timeout commands to be sent to client.
                     case IOOperation::BCTShutdownSend: // We can't disable send-mode while we're sending data. Makes no sense.
                        shared_ptr<IOOperation> empty;
                        return empty;  

                  }
                  break;
               }
            case IOOperation::BCTReceive:
               {
                  switch (pendingType)
                  {
                  case IOOperation::BCTSend:         // We may send data while waiting for data - timeout messages to clients.
                  case IOOperation::BCTDisconnect:   // We may disconnect even though we're waiting for data. If a client times out, this would be nice.
                  case IOOperation::BCTShutdownSend: // It's OK to close the sending even thoug we're receiving data.
                     break;
                  case IOOperation::BCTReceive:      // We can not start new receives while we're already waiting for data. Does not make any sense.
                     shared_ptr<IOOperation> empty;
                     return empty;  
                  }
                  break;
               }
            case IOOperation::BCTDisconnect:
               {
                  // If we're disconnecting we can't start any new operations.
                  shared_ptr<IOOperation> empty;
                  return empty;  
               }
            case IOOperation::BCTShutdownSend:
               {
                  // Shutting down Send isn't an async operation. We can wait for it to complete.
                  shared_ptr<IOOperation> empty;
                  return empty;  
               }
            }         
         }
      }


      _ongoingOperations.push_back(nextOperation);

      _queueOperations.pop_front();

      return nextOperation;
   }

   
}
