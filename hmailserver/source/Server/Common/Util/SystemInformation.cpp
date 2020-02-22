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
   SystemInformation::OperatingSystem SystemInformation::operating_system_ = Unknown;

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
      if (operating_system_ == Unknown)
      {
         OSVERSIONINFO OSversion;

         OSversion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);

         // Disabling warning: "Deprecated. Use VerifyVersionInfo* or IsWindows* macros from VersionHelpers."
         #pragma warning(push) 
         #pragma warning(disable:4996)
         ::GetVersionEx(&OSversion);
         #pragma warning (pop)

         switch(OSversion.dwPlatformId)
         {
         case VER_PLATFORM_WIN32s: 
            operating_system_ = Windows3;
         case VER_PLATFORM_WIN32_WINDOWS:
            if(OSversion.dwMinorVersion==0)
               operating_system_ = Windows95;
            else if(OSversion.dwMinorVersion==10)  
               operating_system_ = Windows98;
            else if(OSversion.dwMinorVersion==90)  
               operating_system_ = Windows98;
            break;
         case VER_PLATFORM_WIN32_NT:
            if(OSversion.dwMajorVersion==5 && OSversion.dwMinorVersion==0)
               operating_system_ = Windows2000;
            else if(OSversion.dwMajorVersion==5 &&   OSversion.dwMinorVersion==1)
               operating_system_ = WindowsXP;
            else if(OSversion.dwMajorVersion<=4)
               operating_system_ = WindowsNT;
            else	
               //for unknown windows/newest windows version
               operating_system_ = Windows2003;
         }      
      }

      return operating_system_;
   }
}