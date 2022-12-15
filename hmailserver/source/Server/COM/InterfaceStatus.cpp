// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceStatus.h"

#include "../Common/Util/ServerStatus.h"

InterfaceStatus::InterfaceStatus() :
   status_(nullptr),
   application_(nullptr)
{

}


bool 
InterfaceStatus::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   status_ = HM::ServerStatus::Instance();
   application_ = HM::Application::Instance();

   return true;
}


STDMETHODIMP 
InterfaceStatus::get_UndeliveredMessages(BSTR *pVal)
{
   try
   {
      if (!status_)
         return GetAccessDenied();

      HM::String sRetVal = status_->GetUnsortedMessageStatus();
      *pVal = sRetVal.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceStatus::get_StartTime(BSTR *pVal)
{
   try
   {
      if (!status_)
         return GetAccessDenied();

      HM::String sRetVal = application_->GetStartTime();
      *pVal = sRetVal.AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceStatus::get_ProcessedMessages(long *pVal)
{
   try
   {
      if (!status_)
         return GetAccessDenied();

      *pVal = status_->GetNumberOfProcessedMessages();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceStatus::get_RemovedViruses(long *pVal)
{
   try
   {
      if (!status_)
         return GetAccessDenied();

      *pVal = status_->GetNumberOfRemovedViruses();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceStatus::get_RemovedSpamMessages(long *pVal)
{
   try
   {
      if (!status_)
         return GetAccessDenied();

      *pVal = status_->GetNumberOfDetectedSpamMessages();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceStatus::get_SessionCount(eSessionType iType, long *pVal)
{
   try
   {
      if (!status_)
         return GetAccessDenied();

      *pVal = status_->GetNumberOfSessions(iType);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceStatus::get_ThreadID(long* pVal)
{
   try
   {
      if (!status_)
         return GetAccessDenied();

      *pVal = status_->GetThreadID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

