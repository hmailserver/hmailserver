// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include <winerror.h>

#include "IOQueueWorkerTask.h"

#include "../Application/SessionManager.h"
#include "../Application/ExceptionHandler.h"
#include "../TCPIP/DisconnectedException.h"

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
      while (true)
      {
         try
         {
            io_service_.run();
            return;
         }
         catch (boost::system::system_error& error)
         {
            if (error.code().value() == ERROR_ABANDONED_WAIT_0)
            {
               // If a call to GetQueuedCompletionStatus fails because the completion port handle associated with it is
               // closed while the call is outstanding, the function returns FALSE, *lpOverlapped will be NULL, 
               //and GetLastError will return ERROR_ABANDONED_WAIT_0.

               return;
            }

            throw;
         }
         catch (DisconnectedException&)
         {
            LOG_DEBUG("Connection was terminated - Client is disconnected.");
            // A client has disconnected. This is not an error scenario, so we can
            // keep running.
         }

      }
   }

}