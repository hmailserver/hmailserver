// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "VirusScanningResult.h"

namespace HM
{
   class CustomVirusScanner
   {
   public:
      CustomVirusScanner(void);
      ~CustomVirusScanner(void);

      static VirusScanningResult Scan(const String &executablePath, int virusReturnCode, const String &sFilename);
      static VirusScanningResult Scan(const String &sFilename);
   };

}