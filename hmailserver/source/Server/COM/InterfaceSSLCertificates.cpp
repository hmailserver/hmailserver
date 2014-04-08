// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "InterfaceSSLCertificates.h"

#include "..\Common\BO\SSLCertificate.h"
#include "..\Common\BO\SSLCertificates.h"

#include "InterfaceSSLCertificate.h"

#include "COMError.h"

STDMETHODIMP InterfaceSSLCertificates::InterfaceSupportsErrorInfo(REFIID riid)
{
   static const IID* arr[] = 
   {
      &IID_IInterfaceSSLCertificates,
   };

   for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
   {
      if (InlineIsEqualGUID(*arr[i],riid))
         return S_OK;
   }

   return S_FALSE;
}
   
void 
InterfaceSSLCertificates::Attach(shared_ptr<HM::SSLCertificates> pBA) 
{ 
   m_pSSLCertificates = pBA; 
}

STDMETHODIMP 
InterfaceSSLCertificates::Refresh()
{
   try
   {
      if (!m_pSSLCertificates)
         return GetAccessDenied();
   
      m_pSSLCertificates->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceSSLCertificates::Clear()
{
   try
   {
      if (!m_pSSLCertificates)
         return GetAccessDenied();
   
      if (!m_pSSLCertificates->DeleteAll())
         return COMError::GenerateError("Clearing of SSL certificates failed");
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSSLCertificates::get_Count(long *pVal)
{
   try
   {
      if (!m_pSSLCertificates)
         return GetAccessDenied();

      *pVal = m_pSSLCertificates->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceSSLCertificates::get_Item(long Index, IInterfaceSSLCertificate **pVal)
{
   try
   {
      if (!m_pSSLCertificates)
         return GetAccessDenied();

      CComObject<InterfaceSSLCertificate>* pInterfaceSSLCertificate = new CComObject<InterfaceSSLCertificate>();
      pInterfaceSSLCertificate->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::SSLCertificate> pBA = m_pSSLCertificates->GetItem(Index);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceSSLCertificate->AttachItem(pBA);
      pInterfaceSSLCertificate->AttachParent(m_pSSLCertificates, true);
      pInterfaceSSLCertificate->AddRef();
      *pVal = pInterfaceSSLCertificate;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceSSLCertificates::DeleteByDBID(long DBID)
{
   try
   {
      if (!m_pSSLCertificates)
         return GetAccessDenied();

      m_pSSLCertificates->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceSSLCertificates::get_ItemByDBID(long lDBID, IInterfaceSSLCertificate **pVal)
{
   try
   {
      if (!m_pSSLCertificates)
         return GetAccessDenied();

      CComObject<InterfaceSSLCertificate>* pInterfaceSSLCertificate = new CComObject<InterfaceSSLCertificate>();
      pInterfaceSSLCertificate->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::SSLCertificate> pBA = m_pSSLCertificates->GetItemByDBID(lDBID);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceSSLCertificate->AttachItem(pBA);
      pInterfaceSSLCertificate->AttachParent(m_pSSLCertificates, true);
      pInterfaceSSLCertificate->AddRef();
   
      *pVal = pInterfaceSSLCertificate;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceSSLCertificates::Add(IInterfaceSSLCertificate **pVal)
{
   try
   {
      if (!m_pSSLCertificates)
         return GetAccessDenied();

      if (!m_pSSLCertificates)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceSSLCertificate>* pInterfaceSSLCertificate = new CComObject<InterfaceSSLCertificate>();
      pInterfaceSSLCertificate->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::SSLCertificate> pBA = shared_ptr<HM::SSLCertificate>(new HM::SSLCertificate);
   
      pInterfaceSSLCertificate->AttachItem(pBA);
      pInterfaceSSLCertificate->AttachParent(m_pSSLCertificates, false);
      pInterfaceSSLCertificate->AddRef();
   
      *pVal = pInterfaceSSLCertificate;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

