// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\systeminformation.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SystemInformation::OperatingSystem SystemInformation::_operatingSystem = Unknown;

   SystemInformation::SystemInformation(void)
   {

   }

   SystemInformation::~SystemInformation(void)
   {
   }

   SystemInformation::OperatingSystem 
   SystemInformation::GetOperatingSystem()
   {
      // Initialize operating system version once:
      if (_operatingSystem == Unknown)
      {
         OSVERSIONINFO OSversion;

         OSversion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
         ::GetVersionEx(&OSversion);

         switch(OSversion.dwPlatformId)
         {
         case VER_PLATFORM_WIN32s: 
            _operatingSystem = Windows3;
         case VER_PLATFORM_WIN32_WINDOWS:
            if(OSversion.dwMinorVersion==0)
               _operatingSystem = Windows95;
            else if(OSversion.dwMinorVersion==10)  
               _operatingSystem = Windows98;
            else if(OSversion.dwMinorVersion==90)  
               _operatingSystem = Windows98;
            break;
         case VER_PLATFORM_WIN32_NT:
            if(OSversion.dwMajorVersion==5 && OSversion.dwMinorVersion==0)
               _operatingSystem = Windows2000;
            else if(OSversion.dwMajorVersion==5 &&   OSversion.dwMinorVersion==1)
               _operatingSystem = WindowsXP;
            else if(OSversion.dwMajorVersion<=4)
               _operatingSystem = WindowsNT;
            else	
               //for unknown windows/newest windows version
               _operatingSystem = Windows2003;
         }      
      }

      return _operatingSystem;
   }
}