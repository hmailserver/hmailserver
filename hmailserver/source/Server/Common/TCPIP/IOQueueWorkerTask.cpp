// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include <winerror.h>

#include "IOQueueWorkerTask.h"

#include "../Application/SessionManager.h"
#include "../Application/ExceptionHandler.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IOCPQueueWorkerTask::IOCPQueueWorkerTask(boost::asio::io_service &io_service) :
      Task("IOCPQueueWorkerTask"),
      io_service_(io_service)
   {

   }

   void 
   IOCPQueueWorkerTask::DoWork()
   {
      while (true)
      {
         boost::function<void()> func = boost::bind( &IOCPQueueWorkerTask::DoWorkInner, this );
         if (ExceptionHandler::Run("IOCPQueueWorkerTask", func))
            return;
      
      }
   }


   void 
   IOCPQueueWorkerTask::DoWorkInner()
   {
      try
      {
         io_service_.run();
      }
      catch (thread_interrupted const&)
      {
         return;
      }
   }

}