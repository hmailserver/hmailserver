// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceEventLog.h"


STDMETHODIMP 
InterfaceEventLog::Write(BSTR sMessage)
{
   try
   {
      HM::Logger::Instance()->LogEvent(sMessage);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


