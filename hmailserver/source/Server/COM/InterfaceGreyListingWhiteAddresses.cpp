// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceGreyListingWhiteAddresses.h"

#include "..\Common\BO\GreyListingWhiteAddress.h"
#include "InterfaceGreyListingWhiteAddress.h"

void 
InterfaceGreyListingWhiteAddresses::Attach(shared_ptr<HM::GreyListingWhiteAddresses> pBA) 
{ 
   m_pGLWhiteAddresses = pBA; 
}

STDMETHODIMP 
InterfaceGreyListingWhiteAddresses::Refresh()
{
   try
   {
      if (!m_pGLWhiteAddresses)
         return GetAccessDenied();

      if (!m_pGLWhiteAddresses)
         return S_FALSE;
   
      m_pGLWhiteAddresses->Refresh();
   
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
      if (!m_pGLWhiteAddresses)
         return GetAccessDenied();

      *pVal = m_pGLWhiteAddresses->GetCount();
   
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
      if (!m_pGLWhiteAddresses)
         return GetAccessDenied();

      CComObject<InterfaceGreyListingWhiteAddress>* pInterfaceGreyListingWhiteAddress = new CComObject<InterfaceGreyListingWhiteAddress>();
      pInterfaceGreyListingWhiteAddress->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::GreyListingWhiteAddress> pBA = m_pGLWhiteAddresses->GetItem(Index);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceGreyListingWhiteAddress->AttachItem(pBA);
      pInterfaceGreyListingWhiteAddress->AttachParent(m_pGLWhiteAddresses, true);
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
      if (!m_pGLWhiteAddresses)
         return GetAccessDenied();

      m_pGLWhiteAddresses->DeleteItemByDBID(DBID);
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
      if (!m_pGLWhiteAddresses)
         return GetAccessDenied();

      CComObject<InterfaceGreyListingWhiteAddress>* pInterfaceGreyListingWhiteAddress = new CComObject<InterfaceGreyListingWhiteAddress>();
      pInterfaceGreyListingWhiteAddress->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::GreyListingWhiteAddress> pBA = m_pGLWhiteAddresses->GetItemByDBID(lDBID);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceGreyListingWhiteAddress->AttachItem(pBA);
      pInterfaceGreyListingWhiteAddress->AttachParent(m_pGLWhiteAddresses, true);
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
      if (!m_pGLWhiteAddresses)
         return GetAccessDenied();

      CComObject<InterfaceGreyListingWhiteAddress>* pInterfaceGreyListingWhiteAddress = new CComObject<InterfaceGreyListingWhiteAddress>();
      pInterfaceGreyListingWhiteAddress->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::GreyListingWhiteAddress> pBA = m_pGLWhiteAddresses->GetItemByName(sName);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceGreyListingWhiteAddress->AttachItem(pBA);
      pInterfaceGreyListingWhiteAddress->AttachParent(m_pGLWhiteAddresses, true);
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
      if (!m_pGLWhiteAddresses)
         return GetAccessDenied();

      if (!m_pGLWhiteAddresses)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceGreyListingWhiteAddress>* pInterfaceGreyListingWhiteAddress = new CComObject<InterfaceGreyListingWhiteAddress>();
      pInterfaceGreyListingWhiteAddress->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::GreyListingWhiteAddress> pBA = shared_ptr<HM::GreyListingWhiteAddress>(new HM::GreyListingWhiteAddress);
   
      pInterfaceGreyListingWhiteAddress->AttachItem(pBA);
      pInterfaceGreyListingWhiteAddress->AttachParent(m_pGLWhiteAddresses, false);
      pInterfaceGreyListingWhiteAddress->AddRef();
   
      *pVal = pInterfaceGreyListingWhiteAddress;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


