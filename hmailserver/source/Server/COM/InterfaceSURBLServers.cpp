// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"

#include "InterfaceSURBLServers.h"
#include "InterfaceSURBLServer.h"

#include "../Common/AntiSpam/AntiSpamConfiguration.h"

bool 
InterfaceSURBLServers::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   m_pSURBLServers = HM::Configuration::Instance()->GetAntiSpamConfiguration().GetSURBLServers();

   return true;
}

STDMETHODIMP 
InterfaceSURBLServers::Refresh()
{
   try
   {
      if (!m_pSURBLServers)
         return GetAccessDenied();

      m_pSURBLServers->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceSURBLServers::get_Count(long *pVal)
{
   try
   {
      if (!m_pSURBLServers)
         return GetAccessDenied();

      *pVal = m_pSURBLServers->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceSURBLServers::get_Item(long Index, IInterfaceSURBLServer **pVal)
{
   try
   {
      if (!m_pSURBLServers)
         return GetAccessDenied();

      CComObject<InterfaceSURBLServer>* pInterfaceSURBLServer = new CComObject<InterfaceSURBLServer>();
      pInterfaceSURBLServer->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::SURBLServer> pDNSBlackList = m_pSURBLServers->GetItem(Index);
   
      if (!pDNSBlackList)
         return DISP_E_BADINDEX;
   
      pInterfaceSURBLServer->AttachItem(pDNSBlackList);
      pInterfaceSURBLServer->AttachParent(m_pSURBLServers, true);
      pInterfaceSURBLServer->AddRef();
      *pVal = pInterfaceSURBLServer;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceSURBLServers::DeleteByDBID(long DBID)
{
   try
   {
      if (!m_pSURBLServers)
         return GetAccessDenied();

      m_pSURBLServers->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceSURBLServers::get_ItemByDBID(long lDBID, IInterfaceSURBLServer **pVal)
{
   try
   {
      if (!m_pSURBLServers)
         return GetAccessDenied();

      CComObject<InterfaceSURBLServer>* pInterfaceSURBLServer = new CComObject<InterfaceSURBLServer>();
      pInterfaceSURBLServer->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::SURBLServer> pDNSBlackList = m_pSURBLServers->GetItemByDBID(lDBID);
   
      if (!pDNSBlackList)
         return DISP_E_BADINDEX;
   
      pInterfaceSURBLServer->AttachItem(pDNSBlackList);
      pInterfaceSURBLServer->AttachParent(m_pSURBLServers, true);
      pInterfaceSURBLServer->AddRef();
   
      *pVal = pInterfaceSURBLServer;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceSURBLServers::Add(IInterfaceSURBLServer **pVal)
{
   try
   {
      if (!m_pSURBLServers)
         return GetAccessDenied();

      if (!m_pSURBLServers)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceSURBLServer>* pInterfaceSURBLServer = new CComObject<InterfaceSURBLServer>();
      pInterfaceSURBLServer->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::SURBLServer> pDNSBL = shared_ptr<HM::SURBLServer>(new HM::SURBLServer);
   
      pInterfaceSURBLServer->AttachItem(pDNSBL);
      pInterfaceSURBLServer->AttachParent(m_pSURBLServers, false);
   
      pInterfaceSURBLServer->AddRef();
   
      *pVal = pInterfaceSURBLServer;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceSURBLServers::get_ItemByDNSHost(BSTR ItemName, IInterfaceSURBLServer **pVal)
{
   try
   {
      if (!m_pSURBLServers)
         return GetAccessDenied();

      CComObject<InterfaceSURBLServer>* pInterfaceSURBLServer = new CComObject<InterfaceSURBLServer>();
      pInterfaceSURBLServer->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::SURBLServer> pDNSBL = m_pSURBLServers->GetItemByName(ItemName);
      if (!pDNSBL)
         return S_FALSE;
   
      pInterfaceSURBLServer->AttachItem(pDNSBL);
      pInterfaceSURBLServer->AttachParent(m_pSURBLServers, true);
      pInterfaceSURBLServer->AddRef();
   
      *pVal = pInterfaceSURBLServer;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


