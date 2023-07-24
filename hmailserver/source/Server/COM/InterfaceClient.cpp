// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "..\COM\InterfaceClient.h"

#include "../Common/Scripting/ClientInfo.h"

// InterfaceClient

void 
InterfaceClient::AttachItem(std::shared_ptr<HM::ClientInfo> pClientInfo)
{
   client_info_ = pClientInfo;
}

STDMETHODIMP InterfaceClient::get_Port(long *pVal)
{
   try
   {
      *pVal = client_info_->GetPort();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceClient::get_IPAddress(BSTR *pVal)
{
   try
   {
      *pVal = client_info_->GetIPAddress().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceClient::get_Username(BSTR *pVal)
{
   try
   {
      *pVal = client_info_->GetUsername().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceClient::get_HELO(BSTR *pVal)
{
   try
   {
      *pVal = client_info_->GetHELO().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceClient::get_Authenticated(VARIANT_BOOL *pVal)
{
   try
   {
      *pVal = client_info_->GetIsAuthenticated() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceClient::get_EncryptedConnection(VARIANT_BOOL* pVal)
{
   try
   {
      *pVal = client_info_->GetIsEncryptedConnection() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceClient::get_CipherVersion(BSTR* pVal)
{
   try
   {
      *pVal = client_info_->GetCipherVersion().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceClient::get_CipherName(BSTR* pVal)
{
   try
   {
      *pVal = client_info_->GetCipherName().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceClient::get_CipherBits(long* pVal)
{
   try
   {
      *pVal = client_info_->GetCipherBits();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceClient::get_SessionID(long* pVal)
{
   try
   {
      *pVal = client_info_->GetSessionID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}