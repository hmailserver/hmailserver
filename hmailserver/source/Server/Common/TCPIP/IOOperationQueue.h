// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

#include "IOOperation.h"

namespace HM
{

   class IOOperationQueue
   {
   public:
      IOOperationQueue();
      ~IOOperationQueue(void);

      void Push(std::shared_ptr<IOOperation> operation);
      std::shared_ptr<IOOperation> Front();
      void Pop(IOOperation::OperationType type);

      bool ContainsQueuedSendOperation();

   private:

      boost::recursive_mutex mutex_;

      std::deque<std::shared_ptr<IOOperation> > queue_operations_;
      
      std::vector<std::shared_ptr<IOOperation > > ongoing_operations_;
   };

}