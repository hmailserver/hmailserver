// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceMessageIndexing.h"

#include "../Common/Persistence/PersistentMessage.h"
#include "../Common/Persistence/PersistentMessageMetaData.h"

#include "../Common/Application/MessageIndexer.h"

InterfaceMessageIndexing::InterfaceMessageIndexing() :
   config_(nullptr)
{

}

bool 
InterfaceMessageIndexing::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   config_ = HM::Configuration::Instance();

   return true;
}

STDMETHODIMP InterfaceMessageIndexing::get_Enabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetMessageIndexing() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessageIndexing::put_Enabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetMessageIndexing(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessageIndexing::get_TotalMessageCount(long *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return false;
   
      *pVal = HM::PersistentMessage::GetTotalMessageCountDelivered();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessageIndexing::get_TotalIndexedCount(long *pVal)
{
   try
   {
      if (!GetIsServerAdmin())
         return false;
   
      HM::PersistentMessageMetaData md;
      *pVal = md.GetTotalMessageCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessageIndexing::Clear()
{
   try
   {
      if (!GetIsServerAdmin())
         return false;
   
      HM::PersistentMessageMetaData md;
      
      md.Clear();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessageIndexing::Index()
{
   try
   {
      if (!GetIsServerAdmin())
         return false;
   
      HM::MessageIndexer::Instance()->IndexNow();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


