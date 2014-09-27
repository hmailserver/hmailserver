// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include <boost/thread.hpp>

namespace HM
{

   class MessageIndexer : public Singleton<MessageIndexer>
   {
   public:
      MessageIndexer();
      ~MessageIndexer(void);
      
      void Start();
      void Stop();

      void IndexNow();

   private:
   

      void WorkerFunc();
      void WorkerFuncInternal();

      void IndexMessages_();

      boost::thread workerThread_;
	   int iIndexRunCount;

      boost::recursive_mutex starterMutex_;

      Event index_now_;
   };
}