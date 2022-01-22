// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SURBLServers;
   class GreyListingWhiteAddresses;
   class WhiteListAddresses;
   class DNSBlackLists;

   class AntiSpamConfiguration
   {
   public:

      bool Load();

	   AntiSpamConfiguration();
	   virtual ~AntiSpamConfiguration();

      int GetSpamThreshold();

      bool GetCheckHostInHelo();
      void SetCheckHostInHelo(bool bNewVal);

      int GetCheckHostInHeloScore();
      void SetCheckHostInHeloScore(int bNewVal);

      bool GetCheckPTR();
      void SetCheckPTR(bool bNewVal);

      int GetCheckPTRScore();
      void SetCheckPTRScore(int bNewVal);

      bool GetUseGreyListing();
      void SetUseGreyListing(bool bNewVal);

      bool GetGreylistingTemporarilyDisabled();
      void SetGreylistingTemporarilyDisabled(bool bNewVal);

      int GetGreyListingInitialDelay();
      void SetGreyListingInitialDelay(int iNewVal);

      int GetGreyListingInitialDelete();
      void SetGreyListingInitialDelete(int iNewVal);

      int GetGreyListingFinalDelete();
      void SetGreyListingFinalDelete(int iNewVal);

      bool GetAddHeaderSpam();
      void SetAddHeaderSpam(bool bNewVal);

      bool GetAddHeaderReason();
      void SetAddHeaderReason(bool bNewVal);

      bool GetPrependSubject();
      void SetPrependSubject(bool bNewVal);

      String GetPrependSubjectText();
      void SetPrependSubjectText(const String &sText);

      bool GetUseMXChecks();
      void SetUseMXChecks(bool bNewVal);

      int GetUseMXChecksScore();
      void SetUseMXChecksScore(int bNewVal);

      bool GetUseSPF();
      void SetUseSPF(bool bNewVal);

      int GetUseSPFScore();
      void SetUseSPFScore(int bNewVal);


      int GetSpamMarkThreshold();
      void SetSpamMarkThreshold(int bNewVal);

      int GetSpamDeleteThreshold();
      void SetSpamDeleteThreshold(int bNewVal);

      bool GetSpamAssassinEnabled();
      void SetSpamAssassinEnabled(bool bNewVal);

      int GetSpamAssassinScore();
      void SetSpamAssassinScore(int bNewVal);

      bool GetSpamAssassinMergeScore();
      void SetSpamAssassinMergeScore(bool bNewVal);

      String GetSpamAssassinHost();
      void SetSpamAssassinHost(const String &sNewVal);

      int GetSpamAssassinPort();
      void SetSpamAssassinPort(int bNewVal);

      int GetAntiSpamMaxSizeKB();
      void SetAntiSpamMaxSizeKB(int newVal);

      bool GetDKIMVerificationEnabled();
      void SetDKIMVerificationEnabled(bool newValue);

      int GetDKIMVerificationFailureScore();
      void SetDKIMVerificationFailureScore(int newValue);

      bool GetBypassGreyListingOnSPFSuccess();
      void SetBypassGreyListingOnSPFSuccess(bool newValue);

      bool GetBypassGreyListingOnMailFromMX();
      void SetBypassGreyListingOnMailFromMX(bool newValue);

      void ClearGreyListingTriplets();

      bool XMLStore(XNode *pBackupNode, int Options);
      bool XMLLoad(XNode *pBackupNode, int iRestoreOptions);

      std::shared_ptr<GreyListingWhiteAddresses> GetGreyListingWhiteAddresses();
      std::shared_ptr<WhiteListAddresses> GetWhiteListAddresses();
      std::shared_ptr<DNSBlackLists> GetDNSBlackLists() {return dnsBlackLists_;}
      std::shared_ptr<SURBLServers> GetSURBLServers() {return surblServers_;}

   private:
      
      std::shared_ptr<PropertySet> GetSettings_();

      bool greylisting_temporarily_disabled_;

      std::shared_ptr<DNSBlackLists> dnsBlackLists_;
      std::shared_ptr<SURBLServers> surblServers_;

   };
}
