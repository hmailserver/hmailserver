// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "ServerMessages.h"

#include "../Persistence/PersistentServerMessage.h"
#include "ServerMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ServerMessages::ServerMessages()
   {
   }

   ServerMessages::~ServerMessages(void)
   {
   }


   void 
   ServerMessages::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads all SURBL servers from the database.
   //---------------------------------------------------------------------------()
   {
      String sSQL = "select * from hm_servermessages order by smname asc";
      DBLoad_(sSQL);
   }

   String 
   ServerMessages::GetMessage(const String &sName) const
   {
      std::shared_ptr<ServerMessage> pSM = GetItemByName(sName);

      if (!pSM)
      {
         // Not good, but should never happen.
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4329, "ServerMessages::GetMessage", "Server message '" + sName + "' could not be found.");

         return "Server error - Please contact your system administrator.";
      }

      return pSM->GetText();

   }
}