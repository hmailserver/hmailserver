// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "InterfaceGroup.h"
#include "InterfaceGroups.h"
#include "InterfaceGroupMembers.h"

#include "../Common/Persistence/PersistentGroup.h"
#include "../Common/BO/Group.h"
#include "../Common/BO/Groups.h"

#include "COMError.h"

STDMETHODIMP InterfaceGroup::InterfaceSupportsErrorInfo(REFIID riid)
{
   try
   {
      static const IID* arr[] = 
      {
         &IID_IInterfaceGroup,
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

STDMETHODIMP 
InterfaceGroup::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      HM::String sErrorMessage;
      if (HM::PersistentGroup::SaveObject(object_, sErrorMessage, HM::PersistenceModeNormal))
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

STDMETHODIMP InterfaceGroup::get_ID(long *pVal)
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

STDMETHODIMP InterfaceGroup::get_Name(BSTR *pVal)
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

STDMETHODIMP InterfaceGroup::put_Name(BSTR sVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetName(sVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceGroup::get_Members(IInterfaceGroupMembers **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
      CComObject<InterfaceGroupMembers>* pItem = new CComObject<InterfaceGroupMembers>();
      pItem->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::GroupMembers> pGM = object_->GetMembers();
   
      if (pGM)
      {
         pItem->Attach(pGM);
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

STDMETHODIMP InterfaceGroup::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!authentication_->GetIsDomainAdmin())
         return authentication_->GetAccessDenied();
   
      if (!parent_collection_)
         return HM::PersistentGroup::DeleteObject(object_) ? S_OK : S_FALSE;
   
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

