// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceTCPIPPorts.h"


#include "..\Common\BO\TCPIPPort.h"
#include "InterfaceTCPIPPort.h"

void 
InterfaceTCPIPPorts::Attach(shared_ptr<HM::TCPIPPorts> pBA) 
{ 
   m_pTCPIPPorts = pBA; 
}

STDMETHODIMP 
InterfaceTCPIPPorts::Refresh()
{
   try
   {
      if (!m_pTCPIPPorts)
         return S_FALSE;
   
      m_pTCPIPPorts->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceTCPIPPorts::SetDefault()
{
   try
   {
      if (!m_pTCPIPPorts)
         return S_FALSE;
   
      m_pTCPIPPorts->SetDefault();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceTCPIPPorts::get_Count(long *pVal)
{
   try
   {
      if (!m_pTCPIPPorts)
         return GetAccessDenied();

      *pVal = m_pTCPIPPorts->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceTCPIPPorts::get_Item(long Index, IInterfaceTCPIPPort **pVal)
{
   try
   {
      if (!m_pTCPIPPorts)
         return GetAccessDenied();

      CComObject<InterfaceTCPIPPort>* pInterfaceTCPIPPort = new CComObject<InterfaceTCPIPPort>();
      pInterfaceTCPIPPort->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::TCPIPPort> pBA = m_pTCPIPPorts->GetItem(Index);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceTCPIPPort->AttachItem(pBA);
      pInterfaceTCPIPPort->AttachParent(m_pTCPIPPorts, true);
      pInterfaceTCPIPPort->AddRef();
      *pVal = pInterfaceTCPIPPort;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceTCPIPPorts::DeleteByDBID(long DBID)
{
   try
   {
      if (!m_pTCPIPPorts)
         return GetAccessDenied();

      m_pTCPIPPorts->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceTCPIPPorts::get_ItemByDBID(long lDBID, IInterfaceTCPIPPort **pVal)
{
   try
   {
      if (!m_pTCPIPPorts)
         return GetAccessDenied();

      CComObject<InterfaceTCPIPPort>* pInterfaceTCPIPPort = new CComObject<InterfaceTCPIPPort>();
      pInterfaceTCPIPPort->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::TCPIPPort> pBA = m_pTCPIPPorts->GetItemByDBID(lDBID);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceTCPIPPort->AttachItem(pBA);
      pInterfaceTCPIPPort->AttachParent(m_pTCPIPPorts, true);
      pInterfaceTCPIPPort->AddRef();
   
      *pVal = pInterfaceTCPIPPort;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceTCPIPPorts::Add(IInterfaceTCPIPPort **pVal)
{
   try
   {
      if (!m_pTCPIPPorts)
         return GetAccessDenied();

      if (!m_pTCPIPPorts)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceTCPIPPort>* pInterfaceTCPIPPort = new CComObject<InterfaceTCPIPPort>();
      pInterfaceTCPIPPort->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::TCPIPPort> pBA = shared_ptr<HM::TCPIPPort>(new HM::TCPIPPort);
   
      pInterfaceTCPIPPort->AttachItem(pBA);
      pInterfaceTCPIPPort->AttachParent(m_pTCPIPPorts, false);
   
      pInterfaceTCPIPPort->AddRef();
   
      *pVal = pInterfaceTCPIPPort;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


