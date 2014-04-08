// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "DeliveryTask.h"
#include "SMTPDeliverer.h"



#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DeliveryTask::DeliveryTask(shared_ptr<Message> pMessage) :
      m_pMessage(pMessage)
   {
      
   }

   DeliveryTask::~DeliveryTask(void)
   {
   }

   void 
   DeliveryTask::DoWork()
   {
      // Do our delivery work.

      try
      {
         SMTPDeliverer::DeliverMessage(m_pMessage);
      }
      catch (boost::system::system_error error)
      {
         String sErrorMessage;
         sErrorMessage.Format(_T("An error occurred while running a delivery task. Error number: %d, Description: %s"), error.code().value(), String(error.what()));
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5315, "DeliveryTask::DoWork", sErrorMessage);
      }
      catch (...)
      {
         // Something went wrong.
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4223, "DeliveryTask::DoWork", "SMTPDeliverer::DeliverMessage() failed");

      }
   }

   void 
   DeliveryTask::StopWork()
   {
      // Not much we can do here since SMTPDeliverer is static. 
   }

}