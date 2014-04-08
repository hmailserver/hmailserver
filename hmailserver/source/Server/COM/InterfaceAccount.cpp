// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
#include "stdafx.h"
#include "InterfaceAccount.h"
#include "InterfaceMessages.h"
#include "InterfaceFetchAccounts.h"
#include "InterfaceRules.h"
#include "InterfaceIMAPFolders.h"

#include "../common/Persistence/PersistentAccount.h"
#include "../common/BO/Accounts.h"
#include "../Common/Util/Math.h"
#include "../Common/Util/PasswordValidator.h"
#include "../Common/Util/Crypt.h"
#include "../Common/Util/Time.h"
#include "../Common/Cache/AccountSizeCache.h"

#include "../POP3/POP3Sessions.h"
#include "../IMAP/IMAPFolderUtilities.h"
#include "../IMAP/IMAPFolderContainer.h"

#include "COMAuthentication.h"

#include "COMError.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

using namespace HM;

#ifdef _DEBUG
   long InterfaceAccount::counter = 0;
#endif

STDMETHODIMP InterfaceAccount::InterfaceSupportsErrorInfo(REFIID riid)
{
   static const IID* arr[] = 
   {
      &IID_IInterfaceAccount,
   };

   for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
   {
      if (InlineIsEqualGUID(*arr[i],riid))
         return S_OK;
   }
   return S_FALSE;
}

void
InterfaceAccount::SetAuthentication(shared_ptr<HM::COMAuthentication> pAuthentication)
{
   m_pAuthentication = pAuthentication;
}

STDMETHODIMP InterfaceAccount::get_Active(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetActive() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_Active(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetActive(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_ADDomain(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetADDomain().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_ADDomain(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsDomainAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      m_pObject->SetADDomain(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_Address(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetAddress().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_Address(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsDomainAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      HM::String sAddress = newVal;
      sAddress.Trim();
   
      m_pObject->SetAddress(sAddress);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_DomainID(long *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = (long) m_pObject->GetDomainID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_DomainID(LONG newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      // Only here for backwards compatibility (4.x)
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_IsAD(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (m_pObject->GetIsAD())
         *pVal = VARIANT_TRUE;
      else
         *pVal = VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_IsAD(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsDomainAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      m_pObject->SetIsAD(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_Password(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetPassword().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_Password(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      // The password isn't encrypted. Encrypt it now using MD5.
      int preferredHashAlgorithm = HM::IniFileSettings::Instance()->GetPreferredHashAlgorithm();
      String sPassword = HM::Crypt::Instance()->EnCrypt(newVal, (HM::Crypt::EncryptionType) preferredHashAlgorithm);
   
      m_pObject->SetPassword(sPassword);
      m_pObject->SetPasswordEncryption(preferredHashAlgorithm);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_Size(float *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      float fMB = (1024*1024);
   
      __int64 accountSizeBytes = AccountSizeCache::Instance()->GetSize(m_pObject->GetID());
   
      *pVal = HM::Math::Round((float) accountSizeBytes / fMB,3);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::Save()
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      String sErrorMessage;
      if (PersistentAccount::SaveObject(m_pObject, sErrorMessage, true))
      {
         // Add to parent collection
         AddToParentCollection();
   
         return S_OK;
      }
   
      return COMError::GenerateError("Failed to save object. " +  sErrorMessage);
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::UnlockMailbox()
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      __int64 iAccountID = m_pObject->GetID();
      POP3Sessions::Instance()->Unlock(iAccountID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::ValidatePassword(BSTR Password, VARIANT_BOOL *pCorrect)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pCorrect = HM::PasswordValidator::ValidatePassword(m_pObject, Password) ? VARIANT_TRUE : VARIANT_FALSE; 
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_ADUsername(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetADUsername().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_ADUsername(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsDomainAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      m_pObject->SetADUsername(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::DeleteMessages()
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      PersistentAccount::DeleteMessages(m_pObject);
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_Messages(IInterfaceMessages **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceMessages>* pMessages = new CComObject<InterfaceMessages>();
      pMessages->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Messages> pMsgs = m_pObject->GetMessages();
   
      if (!pMsgs)
         return DISP_E_BADINDEX;  
   
      pMessages->Attach(pMsgs);
      pMessages->AddRef();
      *pVal = pMessages;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_MaxSize(long *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetAccountMaxSize();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_MaxSize(long pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsDomainAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      m_pObject->SetAccountMaxSize(pVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_VacationMessageIsOn(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = PersistentAccount::GetIsVacationMessageOn(m_pObject) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_VacationMessageIsOn(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetVacationMessageIsOn(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_VacationMessage(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

   
      *pVal = m_pObject->GetVacationMessage().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_VacationMessage(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetVacationMessage(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_VacationSubject(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

   
      *pVal = m_pObject->GetVacationSubject().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_VacationSubject(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetVacationSubject(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_VacationMessageExpires(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetVacationExpires() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_VacationMessageExpires(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetVacationExpires(newVal == VARIANT_TRUE);
      return S_OK;   
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_VacationMessageExpiresDate(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetVacationExpiresDate().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_VacationMessageExpiresDate(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      HM::String string = newVal;
      
      // Validate input date.
      if (string.Left(4) == _T("0000"))
         string = "";
   
      if (string.GetLength() == 0)
         string = Time::GetCurrentDate();
      else if (string.GetLength() != 10 || !Time::IsValidSystemDate(string))
         return COMError::GenerateError("Invalid auto-reply expiry date");
   
      m_pObject->SetVacationExpiresDate(string);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_FetchAccounts(IInterfaceFetchAccounts **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceFetchAccounts>* pItem = new CComObject<InterfaceFetchAccounts >();
      pItem->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::FetchAccounts> pFetchAccounts = shared_ptr<HM::FetchAccounts>(new HM::FetchAccounts(m_pObject->GetID()));
   
      pFetchAccounts->Refresh();
   
      if (pFetchAccounts)
      {
         pItem->Attach(pFetchAccounts);
         pItem->AddRef();
         *pVal = pItem;
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_AdminLevel(eAdminLevel *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = (eAdminLevel) m_pObject->GetAdminLevel();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_AdminLevel(eAdminLevel newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      // Check that the user has permission to do this change.
      switch (newVal)
      {
      case hAdminLevelNormal:
      case hAdminLevelDomainAdmin:
         {
            // The client wants to give this user normal or domain level
            // rights. This is OK if the user is domain or server admin.
            if (!m_pAuthentication->GetIsDomainAdmin() && !m_pAuthentication->GetIsServerAdmin()) 
               return m_pAuthentication->GetAccessDenied(); 
   
            break;
         }
      case hAdminLevelServerAdmin:
         {
            if (m_pObject->GetAdminLevel() == hAdminLevelServerAdmin)
            {
               // It's OK to set this user to admin since the user already is admin. We
               // don't need to change anything.
               return S_OK;
            }
   
            if (!m_pAuthentication->GetIsServerAdmin())
            {
               // Only server admins are allowed to give other users server admin rights.
               return m_pAuthentication->GetAccessDenied(); 
            }
   
            break;
         }
      }
   
   
      m_pObject->SetAdminLevel((HM::Account::AdminLevel) newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_Rules(IInterfaceRules **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceRules >* pItem = new CComObject<InterfaceRules >();
      pItem->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Rules> pRules = m_pObject->GetRules();
   
      if (pRules)
      {
         pItem->Attach(pRules);
         pItem->AddRef();
         *pVal = pItem;
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_IMAPFolders(IInterfaceIMAPFolders **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (m_pObject->GetID() == 0)
         return DISP_E_BADINDEX;  
   
      CComObject<InterfaceIMAPFolders>* pItem = new CComObject<InterfaceIMAPFolders >();
      pItem->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IMAPFolders> pFolders = HM::IMAPFolderContainer::Instance()->GetFoldersForAccount(m_pObject->GetID());
   
      if (pFolders)
      {
         pItem->Attach(pFolders);
         pItem->AddRef();
         *pVal = pItem;
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceAccount::get_QuotaUsed(long *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      __int64 accountSizeBytes = AccountSizeCache::Instance()->GetSize(m_pObject->GetID());
   
      __int64 iMaxSize = ((__int64) m_pObject->GetAccountMaxSize()) * 1024; // Convert from MB to KB
      __int64 iCurrentSize = accountSizeBytes / 1024; // Convert from Bytes to KB
   
      int iPercentageUsed = 0;
      
      if (iMaxSize > 0)
         iPercentageUsed = (int) (((float) iCurrentSize/ (float) iMaxSize) * 100);
   
      *pVal = iPercentageUsed;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_ForwardEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetForwardEnabled() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_ForwardEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetForwardEnabled(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_ForwardAddress(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetForwardAddress().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_ForwardAddress(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetForwardAddress(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_ForwardKeepOriginal(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetForwardKeepOriginal() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_ForwardKeepOriginal(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetForwardKeepOriginal(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_SignatureEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal  = m_pObject->GetEnableSignature() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_SignatureEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetEnableSignature(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_SignaturePlainText(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal  = m_pObject->GetSignaturePlainText().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_SignaturePlainText(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetSignaturePlainText(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_SignatureHTML(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal  = m_pObject->GetSignatureHTML().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_SignatureHTML(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetSignatureHTML(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_PersonFirstName(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal  = m_pObject->GetPersonFirstName().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_PersonFirstName(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetPersonFirstName(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_PersonLastName(BSTR *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal  = m_pObject->GetPersonLastName().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::put_PersonLastName(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetPersonLastName(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::get_LastLogonTime(VARIANT *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      HM::DateTime dt = Time::GetDateFromSystemDate(m_pObject->GetLastLogonTime());
      
      *pVal  = dt.GetVariant();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAccount::Delete()
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsDomainAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      if (!m_pParentCollection)
         return PersistentAccount::DeleteObject(m_pObject) ? S_OK : S_FALSE;
   
      m_pParentCollection->DeleteItemByDBID(m_pObject->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

