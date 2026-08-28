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

      void SetIsSSL(bool is_ssl);

   private:

      boost::recursive_mutex mutex_;

      // An ssl::stream cannot read and write concurrently, so a SSL connection needs a
      // stricter rule than a plain socket.
      bool is_ssl_;

      std::deque<std::shared_ptr<IOOperation> > queue_operations_;
      
      std::vector<std::shared_ptr<IOOperation > > ongoing_operations_;
   };

}