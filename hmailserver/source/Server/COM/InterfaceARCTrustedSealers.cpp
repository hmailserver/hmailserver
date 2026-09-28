// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceARCTrustedSealers.h"

#include "../Common/BO/ARCTrustedSealer.h"
#include "InterfaceARCTrustedSealer.h"

void 
InterfaceARCTrustedSealers::Attach(std::shared_ptr<HM::ARCTrustedSealers> pTrustedSealers) 
{ 
   trusted_sealers_ = pTrustedSealers; 
}

STDMETHODIMP 
InterfaceARCTrustedSealers::Refresh()
{
   try
   {
      if (!trusted_sealers_)
         return GetAccessDenied();

      if (!trusted_sealers_)
         return S_FALSE;
   
      trusted_sealers_->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceARCTrustedSealers::get_Count(long *pVal)
{
   try
   {
      if (!trusted_sealers_)
         return GetAccessDenied();

      *pVal = trusted_sealers_->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceARCTrustedSealers::get_Item(long Index, IInterfaceARCTrustedSealer **pVal)
{
   try
   {
      if (!trusted_sealers_)
         return GetAccessDenied();

      CComObject<InterfaceARCTrustedSealer>* pInterfaceSealer = new CComObject<InterfaceARCTrustedSealer>();
      pInterfaceSealer->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::ARCTrustedSealer> pSealer = trusted_sealers_->GetItem(Index);
   
      if (!pSealer)
         return DISP_E_BADINDEX;
   
      pInterfaceSealer->AttachItem(pSealer);
      pInterfaceSealer->AttachParent(trusted_sealers_, true);
      pInterfaceSealer->AddRef();
      *pVal = pInterfaceSealer;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceARCTrustedSealers::DeleteByDBID(long DBID)
{
   try
   {
      if (!trusted_sealers_)
         return GetAccessDenied();

      trusted_sealers_->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceARCTrustedSealers::get_ItemByDBID(long lDBID, IInterfaceARCTrustedSealer **pVal)
{
   try
   {
      if (!trusted_sealers_)
         return GetAccessDenied();

      CComObject<InterfaceARCTrustedSealer>* pInterfaceSealer = new CComObject<InterfaceARCTrustedSealer>();
      pInterfaceSealer->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::ARCTrustedSealer> pSealer = trusted_sealers_->GetItemByDBID(lDBID);
   
      if (!pSealer)
         return DISP_E_BADINDEX;
   
      pInterfaceSealer->AttachItem(pSealer);
      pInterfaceSealer->AttachParent(trusted_sealers_, true);
      pInterfaceSealer->AddRef();
   
      *pVal = pInterfaceSealer;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceARCTrustedSealers::get_ItemByName(BSTR sName, IInterfaceARCTrustedSealer **pVal)
{
   try
   {
      if (!trusted_sealers_)
         return GetAccessDenied();

      CComObject<InterfaceARCTrustedSealer>* pInterfaceSealer = new CComObject<InterfaceARCTrustedSealer>();
      pInterfaceSealer->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::ARCTrustedSealer> pSealer = trusted_sealers_->GetItemByName(sName);
   
      if (!pSealer)
         return DISP_E_BADINDEX;
   
      pInterfaceSealer->AttachItem(pSealer);
      pInterfaceSealer->AttachParent(trusted_sealers_, true);
      pInterfaceSealer->AddRef();
   
      *pVal = pInterfaceSealer;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceARCTrustedSealers::Add(IInterfaceARCTrustedSealer **pVal)
{
   try
   {
      if (!trusted_sealers_)
         return GetAccessDenied();

      if (!trusted_sealers_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceARCTrustedSealer>* pInterfaceSealer = new CComObject<InterfaceARCTrustedSealer>();
      pInterfaceSealer->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::ARCTrustedSealer> pSealer = std::shared_ptr<HM::ARCTrustedSealer>(new HM::ARCTrustedSealer);
   
      pInterfaceSealer->AttachItem(pSealer);
      pInterfaceSealer->AttachParent(trusted_sealers_, false);
      pInterfaceSealer->AddRef();
   
      *pVal = pInterfaceSealer;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


