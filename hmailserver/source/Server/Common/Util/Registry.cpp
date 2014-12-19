// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Registry.h"



#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Registry::Registry(void)
   {
   }

   Registry::~Registry(void)
   {
   }

   bool
   Registry::GetStringValue(HKEY hive, String key, String valueName, String &value)
   {
     /* HKEY   hkey; 
      DWORD  dwDisposition; 
      LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software"),  
         0, NULL, 0, KEY_READ, NULL, &hkey, &dwDisposition); 
*/

      DWORD maxSize = 8096;

      HKEY   hkey;

      if (RegCreateKeyExW(hive, key, 0, NULL, 0, KEY_READ | KEY_WOW64_32KEY, NULL, &hkey, 0) != ERROR_SUCCESS)
      {
         int err = GetLastError();
         return false;
      }

      DWORD dwType = REG_SZ;
      bool success = 
         RegQueryValueEx(hkey, valueName.GetBuffer(), NULL, &dwType, (PBYTE) value.GetBuffer(maxSize), &maxSize) == ERROR_SUCCESS;

      RegCloseKey(hkey);

      value.ReleaseBuffer();

      if (dwType != REG_SZ || success == false)
         return false;

      return true;
   }



}