// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceFetchAccount.h"

#include "../ExternalFetcher/ExternalFetchManager.h"

#include "../Common/BO/FetchAccounts.h"
#include "../Common/Persistence/PersistentFetchAccount.h"

#include "COMError.h"

InterfaceFetchAccount::InterfaceFetchAccount()
{
   m_pObject = shared_ptr<HM::FetchAccount>(new HM::FetchAccount());
}

STDMETHODIMP InterfaceFetchAccount::get_ID(LONG* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = (long) m_pObject->GetID();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_Name(BSTR* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetName().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_Name(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetName(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_ServerAddress(BSTR* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetServerAddress().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_ServerAddress(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetServerAddress(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_Port(LONG* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetPort();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_Port(LONG newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetPort(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_AccountID(LONG* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = (long) m_pObject->GetAccountID();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_AccountID(LONG newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetAccountID(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_ServerType(LONG* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetServerType();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_ServerType(LONG newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetServerType((HM::FetchAccount::ServerType) newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_Username(BSTR* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetUsername().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_Username(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetUsername(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_Password(BSTR* pVal)
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

STDMETHODIMP InterfaceFetchAccount::put_Password(BSTR newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetPassword(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_MinutesBetweenFetch(LONG* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetMinutesBetweenTry();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_MinutesBetweenFetch(LONG newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetMinutesBetweenTry(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_DaysToKeepMessages(LONG* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetDaysToKeep();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_DaysToKeepMessages(LONG newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetDaysToKeep(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::Save(void)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      HM::PersistentFetchAccount oPersAccount;
      
      if (oPersAccount.SaveObject(m_pObject))
      {
         // Add to parent collection
         AddToParentCollection();
   
         return S_OK;
      }
   
      return COMError::GenerateError("Failed to save object. See hMailServer error log.");
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_Enabled(VARIANT_BOOL* pVal)
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

STDMETHODIMP InterfaceFetchAccount::put_Enabled(VARIANT_BOOL newVal)
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

STDMETHODIMP InterfaceFetchAccount::get_ProcessMIMERecipients(VARIANT_BOOL* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetProcessMIMERecipients() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_ProcessMIMERecipients(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetProcessMIMERecipients(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_ProcessMIMEDate(VARIANT_BOOL* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetProcessMIMEDate() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_ProcessMIMEDate(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetProcessMIMEDate(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::DownloadNow(void)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      HM::PersistentFetchAccount::SetRetryNow(m_pObject->GetID());
   
      // Notify the fetch manager to look for accounts to fetch from now.
      HM::Application::Instance()->GetExternalFetchManager()->SetCheckNow();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_UseSSL(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetUseSSL() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_UseSSL(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetUseSSL(newVal == VARIANT_TRUE ? true : false);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::Delete()
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pAuthentication->GetIsDomainAdmin())
         return m_pAuthentication->GetAccessDenied();
   
      if (!m_pParentCollection)
         return HM::PersistentFetchAccount::DeleteObject(m_pObject) ? S_OK : S_FALSE;
   
      m_pParentCollection->DeleteItemByDBID(m_pObject->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_NextDownloadTime(BSTR* pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetNextTry().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_IsLocked(VARIANT_BOOL* pVal)
{
	try
	{
		if (!m_pObject)
			return GetAccessDenied();

		bool locked = HM::PersistentFetchAccount::IsLocked(m_pObject->GetID());
		*pVal = locked ? VARIANT_TRUE : VARIANT_FALSE;
		return S_OK;
	}
	catch (...)
	{
		return COMError::GenerateGenericMessage();
	}
}

STDMETHODIMP InterfaceFetchAccount::get_UseAntiSpam(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetUseAntiSpam() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_UseAntiSpam(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetUseAntiSpam(newVal == VARIANT_TRUE ? true : false);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_UseAntiVirus(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetUseAntiVirus() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_UseAntiVirus(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetUseAntiVirus(newVal == VARIANT_TRUE ? true : false);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_EnableRouteRecipients(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetEnableRouteRecipients() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_EnableRouteRecipients(VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetEnableRouteRecipients(newVal == VARIANT_TRUE ? true : false);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

