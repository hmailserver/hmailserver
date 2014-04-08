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

   m_pRoutes = HM::Configuration::Instance()->GetSMTPConfiguration()->GetRoutes();

   return true;
}


STDMETHODIMP InterfaceRoutes::get_Count(long *pVal)
{
   try
   {
      if (!m_pRoutes)
         return GetAccessDenied();

      *pVal = m_pRoutes->GetCount();
   
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
      if (!m_pRoutes)
         return GetAccessDenied();

      CComObject<InterfaceRoute>* pInterfaceRoute = new CComObject<InterfaceRoute>();
      pInterfaceRoute->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Route> pRoute = m_pRoutes->GetItem(Index);
   
      if (pRoute)
      {
         pInterfaceRoute->AttachItem(pRoute);
         pInterfaceRoute->AttachParent(m_pRoutes, true);
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
      if (!m_pRoutes)
         return GetAccessDenied();

      m_pRoutes->DeleteItemByDBID(DBID);
   
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
      if (!m_pRoutes)
         return GetAccessDenied();

      if (!m_pRoutes)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceRoute>* pRouteInterface = new CComObject<InterfaceRoute>();
      pRouteInterface->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Route> pRoute = shared_ptr<HM::Route>(new HM::Route);
   
      pRouteInterface->AttachItem(pRoute);
      pRouteInterface->AttachParent(m_pRoutes, false);
   
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
      if (!m_pRoutes)
         return GetAccessDenied();
   
      CComObject<InterfaceRoute>* pRouteInterface = new CComObject<InterfaceRoute>();
      pRouteInterface->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Route> pRoute = m_pRoutes->GetItemByName(ItemName);
      if (!pRoute)
         return DISP_E_BADINDEX;
   
      pRouteInterface->AttachItem(pRoute);
      pRouteInterface->AttachParent(m_pRoutes, true);
   
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
      if (!m_pRoutes)
         return GetAccessDenied();

      CComObject<InterfaceRoute>* pRouteInterface = new CComObject<InterfaceRoute>();
      pRouteInterface->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Route> pRoute = m_pRoutes->GetItemByDBID(lDBID);
      if (!pRoute)
         return DISP_E_BADINDEX;
   
      pRouteInterface->AttachItem(pRoute);
      pRouteInterface->AttachParent(m_pRoutes, true);
   
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
      if (!m_pRoutes)
         return GetAccessDenied();

      if (m_pRoutes)
         m_pRoutes->Refresh();
      else
         return S_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


