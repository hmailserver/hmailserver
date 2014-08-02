// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceTCPIPPort.h"
#include "COMError.h"


STDMETHODIMP 
InterfaceTCPIPPort::InterfaceSupportsErrorInfo(REFIID riid)
{
   try
   {
      static const IID* arr[] = 
      {
         &IID_IInterfaceTCPIPPort,
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
InterfaceTCPIPPort::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();

      HM::String error_message;
      if (HM::PersistentTCPIPPort::SaveObject(object_, error_message, HM::PersistenceModeNormal))
      {
         // Add to parent collection
         AddToParentCollection();

         return S_OK;
      }
   
      return COMError::GenerateError(error_message);
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::get_ID(long *pVal)
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

STDMETHODIMP InterfaceTCPIPPort::put_Protocol(eSessionType newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetProtocol((HM::SessionType) newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::get_Protocol(eSessionType *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (eSessionType) object_->GetProtocol();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::put_PortNumber(long newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetPortNumber(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::get_PortNumber(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetPortNumber();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::put_UseSSL(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (newVal)
         object_->SetConnectionSecurity(HM::CSSSL);
      else
         object_->SetConnectionSecurity(HM::CSNone);

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::get_UseSSL(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetConnectionSecurity() == HM::CSSSL ? VARIANT_TRUE : VARIANT_FALSE ;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::put_ConnectionSecurity(eConnectionSecurity newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetConnectionSecurity((HM::ConnectionSecurity) newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::get_ConnectionSecurity(eConnectionSecurity *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (eConnectionSecurity) object_->GetConnectionSecurity();

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::put_Address(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!object_->SetAddress(newVal))
         return COMError::GenerateError("Invalid IP address string.");
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::get_Address(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      HM::String sIPAddress = object_->GetAddressString();
      *pVal = sIPAddress.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::put_SSLCertificateID(long newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetSSLCertificateID(newVal);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::get_SSLCertificateID(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetSSLCertificateID();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPort::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!authentication_->GetIsServerAdmin())
         return authentication_->GetAccessDenied();
   
      if (!parent_collection_)
         return HM::PersistentTCPIPPort::DeleteObject(object_) ? S_OK : S_FALSE;
   
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

