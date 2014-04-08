// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandIdle.h"
#include "IMAPConnection.h"
#include "IMAPConfiguration.h"
#include "IMAPStore.h"

#include "../Common/BO/Messages.h"
#include "../Common/BO/IMAPFolder.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   IMAPCommandIdle::IMAPCommandIdle(shared_ptr<IMAPConnection> pConnection) :
      _connection(pConnection)
   {

   }

   IMAPCommandIdle::~IMAPCommandIdle()
   {
      shared_ptr<IMAPConnection> safeConnection = _connection.lock ();
      if (!safeConnection)
         return;

      try
      {
         if (safeConnection->GetIsIdling())
         {     
            Finish(false);
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4301, "IMAPCommandIdle::~IMAPCommandIdle", "An unknown error has occurred.");
         
         throw;
      }
   }

   IMAPResult
   IMAPCommandIdle::ExecuteCommand(shared_ptr<IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Handles an IMAP idle command from the client. Switches to IDLE mode by
   // adding us to the IMAP notification thingy.
   //---------------------------------------------------------------------------()
   {
      if (!Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPIdle())
         return IMAPResult(IMAPResult::ResultNo, "IMAP IDLE is not enabled.");
         
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultBad, "Command requires authentication.");

      try
      {
         pConnection->SetIsIdling(true);
         pConnection->SendAsciiData("+ idling\r\n");

         m_sTag = pArgument->Tag();
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4302, "IMAPCommandIdle::ExecuteCommand", "An unknown error has occurred.");

         throw;
      }

      return IMAPResult();
   }

   void 
   IMAPCommandIdle::Finish(bool sendNotificationToClient)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Switches out of IDLE mode.
   //---------------------------------------------------------------------------()
   {  
      shared_ptr<IMAPConnection> safeConnection = _connection.lock ();
      if (!safeConnection)
         return;

      try
      {
         safeConnection->SetIsIdling(false);

         if (sendNotificationToClient)
         {
            String sResponse = m_sTag + " OK IDLE terminated\r\n";
            safeConnection->SendAsciiData(sResponse);
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4303, "IMAPCommandIdle::Finish", "An unknown error has occurred.");

         throw;
      }
   }



}
