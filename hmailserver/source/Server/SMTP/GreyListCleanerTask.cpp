// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "GreyListCleanerTask.h"

#include "../Common/AntiSpam/AntiSpamConfiguration.h"
#include "../Common/Persistence/PersistentGreyList.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   GreyListCleanerTask::GreyListCleanerTask(void)
   {
   }

   GreyListCleanerTask::~GreyListCleanerTask(void)
   {
   }
   
   void
   GreyListCleanerTask::DoWork()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Remove old records from the greylist
   //---------------------------------------------------------------------------()
   {
      // Are we using grey-listing?
      AntiSpamConfiguration &antiSpamConfig = Configuration::Instance()->GetAntiSpamConfiguration();

      if (!antiSpamConfig.GetUseGreyListing())
      {
         // Grey listing is not used. Don't delete old records.
         return;
      }

      LOG_DEBUG("GreyListCleanerTask::DoWork()");
      
      // Check if we should temporarily disable grey listing while we
      // run this task. We might need to do this if the task takes a
      // long time and causes the entire greylisting table to become locked.
      bool bGreylistingEnabledDuringExpiration = IniFileSettings::Instance()->GetGreylistingEnabledDuringRecordExpiration();

      if (!bGreylistingEnabledDuringExpiration)
         antiSpamConfig.SetGreylistingTemporarilyDisabled(true);

      PersistentGreyList::ClearExpiredRecords();      
      
      if (!bGreylistingEnabledDuringExpiration)
         antiSpamConfig.SetGreylistingTemporarilyDisabled(false);
   }

}