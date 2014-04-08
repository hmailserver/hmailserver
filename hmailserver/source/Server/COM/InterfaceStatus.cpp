// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceStatus.h"

#include "../Common/Util/ServerStatus.h"

bool 
InterfaceStatus::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   m_pStatus = HM::ServerStatus::Instance();
   m_pApplication = HM::Application::Instance();

   return true;
}


STDMETHODIMP 
InterfaceStatus::get_UndeliveredMessages(BSTR *pVal)
{
   try
   {
      if (!m_pStatus)
         return GetAccessDenied();

      HM::String sRetVal = m_pStatus->GetUnsortedMessageStatus();
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
      if (!m_pStatus)
         return GetAccessDenied();

      HM::String sRetVal = m_pApplication->GetStartTime();
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
      if (!m_pStatus)
         return GetAccessDenied();

      *pVal = m_pStatus->GetNumberOfProcessedMessages();
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
      if (!m_pStatus)
         return GetAccessDenied();

      *pVal = m_pStatus->GetNumberOfRemovedViruses();
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
      if (!m_pStatus)
         return GetAccessDenied();

      *pVal = m_pStatus->GetNumberOfDetectedSpamMessages();
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
      if (!m_pStatus)
         return GetAccessDenied();

      *pVal = m_pStatus->GetNumberOfSessions(iType);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


