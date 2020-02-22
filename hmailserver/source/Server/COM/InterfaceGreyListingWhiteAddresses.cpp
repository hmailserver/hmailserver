// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceGreyListingWhiteAddresses.h"

#include "..\Common\BO\GreyListingWhiteAddress.h"
#include "InterfaceGreyListingWhiteAddress.h"

void 
InterfaceGreyListingWhiteAddresses::Attach(std::shared_ptr<HM::GreyListingWhiteAddresses> pBA) 
{ 
   gl_white_addresses_ = pBA; 
}

STDMETHODIMP 
InterfaceGreyListingWhiteAddresses::Refresh()
{
   try
   {
      if (!gl_white_addresses_)
         return GetAccessDenied();

      if (!gl_white_addresses_)
         return S_FALSE;
   
      gl_white_addresses_->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceGreyListingWhiteAddresses::get_Count(long *pVal)
{
   try
   {
      if (!gl_white_addresses_)
         return GetAccessDenied();

      *pVal = gl_white_addresses_->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGreyListingWhiteAddresses::get_Item(long Index, IInterfaceGreyListingWhiteAddress **pVal)
{
   try
   {
      if (!gl_white_addresses_)
         return GetAccessDenied();

      CComObject<InterfaceGreyListingWhiteAddress>* pInterfaceGreyListingWhiteAddress = new CComObject<InterfaceGreyListingWhiteAddress>();
      pInterfaceGreyListingWhiteAddress->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::GreyListingWhiteAddress> pBA = gl_white_addresses_->GetItem(Index);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceGreyListingWhiteAddress->AttachItem(pBA);
      pInterfaceGreyListingWhiteAddress->AttachParent(gl_white_addresses_, true);
      pInterfaceGreyListingWhiteAddress->AddRef();
      *pVal = pInterfaceGreyListingWhiteAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGreyListingWhiteAddresses::DeleteByDBID(long DBID)
{
   try
   {
      if (!gl_white_addresses_)
         return GetAccessDenied();

      gl_white_addresses_->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGreyListingWhiteAddresses::get_ItemByDBID(long lDBID, IInterfaceGreyListingWhiteAddress **pVal)
{
   try
   {
      if (!gl_white_addresses_)
         return GetAccessDenied();

      CComObject<InterfaceGreyListingWhiteAddress>* pInterfaceGreyListingWhiteAddress = new CComObject<InterfaceGreyListingWhiteAddress>();
      pInterfaceGreyListingWhiteAddress->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::GreyListingWhiteAddress> pBA = gl_white_addresses_->GetItemByDBID(lDBID);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceGreyListingWhiteAddress->AttachItem(pBA);
      pInterfaceGreyListingWhiteAddress->AttachParent(gl_white_addresses_, true);
      pInterfaceGreyListingWhiteAddress->AddRef();
   
      *pVal = pInterfaceGreyListingWhiteAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGreyListingWhiteAddresses::get_ItemByName(BSTR sName, IInterfaceGreyListingWhiteAddress **pVal)
{
   try
   {
      if (!gl_white_addresses_)
         return GetAccessDenied();

      CComObject<InterfaceGreyListingWhiteAddress>* pInterfaceGreyListingWhiteAddress = new CComObject<InterfaceGreyListingWhiteAddress>();
      pInterfaceGreyListingWhiteAddress->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::GreyListingWhiteAddress> pBA = gl_white_addresses_->GetItemByName(sName);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceGreyListingWhiteAddress->AttachItem(pBA);
      pInterfaceGreyListingWhiteAddress->AttachParent(gl_white_addresses_, true);
      pInterfaceGreyListingWhiteAddress->AddRef();
   
      *pVal = pInterfaceGreyListingWhiteAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGreyListingWhiteAddresses::Add(IInterfaceGreyListingWhiteAddress **pVal)
{
   try
   {
      if (!gl_white_addresses_)
         return GetAccessDenied();

      if (!gl_white_addresses_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceGreyListingWhiteAddress>* pInterfaceGreyListingWhiteAddress = new CComObject<InterfaceGreyListingWhiteAddress>();
      pInterfaceGreyListingWhiteAddress->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::GreyListingWhiteAddress> pBA = std::shared_ptr<HM::GreyListingWhiteAddress>(new HM::GreyListingWhiteAddress);
   
      pInterfaceGreyListingWhiteAddress->AttachItem(pBA);
      pInterfaceGreyListingWhiteAddress->AttachParent(gl_white_addresses_, false);
      pInterfaceGreyListingWhiteAddress->AddRef();
   
      *pVal = pInterfaceGreyListingWhiteAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


