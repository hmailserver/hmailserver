// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceIncomingRelays.h"

#include "../Common/Persistence/PersistentIncomingRelay.h"
#include "InterfaceIncomingRelay.h"

void 
InterfaceIncomingRelays::Attach(shared_ptr<HM::IncomingRelays> incomingRelays)
{
   m_pIncomingRelays = incomingRelays;
}

STDMETHODIMP 
InterfaceIncomingRelays::get_Count(long *pVal)
{
   try
   {
      if (!m_pIncomingRelays)
         return GetAccessDenied();

      *pVal = m_pIncomingRelays->GetCount();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIncomingRelays::Delete(long Index)
{
   try
   {
      if (!m_pIncomingRelays)
         return GetAccessDenied();

      m_pIncomingRelays->DeleteItem(Index);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIncomingRelays::DeleteByDBID(long DBID)
{
   try
   {
      if (!m_pIncomingRelays)
         return GetAccessDenied();

      m_pIncomingRelays->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIncomingRelays::Refresh()
{
   try
   {
      if (!m_pIncomingRelays)
         return GetAccessDenied();

      m_pIncomingRelays->Refresh();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIncomingRelays::get_Item(long Index, IInterfaceIncomingRelay **pVal)
{
   try
   {
      if (!m_pIncomingRelays)
         return GetAccessDenied();

      CComObject<InterfaceIncomingRelay>* pRangeInt = new CComObject<InterfaceIncomingRelay>();
      pRangeInt->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IncomingRelay> pRange = m_pIncomingRelays->GetItem(Index);
   
      if (pRange)
      {
         pRangeInt->AttachItem(pRange);
         pRangeInt->AttachParent(m_pIncomingRelays, true);
         pRangeInt->AddRef();
         *pVal = pRangeInt;
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

STDMETHODIMP InterfaceIncomingRelays::get_ItemByDBID(long DBID, IInterfaceIncomingRelay **pVal)
{
   try
   {
      if (!m_pIncomingRelays)
         return GetAccessDenied();

      CComObject<InterfaceIncomingRelay>* pRangeInt = new CComObject<InterfaceIncomingRelay>();
      pRangeInt->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IncomingRelay> pRange = m_pIncomingRelays->GetItemByDBID(DBID);
   
      if (pRange)
      {
         pRangeInt->AttachItem(pRange);
         pRangeInt->AttachParent(m_pIncomingRelays, true);
         pRangeInt->AddRef();
         *pVal = pRangeInt;
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

STDMETHODIMP InterfaceIncomingRelays::Add(IInterfaceIncomingRelay **pVal)
{
   try
   {
      if (!m_pIncomingRelays)
         return GetAccessDenied();

      if (!m_pIncomingRelays)
         return m_pAuthentication->GetAccessDenied();
   
      CComObject<InterfaceIncomingRelay>* pInterfaceRange = new CComObject<InterfaceIncomingRelay>();
      pInterfaceRange->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IncomingRelay> pRange = shared_ptr<HM::IncomingRelay>(new HM::IncomingRelay); 
   
      pInterfaceRange->AttachItem(pRange);
      pInterfaceRange->AttachParent(m_pIncomingRelays, false);
   
      pInterfaceRange->AddRef();
      *pVal = pInterfaceRange;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIncomingRelays::get_ItemByName(BSTR sName, IInterfaceIncomingRelay **pVal)
{
   try
   {
      if (!m_pIncomingRelays)
         return GetAccessDenied();

      CComObject<InterfaceIncomingRelay>* pRangeInt = new CComObject<InterfaceIncomingRelay>();
      pRangeInt->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IncomingRelay> pRange = m_pIncomingRelays->GetItemByName(sName);
   
      if (pRange)
      {
         pRangeInt->AttachItem(pRange);
         pRangeInt->AttachParent(m_pIncomingRelays, true);
         pRangeInt->AddRef();
         *pVal = pRangeInt;
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


