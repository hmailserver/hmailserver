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

   server_messages_ = HM::Configuration::Instance()->GetServerMessages();

   return true;
}


STDMETHODIMP 
InterfaceServerMessages::Refresh()
{
   try
   {
      if (!server_messages_)
         return GetAccessDenied();

      if (!server_messages_)
         return S_FALSE;
   
      server_messages_->Refresh();
   
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
      if (!server_messages_)
         return GetAccessDenied();

      *pVal = server_messages_->GetCount();
   
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
      if (!server_messages_)
         return GetAccessDenied();

      CComObject<InterfaceServerMessage>* pInterfaceServerMessage = new CComObject<InterfaceServerMessage>();
      pInterfaceServerMessage->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::ServerMessage> pSM = server_messages_->GetItem(Index);
   
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
      if (!server_messages_)
         return GetAccessDenied();

      CComObject<InterfaceServerMessage>* pInterfaceServerMessage = new CComObject<InterfaceServerMessage>();
      pInterfaceServerMessage->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::ServerMessage> pSM = server_messages_->GetItemByDBID(lDBID);
   
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
      if (!server_messages_)
         return GetAccessDenied();

      CComObject<InterfaceServerMessage>* pInterfaceServerMessage = new CComObject<InterfaceServerMessage>();
      pInterfaceServerMessage->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::ServerMessage> pSM = server_messages_->GetItemByName(sName);
   
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


