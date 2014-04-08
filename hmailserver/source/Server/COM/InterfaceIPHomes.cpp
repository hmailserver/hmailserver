// InterfaceIPHomes.cpp : Implementation of InterfaceIPHomes

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceIPHomes.h"

#include "../Common/Persistence/PersistentIPHome.h"
#include "../Common/BO/IPHome.h"

#include "InterfaceIPHome.h"

#include "../Common/Cache/SingleObjectCache.h"

bool 
InterfaceIPHomes::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   m_pIPHomes = HM::SingleObjectCache<HM::IPHomes>::Instance()->GetObject();

   return true;
}

STDMETHODIMP 
InterfaceIPHomes::get_Count(long *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pIPHomes->GetCount();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIPHomes::DeleteByDBID(long DBID)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pIPHomes->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIPHomes::Refresh()
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      m_pIPHomes->Refresh();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIPHomes::get_Item(long Index, IInterfaceIPHome **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceIPHome>* pIPHomeInt = new CComObject<InterfaceIPHome>();
      pIPHomeInt->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IPHome> pIPHome = m_pIPHomes->GetItem(Index);
   
      if (!pIPHome)
         return DISP_E_BADINDEX;  
   
      pIPHomeInt->AttachItem(pIPHome);
      pIPHomeInt->AddRef();
      *pVal = pIPHomeInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIPHomes::get_ItemByDBID(long DBID, IInterfaceIPHome **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceIPHome>* pIPHomeInt = new CComObject<InterfaceIPHome>();
      pIPHomeInt->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IPHome> pIPHome = m_pIPHomes->GetItemByDBID(DBID);
   
      if (!pIPHome)
         return DISP_E_BADINDEX;  
   
      pIPHomeInt->AttachItem(pIPHome);
      pIPHomeInt->AddRef();
      *pVal = pIPHomeInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIPHomes::Add(IInterfaceIPHome **pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pIPHomes)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceIPHome>* pInterfaceIPHome = new CComObject<InterfaceIPHome>();
      pInterfaceIPHome->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IPHome> pIPHome = shared_ptr<HM::IPHome>(new HM::IPHome); 
   
      pInterfaceIPHome->AttachItem(pIPHome);
      pInterfaceIPHome->AttachParent(m_pIPHomes, false);
      pInterfaceIPHome->AddRef();
   
      *pVal = pInterfaceIPHome;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


