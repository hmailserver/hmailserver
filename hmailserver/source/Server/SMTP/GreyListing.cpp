// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "GreyListing.h"
#include "../Common/AntiSpam/AntiSpamConfiguration.h"

#include "../Common/BO/GreyListingWhiteAddresses.h"
#include "../Common/Persistence/PersistentGreyList.h"
#include "../Common/BO/GreyListTriplet.h"

#include "../Common/Util/VariantDateTime.h"
#include "../Common/Util/Time.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   GreyListing::GreyListing(void)
   {
   }

   GreyListing::~GreyListing(void)
   {
   }
   
   bool 
   GreyListing::GetAllowSend(const String &sSenderAddress, const String &sRecipientAddress, const IPAddress &remoteIP)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Implements grey listing spam protection.
   //---------------------------------------------------------------------------()
   {
      if (PersistentGreyListingWhiteAddress::IsSenderWhitelisted(remoteIP.ToString()))
      {
         // The senders IP address is white listed.
         return true;
      }

      // Try to read triplet.
      std::shared_ptr<GreyListTriplet> pTriplet = PersistentGreyList::GetRecord(sSenderAddress, sRecipientAddress, remoteIP);

      AntiSpamConfiguration &antiSpamConfig = Configuration::Instance()->GetAntiSpamConfiguration();

      DateTime dtNow = DateTime::GetCurrentTime();

      if (pTriplet)
      {
         // Check if this triplet is valid yet.
         DateTime dtTripletBlockEnd = Time::GetDateFromSystemDate(pTriplet->GetBlockEndTime());

         if (dtNow > dtTripletBlockEnd)
         {
            // Delivery is OK. We should reset the deletion time of the triple.
            
            const int iDeleteHours = antiSpamConfig.GetGreyListingFinalDelete();

            DateTimeSpan dtDeleteAdd;
            dtDeleteAdd.SetDateTimeSpan(0, iDeleteHours, 0,0);

            pTriplet->SetDeleteTime(Time::GetTimeStampFromDateTime(dtNow + dtDeleteAdd));
 
            // Save the updated triplet.
            if (!PersistentGreyList::ResetDeletionTime(pTriplet))
            {
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4327, "GreyListing::GetAllowSend", "Failed to save grey listing triplet in the database.");
            }

            return true;
         }
         else
         {
            // No, the block time hasn't passed yet.
            PersistentGreyList::IncreaseBlocked(pTriplet->GetID());

            return false;
         }
      }

      // Create new triplet record.
      pTriplet = std::shared_ptr<GreyListTriplet>(new GreyListTriplet());

      // Set timestamps.
      const int iUnblockMinutes = antiSpamConfig.GetGreyListingInitialDelay();
      const int iDeleteHours = antiSpamConfig.GetGreyListingInitialDelete();

      DateTimeSpan tsUnblock;
      DateTimeSpan tsDelete;
      tsUnblock.SetDateTimeSpan(0,0,iUnblockMinutes,0);
      tsDelete.SetDateTimeSpan(0,iDeleteHours,0,0);

      pTriplet->SetCreateTime(Time::GetCurrentDateTime());
      pTriplet->SetBlockEndTime(Time::GetTimeStampFromDateTime(dtNow + tsUnblock));
      pTriplet->SetDeleteTime(Time::GetTimeStampFromDateTime(dtNow + tsDelete));

      pTriplet->SetIPAddress(remoteIP);
      pTriplet->SetSenderAddress(sSenderAddress);
      pTriplet->SetRecipientAddress(sRecipientAddress);
      
      if (!PersistentGreyList::AddObject(pTriplet))
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4326, "GreyListing::GetAllowSend", "Failed to save grey listing triplet in the database.");

         // Allow delivery since there appears to be internal
         // system problems.
         return true;
      }

      return false;
      
   }
}