// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Account.h"
#include "Rules.h"
#include "Messages.h"
#include "IMAPFolders.h"
#include "../Util/Crypt.h"
#include "../Util/Time.h"

#include "FetchAccounts.h"

#include "../../SMTP/SMTPVacationMessageCreator.h"
#include "../Persistence/PersistentAccount.h"
#include "../Cache/AccountSizeCache.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Account::Account() :
      domain_id_(0),
      forward_enabled_(false),
      forward_keep_original_(false),
      active_(false),
      is_ad_(false),
      account_max_size_(0),
      vacation_message_is_on_(false),
      password_encryption_(0),
      admin_level_(NormalUser),
      enable_signature_(false),
      vacation_expires_(false)
   {
      Initialize();
   }

   Account::Account(const String &address, AdminLevel adminLevel) :
      address_(address),
      domain_id_(0),
      forward_enabled_(false),
      forward_keep_original_(false),
      active_(false),
      is_ad_(false),
      account_max_size_(0),
      vacation_message_is_on_(false),
      password_encryption_(0),
      admin_level_(adminLevel),
      enable_signature_(false),
      vacation_expires_(false)
   {
      Initialize();
   }

   void
   Account::Initialize()
   {
      last_logon_time_ = Time::GetCurrentDateTime();
      vacation_expires_date_ = Time::GetCurrentDate();
   }


   Account::~Account()
   {
   
   }

   Account::Account(const Account &oldAccount)
   {
      id_ = oldAccount.id_;
      domain_id_ = oldAccount.domain_id_;
      account_max_size_ = oldAccount.account_max_size_;
      password_encryption_ = oldAccount.password_encryption_;
      address_ = oldAccount.address_;
      password_ = oldAccount.password_;
      addomain_ = oldAccount.addomain_;
      adusername_ = oldAccount.adusername_;
      person_first_name_ = oldAccount.person_first_name_;
      person_last_name_ = oldAccount.person_last_name_;
      vacation_message_ = oldAccount.vacation_message_;
      vacation_subject_ = oldAccount.vacation_subject_;
      vacation_expires_ = oldAccount.vacation_expires_;
      vacation_expires_date_ = oldAccount.vacation_expires_date_;
      signature_plain_text_ = oldAccount.signature_plain_text_;
      signature_html_ = oldAccount.signature_html_;
      forward_address_ = oldAccount.forward_address_;
      forward_enabled_ = oldAccount.forward_enabled_;
      forward_keep_original_ = oldAccount.forward_keep_original_;
      active_ = oldAccount.active_;
      is_ad_ = oldAccount.is_ad_;
      vacation_message_is_on_ = oldAccount.vacation_message_is_on_;
      enable_signature_ = oldAccount.enable_signature_;
      admin_level_ = oldAccount.admin_level_;
      last_logon_time_ = oldAccount.last_logon_time_;
   }

   std::shared_ptr<Messages>
   Account::GetMessages()
   {
      if (messages_.get() == NULL)
      {
         messages_ = std::shared_ptr<Messages>(new Messages(id_, -1));
         messages_->Refresh(false);
      }

      return messages_;
   }

   std::shared_ptr<Rules>
   Account::GetRules()
   {
      if (rules_.get() == NULL)
      {
         rules_ = std::shared_ptr<Rules>(new Rules(id_));
         rules_->Refresh();
      }
   
      return rules_;
   }

   std::shared_ptr<IMAPFolders>
   Account::GetFolders()
   {
      if (folders_.get() == NULL)
      {
         folders_ = std::shared_ptr<IMAPFolders>(new HM::IMAPFolders(id_, -1));
         folders_->Refresh();

      }

      return folders_;
   }


   void 
   Account::SetPassword(const String & newVal)
   {
      password_ = newVal; 
   }

   void
   Account::SetVacationMessageIsOn(bool bNewVal)
   {
      if (!bNewVal && vacation_message_is_on_)
      {
         // The user has just turned of the vacation
         // message. Notify the SMTP deliverer of this.

         if (!address_.IsEmpty())
            SMTPVacationMessageCreator::Instance()->VacationMessageTurnedOff(address_);
      }

      vacation_message_is_on_ = bNewVal;
   }

   bool
   Account::SpaceAvailable(__int64 iBytes) const
   {
      if (account_max_size_ == 0)
         return true;

      __int64 currentSize = AccountSizeCache::Instance()->GetSize(id_);

      // Calculate new size
      __int64 iNewSize = currentSize + iBytes;
      __int64 iMaxSize = ((__int64) account_max_size_) * 1024 * 1024;

      if (iNewSize <= iMaxSize)
         return true;
      else
         return false;
   }

   bool 
   Account::GetForwardEnabled() const
   {
      return forward_enabled_;
   }

   void 
   Account::SetForwardEnabled(bool bEnabled)
   {
      forward_enabled_ = bEnabled;
   }

   bool 
   Account::GetForwardKeepOriginal() const
   {
      return forward_keep_original_;
   }

   void 
   Account::SetForwardKeepOriginal(bool bEnabled)
   {
      forward_keep_original_ = bEnabled;
   }

   bool 
   Account::GetEnableSignature() const
   {
      return enable_signature_;
   }

   void 
   Account::SetEnableSignature(bool bEnabled)
   {
      enable_signature_ = bEnabled;
   }

   String 
   Account::GetSignaturePlainText() const
   {
      return signature_plain_text_;
   }

   void 
   Account::SetSignaturePlainText(const String &sSignature)
   {
      signature_plain_text_ = sSignature;
   }

   String 
   Account::GetSignatureHTML() const
   {
      return signature_html_;
   }

   void 
   Account::SetSignatureHTML(const String &sSignature)
   {
      signature_html_ = sSignature;
   }

   String 
   Account::GetLastLogonTime() const
   {
      return last_logon_time_;
   }

   void 
   Account::SetLastLogonTime(const String &sNewVal)
   {
      last_logon_time_ = sNewVal;
   }

   String 
   Account::GetPersonFirstName() const
   {
      return person_first_name_;
   }

   void 
   Account::SetPersonFirstName(const String &sNewVal)
   {
      person_first_name_ = sNewVal;
   }

   String 
   Account::GetPersonLastName() const
   {
      return person_last_name_;
   }

   void 
   Account::SetPersonLastName(const String &sNewVal)
   {
      person_last_name_ = sNewVal;
   }

   bool 
   Account::XMLStore(XNode *pParentNode, int iBackupOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Account"));

      Logger::Instance()->LogBackup("Backing up account " + address_ + "...");

      pNode->AppendAttr(_T("Name"), String(address_));
      pNode->AppendAttr(_T("PersonFirstName"), person_first_name_);
      pNode->AppendAttr(_T("PersonLastName"), person_last_name_);
      pNode->AppendAttr(_T("Active"), active_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("Password"), String(password_));
      pNode->AppendAttr(_T("PasswordEncryption"), StringParser::IntToString(password_encryption_));
      pNode->AppendAttr(_T("MaxAccountSize"), StringParser::IntToString(account_max_size_));
      pNode->AppendAttr(_T("ADUsername"), adusername_);
      pNode->AppendAttr(_T("ADDomain"), addomain_);
      pNode->AppendAttr(_T("ADActive"), is_ad_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("VacationMessageOn"), vacation_message_is_on_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("VacationMessage"), vacation_message_);
      pNode->AppendAttr(_T("VacationSubject"), vacation_subject_);
      pNode->AppendAttr(_T("VacationExpires"), vacation_expires_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("VacationExpireDate"), vacation_expires_date_);
      pNode->AppendAttr(_T("AdminLevel"), StringParser::IntToString(admin_level_));
      
      pNode->AppendAttr(_T("ForwardEnabled"), forward_enabled_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("ForwardAddress"), String(forward_address_));
      pNode->AppendAttr(_T("ForwardKeepOriginal"), forward_keep_original_ ? _T("1") : _T("0"));

      pNode->AppendAttr(_T("EnableSignature"), enable_signature_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("SignaturePlainText"), signature_plain_text_);
      pNode->AppendAttr(_T("SignatureHTML"), signature_html_);

      pNode->AppendAttr(_T("LastLogonTime"), String(last_logon_time_));

      // Store fetch accounts
      std::shared_ptr<HM::FetchAccounts> pFetchAccounts = std::shared_ptr<HM::FetchAccounts>(new HM::FetchAccounts(id_));
      pFetchAccounts->Refresh();
      if (!pFetchAccounts->XMLStore(pNode, iBackupOptions))
         return false;

      // Store rules
      if (!GetRules()->XMLStore(pNode,iBackupOptions))
         return false;

      if (iBackupOptions & Backup::BOMessages)
      {
         if (!GetFolders()->XMLStore(pNode,iBackupOptions))
            return false;
      }

      return true;
   }

   bool
   Account::XMLLoad(XNode *pAccountNode, int iRestoreOptions)
   {
      address_ = pAccountNode->GetAttrValue(_T("Name"));

      Logger::Instance()->LogBackup("Restoring account " + address_ + "...");

      person_first_name_ = pAccountNode->GetAttrValue(_T("PersonFirstName"));;
      person_last_name_ = pAccountNode->GetAttrValue(_T("PersonLastName"));;
      active_ = (pAccountNode->GetAttrValue(_T("Active")) == _T("1"));
      password_ = pAccountNode->GetAttrValue(_T("Password"));
      password_encryption_ = _ttoi(pAccountNode->GetAttrValue(_T("PasswordEncryption")));
      account_max_size_ = _ttoi(pAccountNode->GetAttrValue(_T("MaxAccountSize")));
      adusername_ = pAccountNode->GetAttrValue(_T("ADUsername"));
      addomain_ = pAccountNode->GetAttrValue(_T("ADDomain"));
      is_ad_ = pAccountNode->GetAttrValue(_T("ADActive")) == _T("1");
      vacation_message_is_on_ = pAccountNode->GetAttrValue(_T("VacationMessageOn")) == _T("1");
      vacation_message_ = pAccountNode->GetAttrValue(_T("VacationMessage"));
      vacation_subject_ = pAccountNode->GetAttrValue(_T("VacationSubject"));
      vacation_expires_ = (pAccountNode->GetAttrValue(_T("VacationExpires")) == _T("1"));
      vacation_expires_date_ = pAccountNode->GetAttrValue(_T("VacationExpireDate"));

      admin_level_ = (AdminLevel) _ttoi(pAccountNode->GetAttrValue(_T("AdminLevel")));
     
      forward_address_ = pAccountNode->GetAttrValue(_T("ForwardAddress"));

      forward_enabled_ = (pAccountNode->GetAttrValue(_T("ForwardEnabled")) == _T("1"));
      forward_address_ = pAccountNode->GetAttrValue(_T("ForwardAddress"));
      forward_keep_original_ = (pAccountNode->GetAttrValue(_T("ForwardKeepOriginal")) == _T("1"));

      signature_plain_text_ = pAccountNode->GetAttrValue(_T("SignaturePlainText"));
      signature_html_ = pAccountNode->GetAttrValue(_T("SignatureHTML"));
      enable_signature_ = (pAccountNode->GetAttrValue(_T("EnableSignature")) == _T("1"));

      enable_signature_ = pAccountNode->GetAttrValue(_T("EnableSignature")) == _T("1");
      signature_plain_text_ = pAccountNode->GetAttrValue(_T("SignaturePlainText"));
      signature_html_ = pAccountNode->GetAttrValue(_T("SignatureHTML"));

      last_logon_time_ = pAccountNode->GetAttrValue(_T("LastLogonTime"));

      return true;
   }

   bool
   Account::XMLLoadSubItems(XNode *pAccountNode, int iRestoreOptions)
   {
      // Load 
      std::shared_ptr<HM::FetchAccounts> pFetchAccounts = std::shared_ptr<HM::FetchAccounts>(new HM::FetchAccounts(id_));
      pFetchAccounts->Refresh();
      if (!pFetchAccounts->XMLLoad(pAccountNode, iRestoreOptions))
         return false;

      if (!GetRules()->XMLLoad(pAccountNode, iRestoreOptions))
         return false;

      // Should we restore messages as well?
      if (iRestoreOptions & Backup::BOMessages)
      {
         if (!GetFolders()->XMLLoad(pAccountNode, -1))
            return false;
      }
      else
      {
         // Create the inbox...
         PersistentAccount::CreateInbox(*this);
      }

      return true;
   }

   bool 
   Account::GetVacationMessageIsOn() const
   {
      return vacation_message_is_on_;

      
   }

   size_t 
   Account::GetEstimatedCachingSize()
   {
      return 1024;
   }

}
