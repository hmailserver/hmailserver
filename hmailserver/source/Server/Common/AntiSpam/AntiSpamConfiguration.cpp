// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "AntiSpamConfiguration.h"

#include "../BO/SURBLServers.h"
#include "../BO/DNSBlackLists.h"
#include "../BO/GreyListingWhiteAddresses.h"
#include "../BO/WhiteListAddresses.h"
#include "../Persistence/PersistentGreyList.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   AntiSpamConfiguration::AntiSpamConfiguration() :
      greylisting_temporarily_disabled_(false)
   { 
   
   }

   AntiSpamConfiguration::~AntiSpamConfiguration()
   {
   
   }

   bool 
   AntiSpamConfiguration::Load()
   {
      dnsBlackLists_ = std::shared_ptr<DNSBlackLists>(new DNSBlackLists);
      dnsBlackLists_->Refresh();

      surblServers_ = std::shared_ptr<SURBLServers>(new SURBLServers);
      surblServers_->Refresh();

      return true;
   }

   std::shared_ptr<PropertySet> 
   AntiSpamConfiguration::GetSettings_()
   {
      return Configuration::Instance()->GetSettings();
   }


   void
   AntiSpamConfiguration::SetCheckHostInHelo(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_AS_CHECKHOSTINHELO, newVal);
   }

   bool 
   AntiSpamConfiguration::GetCheckHostInHelo() 
   {
      return GetSettings_()->GetBool(PROPERTY_AS_CHECKHOSTINHELO);
   }

   void
   AntiSpamConfiguration::SetCheckHostInHeloScore(int newVal)
   {
      GetSettings_()->SetLong(PROPERTY_AS_CHECKHOSTINHELOSCORE, newVal);
   }

   int
   AntiSpamConfiguration::GetCheckHostInHeloScore() 
   {
      return GetSettings_()->GetLong(PROPERTY_AS_CHECKHOSTINHELOSCORE);
   }

   void
   AntiSpamConfiguration::SetCheckPTR(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_AS_CHECKPTR, newVal);
   }

   bool
   AntiSpamConfiguration::GetCheckPTR()
   {
      return GetSettings_()->GetBool(PROPERTY_AS_CHECKPTR);
   }

   void
   AntiSpamConfiguration::SetCheckPTRScore(int newVal)
   {
      GetSettings_()->SetLong(PROPERTY_AS_CHECKPTRSCORE, newVal);
   }

   int
   AntiSpamConfiguration::GetCheckPTRScore()
   {
      return GetSettings_()->GetLong(PROPERTY_AS_CHECKPTRSCORE);
   }

   void
   AntiSpamConfiguration::SetUseGreyListing(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_USEGREYLISTING, newVal);
   }

   bool 
   AntiSpamConfiguration::GetUseGreyListing() 
   {
      return GetSettings_()->GetBool(PROPERTY_USEGREYLISTING);
   }

   int
   AntiSpamConfiguration::GetGreyListingInitialDelay()
   {
      return GetSettings_()->GetLong(PROPERTY_GL_INITIALDELAY);
   }

   void 
   AntiSpamConfiguration::SetGreyListingInitialDelay(int lNewValue)
   {
      GetSettings_()->SetLong(PROPERTY_GL_INITIALDELAY, lNewValue);
   }

   int
   AntiSpamConfiguration::GetGreyListingInitialDelete()
   {
      return GetSettings_()->GetLong(PROPERTY_GL_INITIALDELETE);
   }

   void 
   AntiSpamConfiguration::SetGreyListingInitialDelete(int lNewValue)
   {
      GetSettings_()->SetLong(PROPERTY_GL_INITIALDELETE, lNewValue);
   }

   int
   AntiSpamConfiguration::GetGreyListingFinalDelete()
   {
      return GetSettings_()->GetLong(PROPERTY_GL_FINALDELETE);
   }

   void 
   AntiSpamConfiguration::SetGreyListingFinalDelete(int lNewValue)
   {
      GetSettings_()->SetLong(PROPERTY_GL_FINALDELETE, lNewValue);
   }

   void
   AntiSpamConfiguration::SetAddHeaderSpam(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_AS_ADDHEADERSPAM, newVal);
   }

   bool 
   AntiSpamConfiguration::GetAddHeaderSpam() 
   {
      return GetSettings_()->GetBool(PROPERTY_AS_ADDHEADERSPAM);
   }

   void
   AntiSpamConfiguration::SetAddHeaderReason(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_AS_ADDHEADERREASON, newVal);
   }

   bool 
   AntiSpamConfiguration::GetAddHeaderReason() 
   {
      return GetSettings_()->GetBool(PROPERTY_AS_ADDHEADERREASON);
   }

   void
   AntiSpamConfiguration::SetPrependSubject(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_AS_PREPENDSUBJECT, newVal);
   }

   bool 
   AntiSpamConfiguration::GetPrependSubject() 
   {
      return GetSettings_()->GetBool(PROPERTY_AS_PREPENDSUBJECT);
   }

   void
   AntiSpamConfiguration::SetPrependSubjectText(const String &sText)
   {
      GetSettings_()->SetString(PROPERTY_AS_PREPENDSUBJECTTEXT, sText);
   }

   String 
   AntiSpamConfiguration::GetPrependSubjectText() 
   {
      return GetSettings_()->GetString(PROPERTY_AS_PREPENDSUBJECTTEXT);
   }


   bool 
   AntiSpamConfiguration::XMLStore(XNode *pBackupNode, int iOptions)
   {
      // SURBL SERVERS
      if (!surblServers_->XMLStore(pBackupNode, iOptions))
         return false;

      // GREYLISTING WHITEADDRESSES
      if (!GetGreyListingWhiteAddresses()->XMLStore(pBackupNode, iOptions))
         return false;
   
      // WHITELIST
      if (!GetWhiteListAddresses()->XMLStore(pBackupNode, iOptions))
         return false;

      // DNS BLACK LISTS
      if (!dnsBlackLists_->XMLStore(pBackupNode, iOptions))
         return false;

      return true;
   }

   bool
   AntiSpamConfiguration::XMLLoad(XNode *pBackupNode, int iRestoreOptions)
   {
      // SURBL SERVERS
      surblServers_->Refresh();
      if (!surblServers_->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      // GREYLISTING WHITEADDRESSES
      if (!GetGreyListingWhiteAddresses()->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      // WHITELIST
      if (!GetWhiteListAddresses()->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      // DNS BLACK LISTS
      dnsBlackLists_->Refresh();
      if (!dnsBlackLists_->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      return true;
   }

   bool 
   AntiSpamConfiguration::GetGreylistingTemporarilyDisabled()
   {
      return greylisting_temporarily_disabled_;
   }

   void 
   AntiSpamConfiguration::SetGreylistingTemporarilyDisabled(bool bNewVal)
   {
      greylisting_temporarily_disabled_ = bNewVal;
   }

   void
   AntiSpamConfiguration::SetUseMXChecks(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_USEMXCHECK, newVal);
   }

   bool 
   AntiSpamConfiguration::GetUseMXChecks() 
   {
      return GetSettings_()->GetBool(PROPERTY_USEMXCHECK);
   }

   void
   AntiSpamConfiguration::SetUseMXChecksScore(int newVal)
   {
      GetSettings_()->SetLong(PROPERTY_USEMXCHECKSCORE, newVal);
   }

   int 
   AntiSpamConfiguration::GetUseMXChecksScore() 
   {
      return GetSettings_()->GetLong(PROPERTY_USEMXCHECKSCORE);
   }

   void
   AntiSpamConfiguration::SetSpamMarkThreshold(int newVal)
   {
      GetSettings_()->SetLong(PROPERTY_SPAMMARKTHRESHOLD, newVal);
   }

   int 
   AntiSpamConfiguration::GetSpamMarkThreshold() 
   {
      return GetSettings_()->GetLong(PROPERTY_SPAMMARKTHRESHOLD);
   }

   void
   AntiSpamConfiguration::SetSpamDeleteThreshold(int newVal)
   {
      GetSettings_()->SetLong(PROPERTY_SPAMDELETETHRESHOLD, newVal);
   }

   int 
   AntiSpamConfiguration::GetSpamDeleteThreshold() 
   {
      return GetSettings_()->GetLong(PROPERTY_SPAMDELETETHRESHOLD);
   }

   int
   AntiSpamConfiguration::GetSpamThreshold()
   {
      int iMarkThreshold = GetSpamMarkThreshold();
      int iDeleteThreshold  = GetSpamDeleteThreshold();

      return std::min(iMarkThreshold, iDeleteThreshold);
   }

   void
   AntiSpamConfiguration::SetUseSPF(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_USESPF, newVal);
   }

   bool 
   AntiSpamConfiguration::GetUseSPF() 
   {
      return GetSettings_()->GetBool(PROPERTY_USESPF);
   }

   void
   AntiSpamConfiguration::SetUseSPFScore(int newVal)
   {
      GetSettings_()->SetLong(PROPERTY_USESPFSCORE, newVal);
   }

   int
   AntiSpamConfiguration::GetUseSPFScore() 
   {
      return GetSettings_()->GetLong(PROPERTY_USESPFSCORE);
   }

   void
   AntiSpamConfiguration::SetSpamAssassinEnabled(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_SPAMASSASSIN_ENABLED, newVal);
   }

   bool 
   AntiSpamConfiguration::GetSpamAssassinEnabled() 
   {
      return GetSettings_()->GetBool(PROPERTY_SPAMASSASSIN_ENABLED);
   }

   void
   AntiSpamConfiguration::SetSpamAssassinScore(int newVal)
   {
      GetSettings_()->SetLong(PROPERTY_SPAMASSASSIN_SCORE, newVal);
   }

   int 
   AntiSpamConfiguration::GetSpamAssassinScore() 
   {
      return GetSettings_()->GetLong(PROPERTY_SPAMASSASSIN_SCORE);
   }

   void
   AntiSpamConfiguration::SetSpamAssassinMergeScore(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_SPAMASSASSIN_MERGE_SCORE, newVal);
   }

   bool 
   AntiSpamConfiguration::GetSpamAssassinMergeScore() 
   {
      return GetSettings_()->GetBool(PROPERTY_SPAMASSASSIN_MERGE_SCORE);
   }

   void
   AntiSpamConfiguration::SetSpamAssassinHost(const String &newVal)
   {
      GetSettings_()->SetString(PROPERTY_SPAMASSASSIN_HOST, newVal);
   }

   String 
   AntiSpamConfiguration::GetSpamAssassinHost() 
   {
      return GetSettings_()->GetString(PROPERTY_SPAMASSASSIN_HOST);
   }

   void
   AntiSpamConfiguration::SetSpamAssassinPort(int newVal)
   {
      GetSettings_()->SetLong(PROPERTY_SPAMASSASSIN_PORT, newVal);
   }

   int 
   AntiSpamConfiguration::GetSpamAssassinPort() 
   {
      return GetSettings_()->GetLong(PROPERTY_SPAMASSASSIN_PORT);
   }

   int 
   AntiSpamConfiguration::GetAntiSpamMaxSizeKB()
   {
      return GetSettings_()->GetLong(PROPERTY_AS_MAX_SIZE);
   }

   void 
   AntiSpamConfiguration::SetAntiSpamMaxSizeKB(int newVal)
   {
      GetSettings_()->SetLong(PROPERTY_AS_MAX_SIZE, newVal);
   }

   void 
   AntiSpamConfiguration::ClearGreyListingTriplets()
   {
      HM::PersistentGreyList::ClearAllRecords();
   }

   bool 
   AntiSpamConfiguration::GetDKIMVerificationEnabled()
   {
      return GetSettings_()->GetBool(PROPERTY_AS_DKIM_VERIFICATION_ENABLE  );
   }

   void
   AntiSpamConfiguration::SetDKIMVerificationEnabled(bool newValue)
   {
      GetSettings_()->SetBool(PROPERTY_AS_DKIM_VERIFICATION_ENABLE , newValue);
   }


   int
   AntiSpamConfiguration::GetDKIMVerificationFailureScore()
   {
      return GetSettings_()->GetLong(PROPERTY_AS_DKIM_VERIFICATION_FAILURE_SCORE);
   }

   void
   AntiSpamConfiguration::SetDKIMVerificationFailureScore(int newValue)
   {
      GetSettings_()->SetLong(PROPERTY_AS_DKIM_VERIFICATION_FAILURE_SCORE, newValue);
   }

   bool 
   AntiSpamConfiguration::GetBypassGreyListingOnSPFSuccess()
   {
      return GetSettings_()->GetBool(PROPERTY_BYPASS_GREYLISTING_ON_SPFSUCCESS  );
   }

   void
   AntiSpamConfiguration::SetBypassGreyListingOnSPFSuccess(bool newValue)
   {
      GetSettings_()->SetBool(PROPERTY_BYPASS_GREYLISTING_ON_SPFSUCCESS , newValue);
   }

   bool 
   AntiSpamConfiguration::GetBypassGreyListingOnMailFromMX()
   {
      return GetSettings_()->GetBool(PROPERTY_BYPASS_GREYLISTING_ON_MAILFROMMX  );
   }

   void
   AntiSpamConfiguration::SetBypassGreyListingOnMailFromMX(bool newValue)
   {
      GetSettings_()->SetBool(PROPERTY_BYPASS_GREYLISTING_ON_MAILFROMMX , newValue);
   }

   std::shared_ptr<WhiteListAddresses> 
   AntiSpamConfiguration::GetWhiteListAddresses()
   {
      std::shared_ptr<WhiteListAddresses> whiteListAddresses = std::shared_ptr<WhiteListAddresses>(new WhiteListAddresses);
      whiteListAddresses->Refresh();
      return whiteListAddresses;
   }

   std::shared_ptr<GreyListingWhiteAddresses>
   AntiSpamConfiguration::GetGreyListingWhiteAddresses()
   {
      std::shared_ptr<GreyListingWhiteAddresses> greyListingWhiteAddresses = std::shared_ptr<GreyListingWhiteAddresses>(new GreyListingWhiteAddresses);
      greyListingWhiteAddresses->Refresh();

      return greyListingWhiteAddresses;
   }




}



