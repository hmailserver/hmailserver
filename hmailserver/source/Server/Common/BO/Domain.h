// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


#pragma once

namespace HM
{

   class DistributionLists;
   class Aliases;
   class Accounts;
   class DomainAliases;

   class Domain : public BusinessObject<Domain>
   {
   public:
	   Domain();
	   virtual ~Domain();

      enum AntiSpamOptions
      {
         ASUseGreylisting = 1,
         
         ASDKIMSign = 2,
         ASDKIMSimpleHeader = 4,
         ASDKIMSimpleBody = 8,
         ASDKIMSHA1 = 16,
         ASDKIMSignAliases = 32,
      };

      enum DomainSignatureMethod
      {
         SMUnknown = 0,
         SMSetIfNotSpecifiedInAccount = 1,
         SMOverwriteAccountSignature = 2,
         SMAppendToAccountSignature = 3
      };
      
      enum DomainLimitations
      {
         MaxNoOfAccounts = 1,
         MaxNoOfAliases = 2,
         MaxNoOfDistributionLists = 4
      };

      String GetName() const { return name_; } 
      void SetName(const String & newVal) { name_ = newVal; };

      String GetPostmaster() const { return postmaster_; } 
      void SetPostmaster(const String & newVal) { postmaster_ = newVal; };

      String GetADDomainName() const { return addomain_name_; } 
      void SetADDomainName(const String & newVal) { addomain_name_ = newVal; };

      bool GetIsActive() const { return active_ ? true : false; } 
      void SetIsActive(bool newVal) { active_ = newVal ? 1 : 0; }
      
      int GetMaxMessageSize() const {return max_message_size_;}
      void SetMaxMessageSize(int iNewVal) {max_message_size_ = iNewVal; }
      
      bool GetUsePlusAddressing() const {return use_plus_addressing_; }
      void SetUsePlusAddressing(bool bNewVal) {use_plus_addressing_ = bNewVal; }

      String GetPlusAddressingChar() const {return plus_addressing_char_; }
      void SetPlusAddressingChar(const String &sNewVal) {plus_addressing_char_ = sNewVal; }

      int GetAntiSpamOptions() const {return anti_spam_options_;}
      void SetAntiSpamOptions(int iNewVal) {anti_spam_options_ = iNewVal; }

      bool GetASUseGreyListing() const;
      void SetASUseGreyListing(bool bNewVal);

      int GetMaxSizeMB() const;
      void SetMaxSizeMB(int iNewVal);

      bool GetEnableSignature() const;
      void SetEnableSignature(bool bNewVal);

      DomainSignatureMethod GetSignatureMethod() const;
      void SetSignatureMethod(DomainSignatureMethod sm);

      String GetSignaturePlainText() const;
      void SetSignaturePlainText(const String &sNewVal);

      String GetSignatureHTML() const;
      void SetSignatureHTML(const String &sNewVal);

      bool GetAddSignaturesToReplies() const;
      void SetAddSignaturesToReplies(bool bNewVal);

      bool GetAddSignaturesToLocalMail() const;
      void SetAddSignaturesToLocalMail(bool bNewVal);

      int GetMaxNoOfAccounts() const;
      void SetMaxNoOfAccounts(int iNewVal);
      int GetMaxNoOfAliases() const;
      void SetMaxNoOfAliases(int iNewVal);
      int GetMaxNoOfDistributionLists() const;
      void SetMaxNoOfDistributionLists(int iNewVal);

      int GetLimitationsEnabled() const;
      void SetLimitationsEnabled(int iNewVal);

      bool GetMaxNoOfAccountsEnabled() const;
      void SetMaxNoOfAccountsEnabled(bool iNewVal);
      bool GetMaxNoOfAliasesEnabled() const;
      void SetMaxNoOfAliasesEnabled(bool iNewVal);
      bool GetMaxNoOfDistributionListsEnabled() const;
      void SetMaxNoOfDistributionListsEnabled(bool iNewVal);

      int GetMaxAccountSize() const;
      void SetMaxAccountSize(int iNewVal);
      
      /*
         DKIM START

      */
      bool GetDKIMEnabled() const;
      void SetDKIMEnabled(bool newVal);

      bool GetDKIMAliasesEnabled() const;
      void SetDKIMAliasesEnabled(bool newVal);


      AnsiString GetDKIMSelector() const;
      void SetDKIMSelector(const String &newValue);

      String GetDKIMPrivateKeyFile() const;
      void SetDKIMPrivateKeyFile(const String &newValue);

      int GetDKIMHeaderCanonicalizationMethod() const;
      void SetDKIMHeaderCanonicalizationMethod(int newValue);

      int GetDKIMBodyCanonicalizationMethod() const;
      void SetDKIMBodyCanonicalizationMethod(int newValue);

      int GetDKIMSigningAlgorithm() const;
      void SetDKIMSigningAlgorithm(int newValue);
      /*
         DKIM END
      */

      std::shared_ptr<Accounts> GetAccounts();
      std::shared_ptr<Accounts> GetAccounts(__int64 iAccountID);
      std::shared_ptr<Aliases> GetAliases();
      std::shared_ptr<DistributionLists> GetDistributionLists();
      std::shared_ptr<DomainAliases> GetDomainAliases();

      bool XMLStore(XNode *pParentNode, int iBackupOptions);
      bool XMLLoad(XNode *pDomainNode, int iRestoreOptions);
      bool XMLLoadSubItems(XNode *pDomainNode, int iRestoreOptions);

      size_t GetEstimatedCachingSize();
   protected:

      int anti_spam_options_;
      
      String name_;
      String postmaster_;
      String addomain_name_;

      bool use_plus_addressing_;
      String plus_addressing_char_;

      bool enable_signature_;
      DomainSignatureMethod signature_method_;
      String signature_plain_text_;
      String signature_html_;
      bool add_signatures_to_local_mail_;
      bool add_signatures_to_replies_;
      int active_;
      unsigned int max_message_size_;
      int max_size_mb_;
      int max_no_of_accounts_;
      int max_no_of_aliases_;
      int max_no_of_distribution_lists_;
      int limitations_enabled_;
      int max_account_size_;

      String dkim_selector_;
      String dkim_private_key_file_;

      std::shared_ptr<Accounts> accounts_;
      std::shared_ptr<Aliases> aliases_;
      std::shared_ptr<DistributionLists> distribution_lists_;
      std::shared_ptr<DomainAliases> domain_aliases_;
   };
}