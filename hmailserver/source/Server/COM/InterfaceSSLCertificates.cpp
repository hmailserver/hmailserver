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
InterfaceSSLCertificates::Attach(std::shared_ptr<HM::SSLCertificates> pBA) 
{ 
   ssl_certificates_ = pBA; 
}

STDMETHODIMP 
InterfaceSSLCertificates::Refresh()
{
   try
   {
      if (!ssl_certificates_)
         return GetAccessDenied();
   
      ssl_certificates_->Refresh();
   
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
      if (!ssl_certificates_)
         return GetAccessDenied();
   
      if (!ssl_certificates_->DeleteAll())
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
      if (!ssl_certificates_)
         return GetAccessDenied();

      *pVal = ssl_certificates_->GetCount();
   
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
      if (!ssl_certificates_)
         return GetAccessDenied();

      CComObject<InterfaceSSLCertificate>* pInterfaceSSLCertificate = new CComObject<InterfaceSSLCertificate>();
      pInterfaceSSLCertificate->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::SSLCertificate> pBA = ssl_certificates_->GetItem(Index);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceSSLCertificate->AttachItem(pBA);
      pInterfaceSSLCertificate->AttachParent(ssl_certificates_, true);
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
      if (!ssl_certificates_)
         return GetAccessDenied();

      ssl_certificates_->DeleteItemByDBID(DBID);
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
      if (!ssl_certificates_)
         return GetAccessDenied();

      CComObject<InterfaceSSLCertificate>* pInterfaceSSLCertificate = new CComObject<InterfaceSSLCertificate>();
      pInterfaceSSLCertificate->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::SSLCertificate> pBA = ssl_certificates_->GetItemByDBID(lDBID);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceSSLCertificate->AttachItem(pBA);
      pInterfaceSSLCertificate->AttachParent(ssl_certificates_, true);
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
      if (!ssl_certificates_)
         return GetAccessDenied();

      if (!ssl_certificates_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceSSLCertificate>* pInterfaceSSLCertificate = new CComObject<InterfaceSSLCertificate>();
      pInterfaceSSLCertificate->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::SSLCertificate> pBA = std::shared_ptr<HM::SSLCertificate>(new HM::SSLCertificate);
   
      pInterfaceSSLCertificate->AttachItem(pBA);
      pInterfaceSSLCertificate->AttachParent(ssl_certificates_, false);
      pInterfaceSSLCertificate->AddRef();
   
      *pVal = pInterfaceSSLCertificate;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

