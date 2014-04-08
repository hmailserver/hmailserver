// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "..\COM\InterfaceServerMessages.h"

#include "..\Common\BO\ServerMessage.h"
#include "InterfaceServerMessage.h"
#include "../common/Persistence/PersistentServerMessage.h"

bool 
InterfaceServerMessages::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   m_pServerMessages = HM::Configuration::Instance()->GetServerMessages();

   return true;
}


STDMETHODIMP 
InterfaceServerMessages::Refresh()
{
   try
   {
      if (!m_pServerMessages)
         return GetAccessDenied();

      if (!m_pServerMessages)
         return S_FALSE;
   
      m_pServerMessages->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceServerMessages::get_Count(long *pVal)
{
   try
   {
      if (!m_pServerMessages)
         return GetAccessDenied();

      *pVal = m_pServerMessages->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceServerMessages::get_Item(long Index, IInterfaceServerMessage **pVal)
{
   try
   {
      if (!m_pServerMessages)
         return GetAccessDenied();

      CComObject<InterfaceServerMessage>* pInterfaceServerMessage = new CComObject<InterfaceServerMessage>();
      pInterfaceServerMessage->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::ServerMessage> pSM = m_pServerMessages->GetItem(Index);
   
      if (!pSM)
         return DISP_E_BADINDEX;
   
      pInterfaceServerMessage->AttachItem(pSM);
      pInterfaceServerMessage->AddRef();
      *pVal = pInterfaceServerMessage;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceServerMessages::get_ItemByDBID(long lDBID, IInterfaceServerMessage **pVal)
{
   try
   {
      if (!m_pServerMessages)
         return GetAccessDenied();

      CComObject<InterfaceServerMessage>* pInterfaceServerMessage = new CComObject<InterfaceServerMessage>();
      pInterfaceServerMessage->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::ServerMessage> pSM = m_pServerMessages->GetItemByDBID(lDBID);
   
      if (!pSM)
         return DISP_E_BADINDEX;
   
      pInterfaceServerMessage->AttachItem(pSM);
      pInterfaceServerMessage->AddRef();
   
      *pVal = pInterfaceServerMessage;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceServerMessages::get_ItemByName(BSTR sName, IInterfaceServerMessage **pVal)
{
   try
   {
      if (!m_pServerMessages)
         return GetAccessDenied();

      CComObject<InterfaceServerMessage>* pInterfaceServerMessage = new CComObject<InterfaceServerMessage>();
      pInterfaceServerMessage->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::ServerMessage> pSM = m_pServerMessages->GetItemByName(sName);
   
      if (!pSM)
         return DISP_E_BADINDEX;
   
      pInterfaceServerMessage->AttachItem(pSM);
      pInterfaceServerMessage->AddRef();
   
      *pVal = pInterfaceServerMessage;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


