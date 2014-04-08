// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "..\Threading\Task.h"

namespace HM
{

   class MessageIndexer : public Task
   {
   public:
      MessageIndexer();
      ~MessageIndexer(void);
      
      static void Start();
      static void Stop();

      virtual void DoWork();
      virtual void StopWork();

      static void IndexNow();

   private:
   
      static shared_ptr<MessageIndexer> _GetRunningIndexer();

      void _IndexMessages();


      Event _stopRunning;
      Event _indexNow;

      static CriticalSection _starterLock; 
	  int iIndexRunCount;

   };
}