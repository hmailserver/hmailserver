// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceGroupMember.h"
#include "InterfaceAccount.h"

#include "../Common/BO/GroupMember.h"
#include "../Common/BO/GroupMembers.h"
#include "../Common/BO/Account.h"
#include "../Common/Cache/CacheContainer.h"
#include "../Common/Persistence/PersistentGroupMember.h"
#include "../Common/Persistence/PersistentAccount.h"


STDMETHODIMP 
InterfaceGroupMember::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (HM::PersistentGroupMember::SaveObject(object_))
      {
         // Add to parent collection
         AddToParentCollection();
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceGroupMember::get_ID(long *pVal)
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

STDMETHODIMP InterfaceGroupMember::get_GroupID(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetGroupID();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceGroupMember::put_GroupID(long iVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetGroupID(iVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceGroupMember::get_AccountID(long *pVal)
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

STDMETHODIMP InterfaceGroupMember::put_AccountID(long iVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetAccountID(iVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGroupMember::get_Account(IInterfaceAccount **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceAccount>* pInterfaceAccount = new CComObject<InterfaceAccount>();
      pInterfaceAccount->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Account> pAccount = std::shared_ptr<HM::Account>(new HM::Account);
         
      if (!HM::PersistentAccount::ReadObject(pAccount, (__int64) object_->GetAccountID()))
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

STDMETHODIMP InterfaceGroupMember::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!authentication_->GetIsDomainAdmin())
         return authentication_->GetAccessDenied();
   
      if (!parent_collection_)
         return HM::PersistentGroupMember::DeleteObject(object_) ? S_OK : S_FALSE;
   
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


