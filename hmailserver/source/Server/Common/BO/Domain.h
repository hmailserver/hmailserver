// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


#pragma once

#include "../Cache/Cacheable.h"

namespace HM
{

   class DistributionLists;
   class Aliases;
   class Accounts;
   class DomainAliases;

   class Domain : public Cacheable, public BusinessObject<Domain>
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

      String GetName() const { return m_sName; } 
      void SetName(const String & newVal) { m_sName = newVal; };

      String GetPostmaster() const { return m_sPostmaster; } 
      void SetPostmaster(const String & newVal) { m_sPostmaster = newVal; };

      String GetADDomainName() const { return m_sADDomainName; } 
      void SetADDomainName(const String & newVal) { m_sADDomainName = newVal; };

      bool GetIsActive() const { return m_iActive ? true : false; } 
      void SetIsActive(bool newVal) { m_iActive = newVal ? 1 : 0; }
      
      int GetMaxMessageSize() const {return m_iMaxMessageSize;}
      void SetMaxMessageSize(int iNewVal) {m_iMaxMessageSize = iNewVal; }
      
      bool GetUsePlusAddressing() const {return m_bUsePlusAddressing; }
      void SetUsePlusAddressing(bool bNewVal) {m_bUsePlusAddressing = bNewVal; }

      String GetPlusAddressingChar() const {return m_sPlusAddressingChar; }
      void SetPlusAddressingChar(const String &sNewVal) {m_sPlusAddressingChar = sNewVal; }

      int GetAntiSpamOptions() const {return m_iAntiSpamOptions;}
      void SetAntiSpamOptions(int iNewVal) {m_iAntiSpamOptions = iNewVal; }

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

      shared_ptr<Accounts> GetAccounts();
      shared_ptr<Accounts> GetAccounts(__int64 iAccountID);
      shared_ptr<Aliases> GetAliases();
      shared_ptr<DistributionLists> GetDistributionLists();
      shared_ptr<DomainAliases> GetDomainAliases();

      bool XMLStore(XNode *pParentNode, int iBackupOptions);
      bool XMLLoad(XNode *pDomainNode, int iRestoreOptions);
      bool XMLLoadSubItems(XNode *pDomainNode, int iRestoreOptions);

   protected:

      int m_iAntiSpamOptions;
      
      String m_sName;
      String m_sPostmaster;
      String m_sADDomainName;

      bool m_bUsePlusAddressing;
      String m_sPlusAddressingChar;

      bool m_bEnableSignature;
      DomainSignatureMethod m_eSignatureMethod;
      String m_sSignaturePlainText;
      String m_sSignatureHTML;
      bool m_bAddSignaturesToLocalMail;
      bool m_bAddSignaturesToReplies;
      int m_iActive;
      unsigned int m_iMaxMessageSize;
      int m_iMaxSizeMB;
      int m_iMaxNoOfAccounts;
      int m_iMaxNoOfAliases;
      int m_iMaxNoOfDistributionLists;
      int m_iLimitationsEnabled;
      int m_iMaxAccountSize;

      String _dkimSelector;
      String _dkimPrivateKeyFile;

      shared_ptr<Accounts> m_pAccounts;
      shared_ptr<Aliases> m_pAliases;
      shared_ptr<DistributionLists> m_pDistributionLists;
      shared_ptr<DomainAliases> m_pDomainAliases;
   };
}