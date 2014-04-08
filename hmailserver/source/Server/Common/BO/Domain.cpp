// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


#include "stdafx.h"

#include "Domain.h"

#include "Accounts.h"
#include "Aliases.h"
#include "DistributionLists.h"
#include "DomainAliases.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Domain::Domain() :
      m_bUsePlusAddressing(false),
      m_iActive(0),
      m_iMaxMessageSize(0),
      m_sPlusAddressingChar("+"),
      m_iAntiSpamOptions(1),
      m_iMaxSizeMB(0),
      m_bEnableSignature(false),
      m_eSignatureMethod(SMSetIfNotSpecifiedInAccount),
      m_bAddSignaturesToLocalMail(true),
      m_bAddSignaturesToReplies(false),
      m_iMaxNoOfAccounts(0),
      m_iMaxNoOfAliases(0),
      m_iMaxNoOfDistributionLists(0),
      m_iMaxAccountSize(0),
      m_iLimitationsEnabled(0)
   {
      
   }

   Domain::~Domain()
   {

   }

   shared_ptr<Accounts>
   Domain::GetAccounts()
   {
      if (!m_pAccounts)
         m_pAccounts = shared_ptr<Accounts>(new Accounts(m_iID));

      m_pAccounts->Refresh();
      return m_pAccounts;
   }


   shared_ptr<DomainAliases>
   Domain::GetDomainAliases()
   {
      if (!m_pDomainAliases)
         m_pDomainAliases = shared_ptr<DomainAliases>(new DomainAliases(m_iID));

      m_pDomainAliases->Refresh();
      return m_pDomainAliases;
   }

   shared_ptr<Accounts>
   Domain::GetAccounts(__int64 iAccountID)
   {
      if (!m_pAccounts)
      {
         // Only fetch a specific account and put it in the list. This happens
         // when an API client only has limited access.
         m_pAccounts = shared_ptr<Accounts>(new Accounts(m_iID, iAccountID));
      }

      m_pAccounts->Refresh();
      return m_pAccounts;
   }

   shared_ptr<Aliases>
   Domain::GetAliases()
   {
      if (!m_pAliases)
         m_pAliases = shared_ptr<Aliases>(new Aliases(m_iID));
      
      m_pAliases->Refresh();

      return m_pAliases;
   }

   shared_ptr<DistributionLists>
   Domain::GetDistributionLists()
   {
      if (!m_pDistributionLists)
         m_pDistributionLists = shared_ptr<DistributionLists>(new DistributionLists(m_iID));

      m_pDistributionLists->Refresh();

      return m_pDistributionLists;
   }

   bool 
   Domain::XMLStore(XNode *pParentNode, int iBackupOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Domain"));

      pNode->AppendAttr(_T("Name"), m_sName);
      pNode->AppendAttr(_T("Postmaster"), m_sPostmaster);
      pNode->AppendAttr(_T("ADDomainName"), m_sADDomainName);
      pNode->AppendAttr(_T("Active"), StringParser::IntToString(m_iActive));
      pNode->AppendAttr(_T("MaxMessageSize"), StringParser::IntToString(m_iMaxMessageSize));
      pNode->AppendAttr(_T("MaxSize"), StringParser::IntToString(m_iMaxSizeMB));
      pNode->AppendAttr(_T("MaxAccountSize"), StringParser::IntToString(m_iMaxAccountSize));

      pNode->AppendAttr(_T("UsePlusAddressing"), m_bUsePlusAddressing ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("PlusAddressingChar"), m_sPlusAddressingChar);
      pNode->AppendAttr(_T("AntiSpamOptions"), StringParser::IntToString(m_iAntiSpamOptions));

      pNode->AppendAttr(_T("EnableSignature"), m_bEnableSignature ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("SignatureMethod"), StringParser::IntToString(m_eSignatureMethod));
      pNode->AppendAttr(_T("SignaturePlainText"), m_sSignaturePlainText);
      pNode->AppendAttr(_T("SignatureHTML"), m_sSignatureHTML);
      pNode->AppendAttr(_T("AddSignaturesToLocalMail"), m_bAddSignaturesToLocalMail ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("AddSignaturesToReplies"), m_bAddSignaturesToReplies ? _T("1") : _T("0"));

      pNode->AppendAttr(_T("MaxNoOfAccounts"), StringParser::IntToString(m_iMaxNoOfAccounts));
      pNode->AppendAttr(_T("MaxNoOfAliases"), StringParser::IntToString(m_iMaxNoOfAliases));
      pNode->AppendAttr(_T("MaxNoOfLists"), StringParser::IntToString(m_iMaxNoOfDistributionLists));
      pNode->AppendAttr(_T("LimitationsEnabled"), StringParser::IntToString(m_iLimitationsEnabled));

      pNode->AppendAttr(_T("DKIMSelector"), _dkimSelector);
      pNode->AppendAttr(_T("DKIMPrivateKeyFile"), _dkimPrivateKeyFile);

      if (!GetDomainAliases()->XMLStore(pNode, iBackupOptions))
         return false;

      // Accounts
      if (!GetAccounts()->XMLStore(pNode, iBackupOptions))
         return false;

      if (!GetAliases()->XMLStore(pNode, iBackupOptions))
         return false;

      if (!GetDistributionLists()->XMLStore(pNode, iBackupOptions))
         return false;

      return true;
   }

   bool 
   Domain::XMLLoad(XNode *pNode, int iRestoreOptions)
   {
      m_sName = pNode->GetAttrValue(_T("Name"));
      m_sPostmaster = pNode->GetAttrValue(_T("Postmaster"));
      m_sADDomainName = pNode->GetAttrValue(_T("ADDomainName"));
      m_iActive = _ttoi(pNode->GetAttrValue(_T("Active")));
      m_iMaxMessageSize = _ttoi(pNode->GetAttrValue(_T("MaxMessageSize")));
      m_iMaxSizeMB = _ttoi(pNode->GetAttrValue(_T("MaxSize")));
      m_iMaxAccountSize = _ttoi(pNode->GetAttrValue(_T("MaxAccountSize")));

      m_bUsePlusAddressing = pNode->GetAttrValue(_T("UsePlusAddressing")) == _T("1");
      m_sPlusAddressingChar = pNode->GetAttrValue(_T("PlusAddressingChar"));
      m_iAntiSpamOptions = _ttoi(pNode->GetAttrValue(_T("AntiSpamOptions")));

      m_bEnableSignature = pNode->GetAttrValue(_T("EnableSignature")) == _T("1");
      m_eSignatureMethod = (DomainSignatureMethod) _ttoi(pNode->GetAttrValue(_T("SignatureMethod")));
      m_sSignaturePlainText = pNode->GetAttrValue(_T("SignaturePlainText"));
      m_sSignatureHTML = pNode->GetAttrValue(_T("SignatureHTML"));

      m_bAddSignaturesToLocalMail = pNode->GetAttrValue(_T("AddSignaturesToLocalMail")) == _T("1");
      m_bAddSignaturesToReplies = pNode->GetAttrValue(_T("AddSignaturesToReplies")) == _T("1");

      m_iMaxNoOfAccounts = _ttoi(pNode->GetAttrValue(_T("MaxNoOfAccounts")));
      m_iMaxNoOfAliases = _ttoi(pNode->GetAttrValue(_T("MaxNoOfAliases")));
      m_iMaxNoOfDistributionLists = _ttoi(pNode->GetAttrValue(_T("MaxNoOfLists")));
      
      m_iLimitationsEnabled = _ttoi(pNode->GetAttrValue(_T("LimitationsEnabled")));

      _dkimSelector = pNode->GetAttrValue(_T("DKIMSelector"));
      _dkimPrivateKeyFile = pNode->GetAttrValue(_T("DKIMPrivateKeyFile"));

      return true;
   }

   bool 
   Domain::XMLLoadSubItems(XNode *pNode, int iRestoreOptions)
   {
      if (!GetDomainAliases()->XMLLoad(pNode, iRestoreOptions))
         return false;

      if (!GetAccounts()->XMLLoad(pNode, iRestoreOptions))
         return false;

      if (!GetAliases()->XMLLoad(pNode, iRestoreOptions))
         return false;

      if (!GetDistributionLists()->XMLLoad(pNode, iRestoreOptions))
         return false;

      return true;
   }

   bool 
   Domain::GetASUseGreyListing() const
   {
      return (m_iAntiSpamOptions & ASUseGreylisting) ? true : false;
   }

   void 
   Domain::SetASUseGreyListing(bool bNewVal)
   {
      if (bNewVal)
         m_iAntiSpamOptions = m_iAntiSpamOptions | ASUseGreylisting;
      else
         m_iAntiSpamOptions = m_iAntiSpamOptions & ~ASUseGreylisting;      

   }

   int
   Domain::GetMaxSizeMB() const
   {
      return m_iMaxSizeMB;
   }

   void 
   Domain::SetMaxSizeMB(int iNewVal)
   {
      m_iMaxSizeMB = iNewVal;
   }

   bool  
   Domain::GetEnableSignature() const
   {
      return m_bEnableSignature;
   }

   void 
   Domain::SetEnableSignature(bool bNewVal)
   {
      m_bEnableSignature = bNewVal;
   }

   Domain::DomainSignatureMethod 
   Domain::GetSignatureMethod() const
   {
      return m_eSignatureMethod;
   }

   void 
   Domain::SetSignatureMethod(Domain::DomainSignatureMethod eSM)
   {
      m_eSignatureMethod = eSM;
   }

   String 
   Domain::GetSignaturePlainText() const
   {
      return m_sSignaturePlainText;
   }

   void 
   Domain::SetSignaturePlainText(const String &sSignature)
   {
      m_sSignaturePlainText = sSignature;
   }

   String 
   Domain::GetSignatureHTML() const
   {
      return m_sSignatureHTML;
   }

   void 
   Domain::SetSignatureHTML(const String &sSignature)
   {
      m_sSignatureHTML = sSignature;
   }


   bool
   Domain::GetAddSignaturesToReplies() const
   {
      return m_bAddSignaturesToReplies;
   }

   void 
   Domain::SetAddSignaturesToReplies(bool bNewVal)
   {
      m_bAddSignaturesToReplies = bNewVal;
   }

   bool
   Domain::GetAddSignaturesToLocalMail() const
   {
      return m_bAddSignaturesToLocalMail;
   }

   void 
   Domain::SetAddSignaturesToLocalMail(bool bNewVal)
   {
      m_bAddSignaturesToLocalMail = bNewVal;
   }

   int
   Domain::GetMaxNoOfAccounts() const
   {
      return m_iMaxNoOfAccounts;
   }

   void 
   Domain::SetMaxNoOfAccounts(int iNewVal)
   {
      m_iMaxNoOfAccounts = iNewVal;
   }

   int
   Domain::GetMaxNoOfAliases() const
   {
      return m_iMaxNoOfAliases;
   }

   void 
   Domain::SetMaxNoOfAliases(int iNewVal)
   {
      m_iMaxNoOfAliases = iNewVal;
   }

   int
   Domain::GetMaxNoOfDistributionLists() const
   {
      return m_iMaxNoOfDistributionLists;
   }

   void 
   Domain::SetMaxNoOfDistributionLists(int iNewVal)
   {
      m_iMaxNoOfDistributionLists = iNewVal;
   }

   int
   Domain::GetLimitationsEnabled() const
   {
      return m_iLimitationsEnabled;
   }

   void
   Domain::SetLimitationsEnabled(int iNewVal)
   {
      m_iLimitationsEnabled = iNewVal;
   }

   bool
   Domain::GetMaxNoOfAccountsEnabled() const
   {
      return (m_iLimitationsEnabled & MaxNoOfAccounts) ? true : false;
   }

   void
   Domain::SetMaxNoOfAccountsEnabled(bool bNewVal)
   {
      if (bNewVal)
         m_iLimitationsEnabled = m_iLimitationsEnabled | MaxNoOfAccounts;
      else
         m_iLimitationsEnabled = m_iLimitationsEnabled & ~MaxNoOfAccounts;
   }

   bool
   Domain::GetMaxNoOfAliasesEnabled() const
   {
      return (m_iLimitationsEnabled & MaxNoOfAliases) ? true : false;
   }

   void 
   Domain::SetMaxNoOfAliasesEnabled(bool bNewVal)
   {
      if (bNewVal)
         m_iLimitationsEnabled = m_iLimitationsEnabled | MaxNoOfAliases;
      else
         m_iLimitationsEnabled = m_iLimitationsEnabled & ~MaxNoOfAliases;

   }

   bool
   Domain::GetMaxNoOfDistributionListsEnabled() const
   {
      return (m_iLimitationsEnabled & MaxNoOfDistributionLists) ? true : false;
   }

   void 
   Domain::SetMaxNoOfDistributionListsEnabled(bool bNewVal)
   {
      if (bNewVal)
         m_iLimitationsEnabled = m_iLimitationsEnabled | MaxNoOfDistributionLists;
      else
         m_iLimitationsEnabled = m_iLimitationsEnabled & ~MaxNoOfDistributionLists;
   }

   int
   Domain::GetMaxAccountSize() const
   {
      return m_iMaxAccountSize;
   }

   void 
   Domain::SetMaxAccountSize(int iNewVal)
   {
      m_iMaxAccountSize = iNewVal;
   }

   bool
   Domain::GetDKIMEnabled() const
   {
      return (m_iAntiSpamOptions & ASDKIMSign) ? true : false;
   }

   void
   Domain::SetDKIMEnabled(bool newValue)
   {
      m_iAntiSpamOptions = newValue ? m_iAntiSpamOptions | ASDKIMSign : m_iAntiSpamOptions & ~ASDKIMSign;
   }

   AnsiString 
   Domain::GetDKIMSelector() const
   {
      return _dkimSelector;
   }

   void 
   Domain::SetDKIMSelector(const String &newValue)
   {
      _dkimSelector = newValue;
   }

   String 
   Domain::GetDKIMPrivateKeyFile() const
   {
      return _dkimPrivateKeyFile;
   }

   void 
   Domain::SetDKIMPrivateKeyFile(const String &newValue)
   {
      _dkimPrivateKeyFile = newValue;
   }

   int 
   Domain::GetDKIMHeaderCanonicalizationMethod() const
   {
      if (m_iAntiSpamOptions & ASDKIMSimpleHeader)
         return 1;
      else
         return 2;
   }

   void 
   Domain::SetDKIMHeaderCanonicalizationMethod(int newValue)
   {
      if (newValue == 1)
         m_iAntiSpamOptions |= ASDKIMSimpleHeader;
      else
         m_iAntiSpamOptions &= ~ASDKIMSimpleHeader;
   }


   int 
   Domain::GetDKIMBodyCanonicalizationMethod() const
   {
      if (m_iAntiSpamOptions & ASDKIMSimpleBody)
         return 1;
      else
         return 2;
   }

   void 
   Domain::SetDKIMBodyCanonicalizationMethod(int newValue)
   {
      if (newValue == 1)
         m_iAntiSpamOptions |= ASDKIMSimpleBody;
      else
         m_iAntiSpamOptions &=  ~ASDKIMSimpleBody;
   }

   int 
   Domain::GetDKIMSigningAlgorithm() const
   {
      if (m_iAntiSpamOptions & ASDKIMSHA1)
         return 1;
      else
         return 2;
   }

   void 
   Domain::SetDKIMSigningAlgorithm(int newValue)
   {
      if (newValue == 1)
         m_iAntiSpamOptions |=  ASDKIMSHA1;
      else
         m_iAntiSpamOptions &= ~ASDKIMSHA1;
   }
}