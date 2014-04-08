// Copyright (c) 2012 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "ADOInt64Helper.h"
#include "../Util/SystemInformation.h"

using namespace std;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ADO64Helper ::ADO64Helper ()
   {

   }

   void 
   ADO64Helper ::AddInt64Parameter(_CommandPtr &command, const String& parameterName, __int64 value)
   {
      // int64 variants are not supported in Windows 2000.
      if (SystemInformation::GetOperatingSystem() == SystemInformation::Windows2000)
      {
         String val = StringParser::IntToString(value);

         VARIANT stringType;
         stringType.vt = VT_BSTR;
         stringType.bstrVal  = _bstr_t(val);

         int length = 8000;

         command->Parameters->Append(command->CreateParameter(_bstr_t(parameterName),adWChar,adParamInput, length, stringType));
      }
      else
      {
         VARIANT int64Variant;
         int64Variant.vt = VT_I8;
         int64Variant.llVal = value;

         command->Parameters->Append(command->CreateParameter(_bstr_t(parameterName), adBigInt,adParamInput, sizeof(__int64), int64Variant));
      }
   }
}

