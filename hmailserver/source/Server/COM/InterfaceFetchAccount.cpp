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
   object_ = std::shared_ptr<HM::FetchAccount>(new HM::FetchAccount());
}

STDMETHODIMP InterfaceFetchAccount::get_ID(LONG* pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetID();
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetName().AllocSysString();
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetName(newVal);
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetServerAddress().AllocSysString();
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetServerAddress(newVal);
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetPort();
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetPort(newVal);
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetAccountID();
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetAccountID(newVal);
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetServerType();
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetServerType((HM::FetchAccount::ServerType) newVal);
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetUsername().AllocSysString();
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetUsername(newVal);
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetPassword().AllocSysString();
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetPassword(newVal);
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetMinutesBetweenTry();
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetMinutesBetweenTry(newVal);
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetDaysToKeep();
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetDaysToKeep(newVal);
   
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
      if (!object_)
         return GetAccessDenied();

      HM::PersistentFetchAccount oPersAccount;
      
      if (oPersAccount.SaveObject(object_))
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetActive() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetActive(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_MIMERecipientHeaders(BSTR* pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetMIMERecipientHeaders().AllocSysString();

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_MIMERecipientHeaders(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetMIMERecipientHeaders(newVal);

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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetProcessMIMERecipients() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetProcessMIMERecipients(newVal == VARIANT_TRUE);
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetProcessMIMEDate() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!object_)
         return GetAccessDenied();

      object_->SetProcessMIMEDate(newVal == VARIANT_TRUE);
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
      if (!object_)
         return GetAccessDenied();

      HM::PersistentFetchAccount::SetRetryNow(object_->GetID());
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetConnectionSecurity() == HM::CSSSL ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!object_)
         return GetAccessDenied();

      if (newVal == VARIANT_TRUE)
         object_->SetConnectionSecurity(HM::CSSSL);
      else
         object_->SetConnectionSecurity(HM::CSNone);

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::put_ConnectionSecurity(eConnectionSecurity newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetConnectionSecurity((HM::ConnectionSecurity) newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceFetchAccount::get_ConnectionSecurity(eConnectionSecurity *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (eConnectionSecurity) object_->GetConnectionSecurity();

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
      if (!object_)
         return GetAccessDenied();

      if (!authentication_->GetIsDomainAdmin())
         return authentication_->GetAccessDenied();
   
      if (!parent_collection_)
         return HM::PersistentFetchAccount::DeleteObject(object_) ? S_OK : S_FALSE;
   
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetNextTry().AllocSysString();
   
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
		if (!object_)
			return GetAccessDenied();

		bool locked = HM::PersistentFetchAccount::IsLocked(object_->GetID());
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetUseAntiSpam() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetUseAntiSpam(newVal == VARIANT_TRUE ? true : false);
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetUseAntiVirus() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetUseAntiVirus(newVal == VARIANT_TRUE ? true : false);
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
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetEnableRouteRecipients() ? VARIANT_TRUE : VARIANT_FALSE;
   
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
      if (!object_)
         return GetAccessDenied();

      object_->SetEnableRouteRecipients(newVal == VARIANT_TRUE ? true : false);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

