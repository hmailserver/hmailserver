// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "RemoveExpiredRecords.h"

#include "../Persistence/PersistentSecurityRange.h"
#include "../Persistence/PersistentLogonFailure.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   RemoveExpiredRecords::RemoveExpiredRecords(void)
   {
   }

   RemoveExpiredRecords::~RemoveExpiredRecords(void)
   {
   }
   
   void
   RemoveExpiredRecords::DoWork()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Remove old records from the greylist
   //---------------------------------------------------------------------------()
   {
      PersistentSecurityRange::DeleteExpired();
      
      if (Configuration::Instance()->GetAutoBanLogonEnabled())
      {
         int logonFailureMinutes = Configuration::Instance()->GetMaxLogonAttemptsWithin();
         
         PersistentLogonFailure persistentLogonFailure;
         persistentLogonFailure.ClearOldFailures(logonFailureMinutes);
      }
   }

}