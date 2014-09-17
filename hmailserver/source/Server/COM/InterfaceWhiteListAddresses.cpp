// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceWhiteListAddresses.h"


#include "InterfaceWhiteListAddress.h"

#include "..\Common\BO\WhiteListAddress.h"
#include "..\Common\BO\WhiteListAddresses.h"


void 
InterfaceWhiteListAddresses::Attach(std::shared_ptr<HM::WhiteListAddresses> pBA) 
{ 
   object_ = pBA; 
}

STDMETHODIMP 
InterfaceWhiteListAddresses::Refresh()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!object_)
         return S_FALSE;
   
      object_->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::Clear()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!object_)
         return S_FALSE;
   
      object_->DeleteAll();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceWhiteListAddresses::get_Count(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::get_Item(long Index, IInterfaceWhiteListAddress **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceWhiteListAddress>* pInterfaceWhiteListAddress = new CComObject<InterfaceWhiteListAddress>();
      pInterfaceWhiteListAddress->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::WhiteListAddress> pBA = object_->GetItem(Index);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceWhiteListAddress->AttachItem(pBA);
      pInterfaceWhiteListAddress->AttachParent(object_, true);
      pInterfaceWhiteListAddress->AddRef();
      *pVal = pInterfaceWhiteListAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::DeleteByDBID(long DBID)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::get_ItemByDBID(long lDBID, IInterfaceWhiteListAddress **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceWhiteListAddress>* pInterfaceWhiteListAddress = new CComObject<InterfaceWhiteListAddress>();
      pInterfaceWhiteListAddress->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::WhiteListAddress> pBA = object_->GetItemByDBID(lDBID);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceWhiteListAddress->AttachItem(pBA);
      pInterfaceWhiteListAddress->AttachParent(object_, true);
      pInterfaceWhiteListAddress->AddRef();
   
      *pVal = pInterfaceWhiteListAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::get_ItemByName(BSTR sName, IInterfaceWhiteListAddress **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceWhiteListAddress>* pInterfaceWhiteListAddress = new CComObject<InterfaceWhiteListAddress>();
      pInterfaceWhiteListAddress->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::WhiteListAddress> pBA = object_->GetItemByName(sName);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceWhiteListAddress->AttachItem(pBA);
      pInterfaceWhiteListAddress->AttachParent(object_, true);
      pInterfaceWhiteListAddress->AddRef();
   
      *pVal = pInterfaceWhiteListAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceWhiteListAddresses::Add(IInterfaceWhiteListAddress **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!object_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceWhiteListAddress>* pInterfaceWhiteListAddress = new CComObject<InterfaceWhiteListAddress>();
      pInterfaceWhiteListAddress->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::WhiteListAddress> pBA = std::shared_ptr<HM::WhiteListAddress>(new HM::WhiteListAddress);
   
      pInterfaceWhiteListAddress->AttachItem(pBA);
      pInterfaceWhiteListAddress->AttachParent(object_, false);
   
      pInterfaceWhiteListAddress->AddRef();
   
      *pVal = pInterfaceWhiteListAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


