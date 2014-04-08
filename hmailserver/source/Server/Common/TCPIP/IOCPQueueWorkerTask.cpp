// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include <winerror.h>

#include "IOCPQueueWorkerTask.h"

#include "../Application/SessionManager.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IOCPQueueWorkerTask::IOCPQueueWorkerTask(boost::asio::io_service &io_service) :
      _io_service(io_service)
   {

   }

   void 
   IOCPQueueWorkerTask::DoWork()
   {
      bool continueProcess = true;
      while (continueProcess)
      {
         try
         {
            _io_service.run();

            continueProcess = false;
         }
         catch (boost::system::system_error error)
         {
            String sErrorMessage;
            sErrorMessage.Format(_T("An error occured while handling asynchronous requests. Error number: %d, Description: %s"), error.code().value(), String(error.what()));
            
            ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "IOCPQueueWorkerTask::DoWork", sErrorMessage);
         }
         catch (std::exception const& e)
         {
            String sErrorMessage = Formatter::Format("An unknown error occured while handling asynchronous requests. Error number: {0}", e.what());
            ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "IOCPQueueWorkerTask::DoWork", sErrorMessage);
         }
         catch (...)
         {
            ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "IOCPQueueWorkerTask::DoWork", "An unknown error occured while handling asynchronous requests.");
         }
      }
   }

   void 
   IOCPQueueWorkerTask::StopWork()
   //--------------------------------------------------------------------- ------()
   // DESCRIPTION:
   // Post a message that we should stop.
   //---------------------------------------------------------------------------()
   {
      _io_service.stop();
   }


}