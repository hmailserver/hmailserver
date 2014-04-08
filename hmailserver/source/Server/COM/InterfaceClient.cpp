// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "..\COM\InterfaceClient.h"

#include "../Common/Application/ClientInfo.h"

// InterfaceClient

void 
InterfaceClient::AttachItem(shared_ptr<HM::ClientInfo> pClientInfo)
{
   m_pClientInfo = pClientInfo;
}

STDMETHODIMP InterfaceClient::get_Port(long *pVal)
{
   try
   {
      *pVal = m_pClientInfo->GetPort();
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
      *pVal = m_pClientInfo->GetIPAddress().AllocSysString();
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
      *pVal = m_pClientInfo->GetUsername().AllocSysString();
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
      *pVal = m_pClientInfo->GetHELO().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

