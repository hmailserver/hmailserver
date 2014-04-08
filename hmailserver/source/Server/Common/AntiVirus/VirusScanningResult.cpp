// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "VirusScanningResult.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   

   VirusScanningResult::VirusScanningResult(ScanningResult result, const String &details) :
      m_scanningResult(result),
      m_details(details)
   {
      
   }

   VirusScanningResult::VirusScanningResult(const String &errorMessageSource, const String &errorMessage) :
      m_scanningResult(ErrorOccurred),
      m_details(errorMessage),
      m_errorMessageSource(errorMessageSource)
   {

   }
}
