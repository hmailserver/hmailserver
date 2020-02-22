// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceRoutes.h"

#include "InterfaceRoute.h"

#include "../SMTP/SMTPConfiguration.h"


bool 
InterfaceRoutes::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   routes_ = HM::Configuration::Instance()->GetSMTPConfiguration()->GetRoutes();

   return true;
}


STDMETHODIMP InterfaceRoutes::get_Count(long *pVal)
{
   try
   {
      if (!routes_)
         return GetAccessDenied();

      *pVal = routes_->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoutes::get_Item(long Index, IInterfaceRoute **pVal)
{
   try
   {
      if (!routes_)
         return GetAccessDenied();

      CComObject<InterfaceRoute>* pInterfaceRoute = new CComObject<InterfaceRoute>();
      pInterfaceRoute->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Route> pRoute = routes_->GetItem(Index);
   
      if (pRoute)
      {
         pInterfaceRoute->AttachItem(pRoute);
         pInterfaceRoute->AttachParent(routes_, true);
         pInterfaceRoute->AddRef();
         *pVal = pInterfaceRoute;
      }
      else
      {
         return DISP_E_BADINDEX;  
      }
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoutes::DeleteByDBID(long DBID)
{
   try
   {
      if (!routes_)
         return GetAccessDenied();

      routes_->DeleteItemByDBID(DBID);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceRoutes::Add(IInterfaceRoute **pVal)
{
   try
   {
      if (!routes_)
         return GetAccessDenied();

      if (!routes_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceRoute>* pRouteInterface = new CComObject<InterfaceRoute>();
      pRouteInterface->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Route> pRoute = std::shared_ptr<HM::Route>(new HM::Route);
   
      pRouteInterface->AttachItem(pRoute);
      pRouteInterface->AttachParent(routes_, false);
   
      pRouteInterface->AddRef();
      *pVal = pRouteInterface;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRoutes::get_ItemByName(BSTR ItemName, IInterfaceRoute **pVal)
{
   try
   {
      if (!routes_)
         return GetAccessDenied();
   
      CComObject<InterfaceRoute>* pRouteInterface = new CComObject<InterfaceRoute>();
      pRouteInterface->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Route> pRoute = routes_->GetItemByName(ItemName);
      if (!pRoute)
         return DISP_E_BADINDEX;
   
      pRouteInterface->AttachItem(pRoute);
      pRouteInterface->AttachParent(routes_, true);
   
      pRouteInterface->AddRef();
      *pVal = pRouteInterface;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRoutes::get_ItemByDBID(long lDBID, IInterfaceRoute **pVal)
{
   try
   {
      if (!routes_)
         return GetAccessDenied();

      CComObject<InterfaceRoute>* pRouteInterface = new CComObject<InterfaceRoute>();
      pRouteInterface->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Route> pRoute = routes_->GetItemByDBID(lDBID);
      if (!pRoute)
         return DISP_E_BADINDEX;
   
      pRouteInterface->AttachItem(pRoute);
      pRouteInterface->AttachParent(routes_, true);
   
      pRouteInterface->AddRef();
      *pVal = pRouteInterface;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRoutes::Refresh()
{
   try
   {
      if (!routes_)
         return GetAccessDenied();

      if (routes_)
         routes_->Refresh();
      else
         return S_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


