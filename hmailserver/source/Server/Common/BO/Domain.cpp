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
      use_plus_addressing_(false),
      active_(0),
      max_message_size_(0),
      plus_addressing_char_("+"),
      anti_spam_options_(1),
      max_size_mb_(0),
      enable_signature_(false),
      signature_method_(SMSetIfNotSpecifiedInAccount),
      add_signatures_to_local_mail_(true),
      add_signatures_to_replies_(false),
      max_no_of_accounts_(0),
      max_no_of_aliases_(0),
      max_no_of_distribution_lists_(0),
      max_account_size_(0),
      limitations_enabled_(0)
   {
      
   }

   Domain::~Domain()
   {

   }

   std::shared_ptr<Accounts>
   Domain::GetAccounts()
   {
      if (!accounts_)
         accounts_ = std::shared_ptr<Accounts>(new Accounts(id_));

      accounts_->Refresh();
      return accounts_;
   }


   std::shared_ptr<DomainAliases>
   Domain::GetDomainAliases()
   {
      if (!domain_aliases_)
         domain_aliases_ = std::shared_ptr<DomainAliases>(new DomainAliases(id_));

      domain_aliases_->Refresh();
      return domain_aliases_;
   }

   std::shared_ptr<Accounts>
   Domain::GetAccounts(__int64 iAccountID)
   {
      if (!accounts_)
      {
         // Only fetch a specific account and put it in the list. This happens
         // when an API client only has limited access.
         accounts_ = std::shared_ptr<Accounts>(new Accounts(id_, iAccountID));
      }

      accounts_->Refresh();
      return accounts_;
   }

   std::shared_ptr<Aliases>
   Domain::GetAliases()
   {
      if (!aliases_)
         aliases_ = std::shared_ptr<Aliases>(new Aliases(id_));
      
      aliases_->Refresh();

      return aliases_;
   }

   std::shared_ptr<DistributionLists>
   Domain::GetDistributionLists()
   {
      if (!distribution_lists_)
         distribution_lists_ = std::shared_ptr<DistributionLists>(new DistributionLists(id_));

      distribution_lists_->Refresh();

      return distribution_lists_;
   }

   bool 
   Domain::XMLStore(XNode *pParentNode, int iBackupOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Domain"));

      pNode->AppendAttr(_T("Name"), name_);
      pNode->AppendAttr(_T("Postmaster"), postmaster_);
      pNode->AppendAttr(_T("ADDomainName"), addomain_name_);
      pNode->AppendAttr(_T("Active"), StringParser::IntToString(active_));
      pNode->AppendAttr(_T("MaxMessageSize"), StringParser::IntToString(max_message_size_));
      pNode->AppendAttr(_T("MaxSize"), StringParser::IntToString(max_size_mb_));
      pNode->AppendAttr(_T("MaxAccountSize"), StringParser::IntToString(max_account_size_));

      pNode->AppendAttr(_T("UsePlusAddressing"), use_plus_addressing_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("PlusAddressingChar"), plus_addressing_char_);
      pNode->AppendAttr(_T("AntiSpamOptions"), StringParser::IntToString(anti_spam_options_));

      pNode->AppendAttr(_T("EnableSignature"), enable_signature_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("SignatureMethod"), StringParser::IntToString(signature_method_));
      pNode->AppendAttr(_T("SignaturePlainText"), signature_plain_text_);
      pNode->AppendAttr(_T("SignatureHTML"), signature_html_);
      pNode->AppendAttr(_T("AddSignaturesToLocalMail"), add_signatures_to_local_mail_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("AddSignaturesToReplies"), add_signatures_to_replies_ ? _T("1") : _T("0"));

      pNode->AppendAttr(_T("MaxNoOfAccounts"), StringParser::IntToString(max_no_of_accounts_));
      pNode->AppendAttr(_T("MaxNoOfAliases"), StringParser::IntToString(max_no_of_aliases_));
      pNode->AppendAttr(_T("MaxNoOfLists"), StringParser::IntToString(max_no_of_distribution_lists_));
      pNode->AppendAttr(_T("LimitationsEnabled"), StringParser::IntToString(limitations_enabled_));

      pNode->AppendAttr(_T("DKIMSelector"), dkim_selector_);
      pNode->AppendAttr(_T("DKIMPrivateKeyFile"), dkim_private_key_file_);

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
      name_ = pNode->GetAttrValue(_T("Name"));
      postmaster_ = pNode->GetAttrValue(_T("Postmaster"));
      addomain_name_ = pNode->GetAttrValue(_T("ADDomainName"));
      active_ = _ttoi(pNode->GetAttrValue(_T("Active")));
      max_message_size_ = _ttoi(pNode->GetAttrValue(_T("MaxMessageSize")));
      max_size_mb_ = _ttoi(pNode->GetAttrValue(_T("MaxSize")));
      max_account_size_ = _ttoi(pNode->GetAttrValue(_T("MaxAccountSize")));

      use_plus_addressing_ = pNode->GetAttrValue(_T("UsePlusAddressing")) == _T("1");
      plus_addressing_char_ = pNode->GetAttrValue(_T("PlusAddressingChar"));
      anti_spam_options_ = _ttoi(pNode->GetAttrValue(_T("AntiSpamOptions")));

      enable_signature_ = pNode->GetAttrValue(_T("EnableSignature")) == _T("1");
      signature_method_ = (DomainSignatureMethod) _ttoi(pNode->GetAttrValue(_T("SignatureMethod")));
      signature_plain_text_ = pNode->GetAttrValue(_T("SignaturePlainText"));
      signature_html_ = pNode->GetAttrValue(_T("SignatureHTML"));

      add_signatures_to_local_mail_ = pNode->GetAttrValue(_T("AddSignaturesToLocalMail")) == _T("1");
      add_signatures_to_replies_ = pNode->GetAttrValue(_T("AddSignaturesToReplies")) == _T("1");

      max_no_of_accounts_ = _ttoi(pNode->GetAttrValue(_T("MaxNoOfAccounts")));
      max_no_of_aliases_ = _ttoi(pNode->GetAttrValue(_T("MaxNoOfAliases")));
      max_no_of_distribution_lists_ = _ttoi(pNode->GetAttrValue(_T("MaxNoOfLists")));
      
      limitations_enabled_ = _ttoi(pNode->GetAttrValue(_T("LimitationsEnabled")));

      dkim_selector_ = pNode->GetAttrValue(_T("DKIMSelector"));
      dkim_private_key_file_ = pNode->GetAttrValue(_T("DKIMPrivateKeyFile"));

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
      return (anti_spam_options_ & ASUseGreylisting) ? true : false;
   }

   void 
   Domain::SetASUseGreyListing(bool bNewVal)
   {
      if (bNewVal)
         anti_spam_options_ = anti_spam_options_ | ASUseGreylisting;
      else
         anti_spam_options_ = anti_spam_options_ & ~ASUseGreylisting;      

   }

   int
   Domain::GetMaxSizeMB() const
   {
      return max_size_mb_;
   }

   void 
   Domain::SetMaxSizeMB(int iNewVal)
   {
      max_size_mb_ = iNewVal;
   }

   bool  
   Domain::GetEnableSignature() const
   {
      return enable_signature_;
   }

   void 
   Domain::SetEnableSignature(bool bNewVal)
   {
      enable_signature_ = bNewVal;
   }

   Domain::DomainSignatureMethod 
   Domain::GetSignatureMethod() const
   {
      return signature_method_;
   }

   void 
   Domain::SetSignatureMethod(Domain::DomainSignatureMethod eSM)
   {
      signature_method_ = eSM;
   }

   String 
   Domain::GetSignaturePlainText() const
   {
      return signature_plain_text_;
   }

   void 
   Domain::SetSignaturePlainText(const String &sSignature)
   {
      signature_plain_text_ = sSignature;
   }

   String 
   Domain::GetSignatureHTML() const
   {
      return signature_html_;
   }

   void 
   Domain::SetSignatureHTML(const String &sSignature)
   {
      signature_html_ = sSignature;
   }


   bool
   Domain::GetAddSignaturesToReplies() const
   {
      return add_signatures_to_replies_;
   }

   void 
   Domain::SetAddSignaturesToReplies(bool bNewVal)
   {
      add_signatures_to_replies_ = bNewVal;
   }

   bool
   Domain::GetAddSignaturesToLocalMail() const
   {
      return add_signatures_to_local_mail_;
   }

   void 
   Domain::SetAddSignaturesToLocalMail(bool bNewVal)
   {
      add_signatures_to_local_mail_ = bNewVal;
   }

   int
   Domain::GetMaxNoOfAccounts() const
   {
      return max_no_of_accounts_;
   }

   void 
   Domain::SetMaxNoOfAccounts(int iNewVal)
   {
      max_no_of_accounts_ = iNewVal;
   }

   int
   Domain::GetMaxNoOfAliases() const
   {
      return max_no_of_aliases_;
   }

   void 
   Domain::SetMaxNoOfAliases(int iNewVal)
   {
      max_no_of_aliases_ = iNewVal;
   }

   int
   Domain::GetMaxNoOfDistributionLists() const
   {
      return max_no_of_distribution_lists_;
   }

   void 
   Domain::SetMaxNoOfDistributionLists(int iNewVal)
   {
      max_no_of_distribution_lists_ = iNewVal;
   }

   int
   Domain::GetLimitationsEnabled() const
   {
      return limitations_enabled_;
   }

   void
   Domain::SetLimitationsEnabled(int iNewVal)
   {
      limitations_enabled_ = iNewVal;
   }

   bool
   Domain::GetMaxNoOfAccountsEnabled() const
   {
      return (limitations_enabled_ & MaxNoOfAccounts) ? true : false;
   }

   void
   Domain::SetMaxNoOfAccountsEnabled(bool bNewVal)
   {
      if (bNewVal)
         limitations_enabled_ = limitations_enabled_ | MaxNoOfAccounts;
      else
         limitations_enabled_ = limitations_enabled_ & ~MaxNoOfAccounts;
   }

   bool
   Domain::GetMaxNoOfAliasesEnabled() const
   {
      return (limitations_enabled_ & MaxNoOfAliases) ? true : false;
   }

   void 
   Domain::SetMaxNoOfAliasesEnabled(bool bNewVal)
   {
      if (bNewVal)
         limitations_enabled_ = limitations_enabled_ | MaxNoOfAliases;
      else
         limitations_enabled_ = limitations_enabled_ & ~MaxNoOfAliases;

   }

   bool
   Domain::GetMaxNoOfDistributionListsEnabled() const
   {
      return (limitations_enabled_ & MaxNoOfDistributionLists) ? true : false;
   }

   void 
   Domain::SetMaxNoOfDistributionListsEnabled(bool bNewVal)
   {
      if (bNewVal)
         limitations_enabled_ = limitations_enabled_ | MaxNoOfDistributionLists;
      else
         limitations_enabled_ = limitations_enabled_ & ~MaxNoOfDistributionLists;
   }

   int
   Domain::GetMaxAccountSize() const
   {
      return max_account_size_;
   }

   void 
   Domain::SetMaxAccountSize(int iNewVal)
   {
      max_account_size_ = iNewVal;
   }

   bool
   Domain::GetDKIMEnabled() const
   {
      return (anti_spam_options_ & ASDKIMSign) ? true : false;
   }

   void
   Domain::SetDKIMEnabled(bool newValue)
   {
      anti_spam_options_ = newValue ? anti_spam_options_ | ASDKIMSign : anti_spam_options_ & ~ASDKIMSign;
   }

   bool
      Domain::GetDKIMAliasesEnabled() const
   {
      return (anti_spam_options_ & ASDKIMSignAliases) ? true : false;
   }

   void
      Domain::SetDKIMAliasesEnabled(bool newValue)
   {
      anti_spam_options_ = newValue ? anti_spam_options_ | ASDKIMSignAliases : anti_spam_options_ & ~ASDKIMSignAliases;
   }

   AnsiString
   Domain::GetDKIMSelector() const
   {
      return dkim_selector_;
   }

   void 
   Domain::SetDKIMSelector(const String &newValue)
   {
      dkim_selector_ = newValue;
   }

   String 
   Domain::GetDKIMPrivateKeyFile() const
   {
      return dkim_private_key_file_;
   }

   void 
   Domain::SetDKIMPrivateKeyFile(const String &newValue)
   {
      dkim_private_key_file_ = newValue;
   }

   int 
   Domain::GetDKIMHeaderCanonicalizationMethod() const
   {
      if (anti_spam_options_ & ASDKIMSimpleHeader)
         return 1;
      else
         return 2;
   }

   void 
   Domain::SetDKIMHeaderCanonicalizationMethod(int newValue)
   {
      if (newValue == 1)
         anti_spam_options_ |= ASDKIMSimpleHeader;
      else
         anti_spam_options_ &= ~ASDKIMSimpleHeader;
   }


   int 
   Domain::GetDKIMBodyCanonicalizationMethod() const
   {
      if (anti_spam_options_ & ASDKIMSimpleBody)
         return 1;
      else
         return 2;
   }

   void 
   Domain::SetDKIMBodyCanonicalizationMethod(int newValue)
   {
      if (newValue == 1)
         anti_spam_options_ |= ASDKIMSimpleBody;
      else
         anti_spam_options_ &=  ~ASDKIMSimpleBody;
   }

   int 
   Domain::GetDKIMSigningAlgorithm() const
   {
      if (anti_spam_options_ & ASDKIMSHA1)
         return 1;
      else
         return 2;
   }

   void 
   Domain::SetDKIMSigningAlgorithm(int newValue)
   {
      if (newValue == 1)
         anti_spam_options_ |=  ASDKIMSHA1;
      else
         anti_spam_options_ &= ~ASDKIMSHA1;
   }

   size_t 
   Domain::GetEstimatedCachingSize()
   {
      return 1024;
   }
}