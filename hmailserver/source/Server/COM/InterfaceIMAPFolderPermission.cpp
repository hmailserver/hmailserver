// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "InterfaceIMAPFolderPermission.h"
#include "InterfaceAccount.h"
#include "InterfaceGroup.h"

#include "../Common/BO/ACLPermissions.h"
#include "../Common/BO/Groups.h"
#include "../Common/Persistence/PersistentACLPermission.h"
#include "../Common/Persistence/PersistentAccount.h"
#include "../Common/Cache/CacheContainer.h"
#include "../IMAP/IMAPConfiguration.h"

#include "COMError.h"

STDMETHODIMP InterfaceIMAPFolderPermission::InterfaceSupportsErrorInfo(REFIID riid)
{
   try
   {
      static const IID* arr[] = 
      {
         &IID_IInterfaceIMAPFolderPermission,
      };
   
      for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
      {
         if (InlineIsEqualGUID(*arr[i],riid))
            return S_OK;
      }
      return S_FALSE;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermission::get_ShareFolderID(long *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = (long) m_pObject->GetShareFolderID();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermission::get_PermissionType(eACLPermissionType *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = (eACLPermissionType) m_pObject->GetPermissionType();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermission::put_PermissionType(eACLPermissionType newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetPermissionType((HM::ACLPermission::ePermissionType) newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermission::get_PermissionGroupID(long *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = (long) m_pObject->GetPermissionGroupID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermission::put_PermissionGroupID(LONG newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetPermissionGroupID(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermission::get_PermissionAccountID(long *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = (long) m_pObject->GetPermissionAccountID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermission::put_PermissionAccountID(LONG newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetPermissionAccountID(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermission::get_Value(long *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = (long) m_pObject->GetValue();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermission::put_Value(LONG newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetValue(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermission::Save()
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (HM::PersistentACLPermission::SaveObject(m_pObject))
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

STDMETHODIMP 
InterfaceIMAPFolderPermission::get_Permission(eACLPermission iPermission, VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetAllow((HM::ACLPermission::ePermission) iPermission) ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolderPermission::put_Permission(eACLPermission iPermission, VARIANT_BOOL newVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetAllow((HM::ACLPermission::ePermission) iPermission, newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermission::Delete()
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pParentCollection)
         return HM::PersistentACLPermission::DeleteObject(m_pObject) ? S_OK : S_FALSE;
   
      m_pParentCollection->DeleteItemByDBID(m_pObject->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolderPermission::get_Account(IInterfaceAccount **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceAccount>* pInterfaceAccount = new CComObject<InterfaceAccount>();
      pInterfaceAccount->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Account> pAccount = shared_ptr<HM::Account>(new HM::Account);
   
      if (!HM::PersistentAccount::ReadObject(pAccount, (__int64) m_pObject->GetPermissionAccountID()))
         return DISP_E_BADINDEX;
   
      pInterfaceAccount->AttachItem(pAccount);
      pInterfaceAccount->AddRef();
      *pVal = pInterfaceAccount;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolderPermission::get_Group(IInterfaceGroup **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceGroup>* pInterfaceGroup = new CComObject<InterfaceGroup>();
      pInterfaceGroup->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Group> pGroup = HM::Configuration::Instance()->GetIMAPConfiguration()->GetGroups()->GetItemByDBID(m_pObject->GetPermissionGroupID());
   
      if (!pGroup)
         return DISP_E_BADINDEX;
   
      pInterfaceGroup->AttachItem(pGroup);
      pInterfaceGroup->AddRef();
      *pVal = pInterfaceGroup;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

