// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceDistributionList.h"
#include "InterfaceDistributionListRecipients.h"

#include "../common/persistence/PersistentDistributionList.h"
#include "../common/persistence/PersistentDistributionListRecipient.h"

#include "../common/bo/DistributionLists.h"
#include "../common/bo/DistributionListRecipients.h"

#include "COMError.h"


#ifdef _DEBUG
   long InterfaceDistributionList::counter = 0;
#endif

STDMETHODIMP InterfaceDistributionList::InterfaceSupportsErrorInfo(REFIID riid)
{
   try
   {
      static const IID* arr[] = 
      {
         &IID_IInterfaceDistributionList,
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

STDMETHODIMP InterfaceDistributionList::get_ID(long *pVal)
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

STDMETHODIMP InterfaceDistributionList::get_Address(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      HM::String sName = object_->GetAddress();
   
      *pVal = sName.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionList::put_Address(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      HM::String sName = newVal;
      object_->SetAddress(sName);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionList::get_RequireSenderAddress(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      HM::String sAddress = object_->GetRequireAddress();
   
      *pVal = sAddress.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionList::put_RequireSenderAddress(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      HM::String sName = newVal;
      object_->SetRequireAddress(sName);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionList::get_RequireSMTPAuth(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      if (object_->GetRequireAuth())
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

STDMETHODIMP InterfaceDistributionList::put_RequireSMTPAuth(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      if (newVal == VARIANT_TRUE)
         object_->SetRequireAuth(true);
      else
         object_->SetRequireAuth(false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionList::get_Active(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      if (object_->GetActive())
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

STDMETHODIMP InterfaceDistributionList::put_Active(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      if (newVal == VARIANT_TRUE)
         object_->SetActive(true);
      else
         object_->SetActive(false);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionList::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
   
      if (!object_)
         return S_FALSE;
   
      HM::PersistentDistributionList::DeleteObject(object_);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionList::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!object_)
         return S_FALSE;
   
      HM::String sErrorMessage;
      if (HM::PersistentDistributionList::SaveObject(object_, sErrorMessage, HM::PersistenceModeNormal))
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

STDMETHODIMP InterfaceDistributionList::get_Recipients(IInterfaceDistributionListRecipients **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceDistributionListRecipients>* pItem = new CComObject<InterfaceDistributionListRecipients>();
      pItem->SetAuthentication(authentication_);
   
      pItem->SetListID(object_->GetID());
      std::shared_ptr<HM::DistributionListRecipients> pRecipients = object_->GetMembers();
   
      if (pRecipients)
      {
         pItem->Attach(pRecipients);
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

STDMETHODIMP InterfaceDistributionList::get_Mode(eDistributionListMode *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      switch (object_->GetListMode())
      {
      case HM::DistributionList::LMPublic:
         *pVal = eLMPublic;
         break;
      case HM::DistributionList::LMMembership:
         *pVal = eLMMembership;
         break;
      case HM::DistributionList::LMAnnouncement:
         *pVal = eLMAnnouncement;
         break;
      default:
         *pVal = eLMPublic;
         break;
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDistributionList::put_Mode(eDistributionListMode newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      HM::DistributionList::ListMode iMode = HM::DistributionList::LMPublic;
   
      switch (newVal)
      {
      case eLMPublic:
         iMode = HM::DistributionList::LMPublic;
         break;
      case eLMMembership:
         iMode = HM::DistributionList::LMMembership;
         break;
      case eLMAnnouncement:
         iMode = HM::DistributionList::LMAnnouncement;
         break;
      }
   
      object_->SetListMode(iMode);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

