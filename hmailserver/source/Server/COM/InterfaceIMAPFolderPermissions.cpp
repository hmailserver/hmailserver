// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceIMAPFolderPermissions.h"
#include "InterfaceIMAPFolderPermission.h"

#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/ACLPermissions.h"
#include "../Common/BO/ACLPermission.h"
#include "../Common/Application/ACLManager.h"

void 
InterfaceIMAPFolderPermissions::AttachItem(std::shared_ptr<HM::IMAPFolder> pFolder)
{
   folder_ = pFolder;
   
   // Fetch the permissions in this ACL. It's really these we are
   // interested in in this interface.
   acl_permission_ = folder_->GetPermissions();
}

STDMETHODIMP InterfaceIMAPFolderPermissions::get_Count(long *pVal)
{
   try
   {
      if (!acl_permission_)
         return GetAccessDenied();

      *pVal = acl_permission_->GetCount();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermissions::Delete(long Index)
{
   try
   {
      if (!acl_permission_)
         return GetAccessDenied();

      acl_permission_->DeleteItem(Index);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermissions::Refresh()
{
   try
   {
      if (!acl_permission_)
         return GetAccessDenied();

      acl_permission_->Refresh();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermissions::get_Item(long Index, IInterfaceIMAPFolderPermission **pVal)
{
   try
   {
      if (!acl_permission_)
         return GetAccessDenied();

      CComObject<InterfaceIMAPFolderPermission>* pACLPermission = new CComObject<InterfaceIMAPFolderPermission>();
      pACLPermission->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::ACLPermission> pPersACLPermission = acl_permission_->GetItem(Index);
   
      if (!pPersACLPermission)
         return DISP_E_BADINDEX;  
   
      pACLPermission->AttachItem(pPersACLPermission);
      pACLPermission->AttachParent(acl_permission_, true);
      pACLPermission->AddRef();
      *pVal = pACLPermission;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermissions::Add(IInterfaceIMAPFolderPermission **pVal)
{
   try
   {
      if (!acl_permission_)
         return GetAccessDenied();

      if (!authentication_->GetIsDomainAdmin())
         return authentication_->GetAccessDenied();
   
      if (!acl_permission_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceIMAPFolderPermission>* pIntACLPermission = new CComObject<InterfaceIMAPFolderPermission>();
      pIntACLPermission->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::ACLPermission> pACLPermission = std::shared_ptr<HM::ACLPermission>(new HM::ACLPermission);
   
      pACLPermission->SetShareFolderID(folder_->GetID());
   
      pIntACLPermission->AttachItem(pACLPermission);
      pIntACLPermission->AttachParent(acl_permission_, false);
   
      pIntACLPermission->AddRef();
      *pVal = pIntACLPermission;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermissions::get_ItemByDBID(long DBID, IInterfaceIMAPFolderPermission **pVal)
{
   try
   {
      if (!acl_permission_)
         return GetAccessDenied();

      CComObject<InterfaceIMAPFolderPermission>* pACLPermission = new CComObject<InterfaceIMAPFolderPermission>();
      pACLPermission->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::ACLPermission> pPersACLPermission = acl_permission_->GetItemByDBID(DBID);
   
      if (pPersACLPermission)
      {
         pACLPermission->AttachItem(pPersACLPermission);
         pACLPermission->AttachParent(acl_permission_, true);
         pACLPermission->AddRef();
         *pVal = pACLPermission;
      }
      else
      {
         return DISP_E_BADINDEX;  
      }
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermissions::DeleteByDBID(long DBID)
{
   try
   {
      if (!acl_permission_)
         return GetAccessDenied();

   
      if (acl_permission_)
         acl_permission_->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolderPermissions::get_ItemByName(BSTR Name, IInterfaceIMAPFolderPermission **pVal)
{
   try
   {
      if (!acl_permission_)
         return GetAccessDenied();

      CComObject<InterfaceIMAPFolderPermission>* pACLPermission = new CComObject<InterfaceIMAPFolderPermission>();
      pACLPermission->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::ACLPermission> pPersACLPermission = acl_permission_->GetItemByName(Name);
   
      if (pPersACLPermission)
      {
         pACLPermission->AttachItem(pPersACLPermission);
         pACLPermission->AttachParent(acl_permission_, true);
         pACLPermission->AddRef();
         *pVal = pACLPermission;
      }
      else
      {
         return DISP_E_BADINDEX;  
      }
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


