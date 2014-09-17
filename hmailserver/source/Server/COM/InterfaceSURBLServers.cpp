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

   surbl_servers_ = HM::Configuration::Instance()->GetAntiSpamConfiguration().GetSURBLServers();

   return true;
}

STDMETHODIMP 
InterfaceSURBLServers::Refresh()
{
   try
   {
      if (!surbl_servers_)
         return GetAccessDenied();

      surbl_servers_->Refresh();
   
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
      if (!surbl_servers_)
         return GetAccessDenied();

      *pVal = surbl_servers_->GetCount();
   
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
      if (!surbl_servers_)
         return GetAccessDenied();

      CComObject<InterfaceSURBLServer>* pInterfaceSURBLServer = new CComObject<InterfaceSURBLServer>();
      pInterfaceSURBLServer->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::SURBLServer> pDNSBlackList = surbl_servers_->GetItem(Index);
   
      if (!pDNSBlackList)
         return DISP_E_BADINDEX;
   
      pInterfaceSURBLServer->AttachItem(pDNSBlackList);
      pInterfaceSURBLServer->AttachParent(surbl_servers_, true);
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
      if (!surbl_servers_)
         return GetAccessDenied();

      surbl_servers_->DeleteItemByDBID(DBID);
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
      if (!surbl_servers_)
         return GetAccessDenied();

      CComObject<InterfaceSURBLServer>* pInterfaceSURBLServer = new CComObject<InterfaceSURBLServer>();
      pInterfaceSURBLServer->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::SURBLServer> pDNSBlackList = surbl_servers_->GetItemByDBID(lDBID);
   
      if (!pDNSBlackList)
         return DISP_E_BADINDEX;
   
      pInterfaceSURBLServer->AttachItem(pDNSBlackList);
      pInterfaceSURBLServer->AttachParent(surbl_servers_, true);
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
      if (!surbl_servers_)
         return GetAccessDenied();

      if (!surbl_servers_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceSURBLServer>* pInterfaceSURBLServer = new CComObject<InterfaceSURBLServer>();
      pInterfaceSURBLServer->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::SURBLServer> pDNSBL = std::shared_ptr<HM::SURBLServer>(new HM::SURBLServer);
   
      pInterfaceSURBLServer->AttachItem(pDNSBL);
      pInterfaceSURBLServer->AttachParent(surbl_servers_, false);
   
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
      if (!surbl_servers_)
         return GetAccessDenied();

      CComObject<InterfaceSURBLServer>* pInterfaceSURBLServer = new CComObject<InterfaceSURBLServer>();
      pInterfaceSURBLServer->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::SURBLServer> pDNSBL = surbl_servers_->GetItemByName(ItemName);
      if (!pDNSBL)
         return S_FALSE;
   
      pInterfaceSURBLServer->AttachItem(pDNSBL);
      pInterfaceSURBLServer->AttachParent(surbl_servers_, true);
      pInterfaceSURBLServer->AddRef();
   
      *pVal = pInterfaceSURBLServer;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


