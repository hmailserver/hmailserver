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
      m_iDomainID(0),
      m_bForwardEnabled(false),
      m_bForwardKeepOriginal(false),
      m_bActive(false),
      m_bIsAD(false),
      m_iAccountMaxSize(0),
      m_bVacationMessageIsOn(false),
      m_iPasswordEncryption(0),
      m_iAdminLevel(NormalUser),
      m_bEnableSignature(false),
      m_bVacationExpires(false)
   {
      Initialize();
   }

   Account::Account(const String &address, AdminLevel adminLevel) :
      m_sAddress(address),
      m_iDomainID(0),
      m_bForwardEnabled(false),
      m_bForwardKeepOriginal(false),
      m_bActive(false),
      m_bIsAD(false),
      m_iAccountMaxSize(0),
      m_bVacationMessageIsOn(false),
      m_iPasswordEncryption(0),
      m_iAdminLevel(adminLevel),
      m_bEnableSignature(false),
      m_bVacationExpires(false)
   {
      Initialize();
   }

   void
   Account::Initialize()
   {
      m_sLastLogonTime = Time::GetCurrentDateTime();
      m_sVacationExpiresDate = Time::GetCurrentDate();
   }


   Account::~Account()
   {
   
   }

   Account::Account(const Account &oldAccount)
   {
      m_iID = oldAccount.m_iID;
      m_iDomainID = oldAccount.m_iDomainID;
      m_iAccountMaxSize = oldAccount.m_iAccountMaxSize;
      m_iPasswordEncryption = oldAccount.m_iPasswordEncryption;
      m_sAddress = oldAccount.m_sAddress;
      m_sPassword = oldAccount.m_sPassword;
      m_sADDomain = oldAccount.m_sADDomain;
      m_sADUsername = oldAccount.m_sADUsername;
      m_sPersonFirstName = oldAccount.m_sPersonFirstName;
      m_sPersonLastName = oldAccount.m_sPersonLastName;
      m_sVacationMessage = oldAccount.m_sVacationMessage;
      m_sVacationSubject = oldAccount.m_sVacationSubject;
      m_bVacationExpires = oldAccount.m_bVacationExpires;
      m_sVacationExpiresDate = oldAccount.m_sVacationExpiresDate;
      m_sSignaturePlainText = oldAccount.m_sSignaturePlainText;
      m_sSignatureHTML = oldAccount.m_sSignatureHTML;
      m_sForwardAddress = oldAccount.m_sForwardAddress;
      m_bForwardEnabled = oldAccount.m_bForwardEnabled;
      m_bForwardKeepOriginal = oldAccount.m_bForwardKeepOriginal;
      m_bActive = oldAccount.m_bActive;
      m_bIsAD = oldAccount.m_bIsAD;
      m_bVacationMessageIsOn = oldAccount.m_bVacationMessageIsOn;
      m_bEnableSignature = oldAccount.m_bEnableSignature;
      m_iAdminLevel = oldAccount.m_iAdminLevel;
      m_sLastLogonTime = oldAccount.m_sLastLogonTime;
   }

   shared_ptr<Messages>
   Account::GetMessages()
   {
      if (m_oMessages.get() == NULL)
      {
         m_oMessages = shared_ptr<Messages>(new Messages(m_iID, -1));
         m_oMessages->Refresh();
      }

      return m_oMessages;
   }

   shared_ptr<Rules>
   Account::GetRules()
   {
      if (m_pRules.get() == NULL)
      {
         m_pRules = shared_ptr<Rules>(new Rules(m_iID));
         m_pRules->Refresh();
      }
   
      return m_pRules;
   }

   shared_ptr<IMAPFolders>
   Account::GetFolders()
   {
      if (m_pFolders.get() == NULL)
      {
         m_pFolders = shared_ptr<IMAPFolders>(new HM::IMAPFolders(m_iID, -1));
         m_pFolders->Refresh();

      }

      return m_pFolders;
   }


   void 
   Account::SetPassword(const String & newVal)
   {
      m_sPassword = newVal; 
   }

   void
   Account::SetVacationMessageIsOn(bool bNewVal)
   {
      if (!bNewVal && m_bVacationMessageIsOn)
      {
         // The user has just turned of the vacation
         // message. Notify the SMTP deliverer of this.

         if (!m_sAddress.IsEmpty())
            SMTPVacationMessageCreator::Instance()->VacationMessageTurnedOff(m_sAddress);
      }

      m_bVacationMessageIsOn = bNewVal;
   }

   bool
   Account::SpaceAvailable(__int64 iBytes) const
   {
      if (m_iAccountMaxSize == 0)
         return true;

      __int64 currentSize = AccountSizeCache::Instance()->GetSize(m_iID);

      // Calculate new size
      __int64 iNewSize = currentSize + iBytes;
      __int64 iMaxSize = ((__int64) m_iAccountMaxSize) * 1024 * 1024;

      if (iNewSize <= iMaxSize)
         return true;
      else
         return false;
   }

   bool 
   Account::GetForwardEnabled() const
   {
      return m_bForwardEnabled;
   }

   void 
   Account::SetForwardEnabled(bool bEnabled)
   {
      m_bForwardEnabled = bEnabled;
   }

   bool 
   Account::GetForwardKeepOriginal() const
   {
      return m_bForwardKeepOriginal;
   }

   void 
   Account::SetForwardKeepOriginal(bool bEnabled)
   {
      m_bForwardKeepOriginal = bEnabled;
   }

   bool 
   Account::GetEnableSignature() const
   {
      return m_bEnableSignature;
   }

   void 
   Account::SetEnableSignature(bool bEnabled)
   {
      m_bEnableSignature = bEnabled;
   }

   String 
   Account::GetSignaturePlainText() const
   {
      return m_sSignaturePlainText;
   }

   void 
   Account::SetSignaturePlainText(const String &sSignature)
   {
      m_sSignaturePlainText = sSignature;
   }

   String 
   Account::GetSignatureHTML() const
   {
      return m_sSignatureHTML;
   }

   void 
   Account::SetSignatureHTML(const String &sSignature)
   {
      m_sSignatureHTML = sSignature;
   }

   String 
   Account::GetLastLogonTime() const
   {
      return m_sLastLogonTime;
   }

   void 
   Account::SetLastLogonTime(const String &sNewVal)
   {
      m_sLastLogonTime = sNewVal;
   }

   String 
   Account::GetPersonFirstName() const
   {
      return m_sPersonFirstName;
   }

   void 
   Account::SetPersonFirstName(const String &sNewVal)
   {
      m_sPersonFirstName = sNewVal;
   }

   String 
   Account::GetPersonLastName() const
   {
      return m_sPersonLastName;
   }

   void 
   Account::SetPersonLastName(const String &sNewVal)
   {
      m_sPersonLastName = sNewVal;
   }

   bool 
   Account::XMLStore(XNode *pParentNode, int iBackupOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Account"));

      Logger::Instance()->LogBackup("Backing up account " + m_sAddress + "...");

      pNode->AppendAttr(_T("Name"), String(m_sAddress));
      pNode->AppendAttr(_T("PersonFirstName"), m_sPersonFirstName);
      pNode->AppendAttr(_T("PersonLastName"), m_sPersonLastName);
      pNode->AppendAttr(_T("Active"), m_bActive ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("Password"), String(m_sPassword));
      pNode->AppendAttr(_T("PasswordEncryption"), StringParser::IntToString(m_iPasswordEncryption));
      pNode->AppendAttr(_T("MaxAccountSize"), StringParser::IntToString(m_iAccountMaxSize));
      pNode->AppendAttr(_T("ADUsername"), m_sADUsername);
      pNode->AppendAttr(_T("ADDomain"), m_sADDomain);
      pNode->AppendAttr(_T("ADActive"), m_bIsAD ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("VacationMessageOn"), m_bVacationMessageIsOn ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("VacationMessage"), m_sVacationMessage);
      pNode->AppendAttr(_T("VacationSubject"), m_sVacationSubject);
      pNode->AppendAttr(_T("VacationExpires"), m_bVacationExpires ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("VacationExpireDate"), m_sVacationExpiresDate);
      pNode->AppendAttr(_T("AdminLevel"), StringParser::IntToString(m_iAdminLevel));
      
      pNode->AppendAttr(_T("ForwardEnabled"), m_bForwardEnabled ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("ForwardAddress"), String(m_sForwardAddress));
      pNode->AppendAttr(_T("ForwardKeepOriginal"), m_bForwardKeepOriginal ? _T("1") : _T("0"));

      pNode->AppendAttr(_T("EnableSignature"), m_bEnableSignature ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("SignaturePlainText"), m_sSignaturePlainText);
      pNode->AppendAttr(_T("SignatureHTML"), m_sSignatureHTML);

      pNode->AppendAttr(_T("LastLogonTime"), String(m_sLastLogonTime));

      // Store fetch accounts
      shared_ptr<HM::FetchAccounts> pFetchAccounts = shared_ptr<HM::FetchAccounts>(new HM::FetchAccounts(m_iID));
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
      m_sAddress = pAccountNode->GetAttrValue(_T("Name"));

      Logger::Instance()->LogBackup("Restoring account " + m_sAddress + "...");

      m_sPersonFirstName = pAccountNode->GetAttrValue(_T("PersonFirstName"));;
      m_sPersonLastName = pAccountNode->GetAttrValue(_T("PersonLastName"));;
      m_bActive = (pAccountNode->GetAttrValue(_T("Active")) == _T("1"));
      m_sPassword = pAccountNode->GetAttrValue(_T("Password"));
      m_iPasswordEncryption = _ttoi(pAccountNode->GetAttrValue(_T("PasswordEncryption")));
      m_iAccountMaxSize = _ttoi(pAccountNode->GetAttrValue(_T("MaxAccountSize")));
      m_sADUsername = pAccountNode->GetAttrValue(_T("ADUsername"));
      m_sADDomain = pAccountNode->GetAttrValue(_T("ADDomain"));
      m_bIsAD = pAccountNode->GetAttrValue(_T("ADActive")) == _T("1");
      m_bVacationMessageIsOn = pAccountNode->GetAttrValue(_T("VacationMessageOn")) == _T("1");
      m_sVacationMessage = pAccountNode->GetAttrValue(_T("VacationMessage"));
      m_sVacationSubject = pAccountNode->GetAttrValue(_T("VacationSubject"));
      m_bVacationExpires = (pAccountNode->GetAttrValue(_T("VacationExpires")) == _T("1"));
      m_sVacationExpiresDate = pAccountNode->GetAttrValue(_T("VacationExpireDate"));

      m_iAdminLevel = (AdminLevel) _ttoi(pAccountNode->GetAttrValue(_T("AdminLevel")));
     
      m_sForwardAddress = pAccountNode->GetAttrValue(_T("ForwardAddress"));

      m_bForwardEnabled = (pAccountNode->GetAttrValue(_T("ForwardEnabled")) == _T("1"));
      m_sForwardAddress = pAccountNode->GetAttrValue(_T("ForwardAddress"));
      m_bForwardKeepOriginal = (pAccountNode->GetAttrValue(_T("ForwardKeepOriginal")) == _T("1"));

      m_sSignaturePlainText = pAccountNode->GetAttrValue(_T("SignaturePlainText"));
      m_sSignatureHTML = pAccountNode->GetAttrValue(_T("SignatureHTML"));
      m_bEnableSignature = (pAccountNode->GetAttrValue(_T("EnableSignature")) == _T("1"));

      m_bEnableSignature = pAccountNode->GetAttrValue(_T("EnableSignature")) == _T("1");
      m_sSignaturePlainText = pAccountNode->GetAttrValue(_T("SignaturePlainText"));
      m_sSignatureHTML = pAccountNode->GetAttrValue(_T("SignatureHTML"));

      m_sLastLogonTime = pAccountNode->GetAttrValue(_T("LastLogonTime"));

      return true;
   }

   bool
   Account::XMLLoadSubItems(XNode *pAccountNode, int iRestoreOptions)
   {
      // Load 
      shared_ptr<HM::FetchAccounts> pFetchAccounts = shared_ptr<HM::FetchAccounts>(new HM::FetchAccounts(m_iID));
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
      return m_bVacationMessageIsOn;

      
   }

}
