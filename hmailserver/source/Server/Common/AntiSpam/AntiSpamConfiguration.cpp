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
      m_bGreylistingTemporarilyDisabled(false)
   { 
   
   }

   AntiSpamConfiguration::~AntiSpamConfiguration()
   {
   
   }

   bool 
   AntiSpamConfiguration::Load()
   {
      _dnsBlackLists = shared_ptr<DNSBlackLists>(new DNSBlackLists);
      _dnsBlackLists->Refresh();

      _surblServers = shared_ptr<SURBLServers>(new SURBLServers);
      _surblServers->Refresh();

      return true;
   }

   shared_ptr<PropertySet> 
   AntiSpamConfiguration::_GetSettings()
   {
      return Configuration::Instance()->GetSettings();
   }


   void
   AntiSpamConfiguration::SetCheckHostInHelo(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_AS_CHECKHOSTINHELO, newVal);
   }

   bool 
   AntiSpamConfiguration::GetCheckHostInHelo() 
   {
      return _GetSettings()->GetBool(PROPERTY_AS_CHECKHOSTINHELO);
   }

   void
   AntiSpamConfiguration::SetCheckHostInHeloScore(int newVal)
   {
      _GetSettings()->SetLong(PROPERTY_AS_CHECKHOSTINHELOSCORE, newVal);
   }

   int
   AntiSpamConfiguration::GetCheckHostInHeloScore() 
   {
      return _GetSettings()->GetLong(PROPERTY_AS_CHECKHOSTINHELOSCORE);
   }


   void
   AntiSpamConfiguration::SetUseGreyListing(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_USEGREYLISTING, newVal);
   }

   bool 
   AntiSpamConfiguration::GetUseGreyListing() 
   {
      return _GetSettings()->GetBool(PROPERTY_USEGREYLISTING);
   }

   int
   AntiSpamConfiguration::GetGreyListingInitialDelay()
   {
      return _GetSettings()->GetLong(PROPERTY_GL_INITIALDELAY);
   }

   void 
   AntiSpamConfiguration::SetGreyListingInitialDelay(int lNewValue)
   {
      _GetSettings()->SetLong(PROPERTY_GL_INITIALDELAY, lNewValue);
   }

   int
   AntiSpamConfiguration::GetGreyListingInitialDelete()
   {
      return _GetSettings()->GetLong(PROPERTY_GL_INITIALDELETE);
   }

   void 
   AntiSpamConfiguration::SetGreyListingInitialDelete(int lNewValue)
   {
      _GetSettings()->SetLong(PROPERTY_GL_INITIALDELETE, lNewValue);
   }

   int
   AntiSpamConfiguration::GetGreyListingFinalDelete()
   {
      return _GetSettings()->GetLong(PROPERTY_GL_FINALDELETE);
   }

   void 
   AntiSpamConfiguration::SetGreyListingFinalDelete(int lNewValue)
   {
      _GetSettings()->SetLong(PROPERTY_GL_FINALDELETE, lNewValue);
   }

   void
   AntiSpamConfiguration::SetAddHeaderSpam(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_AS_ADDHEADERSPAM, newVal);
   }

   bool 
   AntiSpamConfiguration::GetAddHeaderSpam() 
   {
      return _GetSettings()->GetBool(PROPERTY_AS_ADDHEADERSPAM);
   }

   void
   AntiSpamConfiguration::SetAddHeaderReason(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_AS_ADDHEADERREASON, newVal);
   }

   bool 
   AntiSpamConfiguration::GetAddHeaderReason() 
   {
      return _GetSettings()->GetBool(PROPERTY_AS_ADDHEADERREASON);
   }

   void
   AntiSpamConfiguration::SetPrependSubject(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_AS_PREPENDSUBJECT, newVal);
   }

   bool 
   AntiSpamConfiguration::GetPrependSubject() 
   {
      return _GetSettings()->GetBool(PROPERTY_AS_PREPENDSUBJECT);
   }

   void
   AntiSpamConfiguration::SetPrependSubjectText(const String &sText)
   {
      _GetSettings()->SetString(PROPERTY_AS_PREPENDSUBJECTTEXT, sText);
   }

   String 
   AntiSpamConfiguration::GetPrependSubjectText() 
   {
      return _GetSettings()->GetString(PROPERTY_AS_PREPENDSUBJECTTEXT);
   }


   bool 
   AntiSpamConfiguration::XMLStore(XNode *pBackupNode, int iOptions)
   {
      // SURBL SERVERS
      if (!_surblServers->XMLStore(pBackupNode, iOptions))
         return false;

      // GREYLISTING WHITEADDRESSES
      if (!GetGreyListingWhiteAddresses()->XMLStore(pBackupNode, iOptions))
         return false;
   
      // WHITELIST
      if (!GetWhiteListAddresses()->XMLStore(pBackupNode, iOptions))
         return false;

      // DNS BLACK LISTS
      if (!_dnsBlackLists->XMLStore(pBackupNode, iOptions))
         return false;

      return true;
   }

   bool
   AntiSpamConfiguration::XMLLoad(XNode *pBackupNode, int iRestoreOptions)
   {
      // SURBL SERVERS
      _surblServers->Refresh();
      if (!_surblServers->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      // GREYLISTING WHITEADDRESSES
      if (!GetGreyListingWhiteAddresses()->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      // WHITELIST
      if (!GetWhiteListAddresses()->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      // DNS BLACK LISTS
      _dnsBlackLists->Refresh();
      if (!_dnsBlackLists->XMLLoad(pBackupNode, iRestoreOptions))
         return false;

      return true;
   }

   bool 
   AntiSpamConfiguration::GetGreylistingTemporarilyDisabled()
   {
      return m_bGreylistingTemporarilyDisabled;
   }

   void 
   AntiSpamConfiguration::SetGreylistingTemporarilyDisabled(bool bNewVal)
   {
      m_bGreylistingTemporarilyDisabled = bNewVal;
   }

   void
   AntiSpamConfiguration::SetUseMXChecks(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_USEMXCHECK, newVal);
   }

   bool 
   AntiSpamConfiguration::GetUseMXChecks() 
   {
      return _GetSettings()->GetBool(PROPERTY_USEMXCHECK);
   }

   void
   AntiSpamConfiguration::SetUseMXChecksScore(int newVal)
   {
      _GetSettings()->SetLong(PROPERTY_USEMXCHECKSCORE, newVal);
   }

   int 
   AntiSpamConfiguration::GetUseMXChecksScore() 
   {
      return _GetSettings()->GetLong(PROPERTY_USEMXCHECKSCORE);
   }

   void
   AntiSpamConfiguration::SetSpamMarkThreshold(int newVal)
   {
      _GetSettings()->SetLong(PROPERTY_SPAMMARKTHRESHOLD, newVal);
   }

   int 
   AntiSpamConfiguration::GetSpamMarkThreshold() 
   {
      return _GetSettings()->GetLong(PROPERTY_SPAMMARKTHRESHOLD);
   }

   void
   AntiSpamConfiguration::SetSpamDeleteThreshold(int newVal)
   {
      _GetSettings()->SetLong(PROPERTY_SPAMDELETETHRESHOLD, newVal);
   }

   int 
   AntiSpamConfiguration::GetSpamDeleteThreshold() 
   {
      return _GetSettings()->GetLong(PROPERTY_SPAMDELETETHRESHOLD);
   }

   int
   AntiSpamConfiguration::GetSpamThreshold()
   {
      int iMarkThreshold = GetSpamMarkThreshold();
      int iDeleteThreshold  = GetSpamDeleteThreshold();

      return min(iMarkThreshold, iDeleteThreshold);
   }

   void
   AntiSpamConfiguration::SetUseSPF(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_USESPF, newVal);
   }

   bool 
   AntiSpamConfiguration::GetUseSPF() 
   {
      return _GetSettings()->GetBool(PROPERTY_USESPF);
   }

   void
   AntiSpamConfiguration::SetUseSPFScore(int newVal)
   {
      _GetSettings()->SetLong(PROPERTY_USESPFSCORE, newVal);
   }

   int
   AntiSpamConfiguration::GetUseSPFScore() 
   {
      return _GetSettings()->GetLong(PROPERTY_USESPFSCORE);
   }

   void
   AntiSpamConfiguration::SetSpamAssassinEnabled(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_SPAMASSASSIN_ENABLED, newVal);
   }

   bool 
   AntiSpamConfiguration::GetSpamAssassinEnabled() 
   {
      return _GetSettings()->GetBool(PROPERTY_SPAMASSASSIN_ENABLED);
   }

   void
   AntiSpamConfiguration::SetSpamAssassinScore(int newVal)
   {
      _GetSettings()->SetLong(PROPERTY_SPAMASSASSIN_SCORE, newVal);
   }

   int 
   AntiSpamConfiguration::GetSpamAssassinScore() 
   {
      return _GetSettings()->GetLong(PROPERTY_SPAMASSASSIN_SCORE);
   }

   void
   AntiSpamConfiguration::SetSpamAssassinMergeScore(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_SPAMASSASSIN_MERGE_SCORE, newVal);
   }

   bool 
   AntiSpamConfiguration::GetSpamAssassinMergeScore() 
   {
      return _GetSettings()->GetBool(PROPERTY_SPAMASSASSIN_MERGE_SCORE);
   }

   void
   AntiSpamConfiguration::SetSpamAssassinHost(const String &newVal)
   {
      _GetSettings()->SetString(PROPERTY_SPAMASSASSIN_HOST, newVal);
   }

   String 
   AntiSpamConfiguration::GetSpamAssassinHost() 
   {
      return _GetSettings()->GetString(PROPERTY_SPAMASSASSIN_HOST);
   }

   void
   AntiSpamConfiguration::SetSpamAssassinPort(int newVal)
   {
      _GetSettings()->SetLong(PROPERTY_SPAMASSASSIN_PORT, newVal);
   }

   int 
   AntiSpamConfiguration::GetSpamAssassinPort() 
   {
      return _GetSettings()->GetLong(PROPERTY_SPAMASSASSIN_PORT);
   }

   int 
   AntiSpamConfiguration::GetAntiSpamMaxSizeKB()
   {
      return _GetSettings()->GetLong(PROPERTY_AS_MAX_SIZE);
   }

   void 
   AntiSpamConfiguration::SetAntiSpamMaxSizeKB(int newVal)
   {
      _GetSettings()->SetLong(PROPERTY_AS_MAX_SIZE, newVal);
   }

   void 
   AntiSpamConfiguration::ClearGreyListingTriplets()
   {
      HM::PersistentGreyList::ClearAllRecords();
   }

   bool 
   AntiSpamConfiguration::GetDKIMVerificationEnabled()
   {
      return _GetSettings()->GetBool(PROPERTY_AS_DKIM_VERIFICATION_ENABLE  );
   }

   void
   AntiSpamConfiguration::SetDKIMVerificationEnabled(bool newValue)
   {
      _GetSettings()->SetBool(PROPERTY_AS_DKIM_VERIFICATION_ENABLE , newValue);
   }


   int
   AntiSpamConfiguration::GetDKIMVerificationFailureScore()
   {
      return _GetSettings()->GetLong(PROPERTY_AS_DKIM_VERIFICATION_FAILURE_SCORE);
   }

   void
   AntiSpamConfiguration::SetDKIMVerificationFailureScore(int newValue)
   {
      _GetSettings()->SetLong(PROPERTY_AS_DKIM_VERIFICATION_FAILURE_SCORE, newValue);
   }

   bool 
   AntiSpamConfiguration::GetBypassGreyListingOnSPFSuccess()
   {
      return _GetSettings()->GetBool(PROPERTY_BYPASS_GREYLISTING_ON_SPFSUCCESS  );
   }

   void
   AntiSpamConfiguration::SetBypassGreyListingOnSPFSuccess(bool newValue)
   {
      _GetSettings()->SetBool(PROPERTY_BYPASS_GREYLISTING_ON_SPFSUCCESS , newValue);
   }

   bool 
   AntiSpamConfiguration::GetBypassGreyListingOnMailFromMX()
   {
      return _GetSettings()->GetBool(PROPERTY_BYPASS_GREYLISTING_ON_MAILFROMMX  );
   }

   void
   AntiSpamConfiguration::SetBypassGreyListingOnMailFromMX(bool newValue)
   {
      _GetSettings()->SetBool(PROPERTY_BYPASS_GREYLISTING_ON_MAILFROMMX , newValue);
   }

   shared_ptr<WhiteListAddresses> 
   AntiSpamConfiguration::GetWhiteListAddresses()
   {
      shared_ptr<WhiteListAddresses> whiteListAddresses = shared_ptr<WhiteListAddresses>(new WhiteListAddresses);
      whiteListAddresses->Refresh();
      return whiteListAddresses;
   }

   shared_ptr<GreyListingWhiteAddresses>
   AntiSpamConfiguration::GetGreyListingWhiteAddresses()
   {
      shared_ptr<GreyListingWhiteAddresses> greyListingWhiteAddresses = shared_ptr<GreyListingWhiteAddresses>(new GreyListingWhiteAddresses);
      greyListingWhiteAddresses->Refresh();

      return greyListingWhiteAddresses;
   }




}



