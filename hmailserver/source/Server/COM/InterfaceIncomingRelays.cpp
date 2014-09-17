// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceIncomingRelays.h"

#include "../Common/Persistence/PersistentIncomingRelay.h"
#include "InterfaceIncomingRelay.h"

void 
InterfaceIncomingRelays::Attach(std::shared_ptr<HM::IncomingRelays> incomingRelays)
{
   incoming_relays_ = incomingRelays;
}

STDMETHODIMP 
InterfaceIncomingRelays::get_Count(long *pVal)
{
   try
   {
      if (!incoming_relays_)
         return GetAccessDenied();

      *pVal = incoming_relays_->GetCount();
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
      if (!incoming_relays_)
         return GetAccessDenied();

      incoming_relays_->DeleteItem(Index);
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
      if (!incoming_relays_)
         return GetAccessDenied();

      incoming_relays_->DeleteItemByDBID(DBID);
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
      if (!incoming_relays_)
         return GetAccessDenied();

      incoming_relays_->Refresh();
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
      if (!incoming_relays_)
         return GetAccessDenied();

      CComObject<InterfaceIncomingRelay>* pRangeInt = new CComObject<InterfaceIncomingRelay>();
      pRangeInt->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::IncomingRelay> pRange = incoming_relays_->GetItem(Index);
   
      if (pRange)
      {
         pRangeInt->AttachItem(pRange);
         pRangeInt->AttachParent(incoming_relays_, true);
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
      if (!incoming_relays_)
         return GetAccessDenied();

      CComObject<InterfaceIncomingRelay>* pRangeInt = new CComObject<InterfaceIncomingRelay>();
      pRangeInt->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::IncomingRelay> pRange = incoming_relays_->GetItemByDBID(DBID);
   
      if (pRange)
      {
         pRangeInt->AttachItem(pRange);
         pRangeInt->AttachParent(incoming_relays_, true);
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
      if (!incoming_relays_)
         return GetAccessDenied();

      if (!incoming_relays_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceIncomingRelay>* pInterfaceRange = new CComObject<InterfaceIncomingRelay>();
      pInterfaceRange->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::IncomingRelay> pRange = std::shared_ptr<HM::IncomingRelay>(new HM::IncomingRelay); 
   
      pInterfaceRange->AttachItem(pRange);
      pInterfaceRange->AttachParent(incoming_relays_, false);
   
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
      if (!incoming_relays_)
         return GetAccessDenied();

      CComObject<InterfaceIncomingRelay>* pRangeInt = new CComObject<InterfaceIncomingRelay>();
      pRangeInt->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::IncomingRelay> pRange = incoming_relays_->GetItemByName(sName);
   
      if (pRange)
      {
         pRangeInt->AttachItem(pRange);
         pRangeInt->AttachParent(incoming_relays_, true);
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


