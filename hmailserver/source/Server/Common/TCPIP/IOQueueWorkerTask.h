// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


#include "..\Threading\Task.h"

namespace HM
{
   class Socket;
   class SocketCompletionPort;

   class IOCPQueueWorkerTask : public Task
   {
   public:

      IOCPQueueWorkerTask(boost::asio::io_service &io_service);

      virtual void DoWork();
      void DoWorkInner();

   private:

      boost::asio::io_service &io_service_;
   };
}