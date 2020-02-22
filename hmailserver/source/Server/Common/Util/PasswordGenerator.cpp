// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "PasswordGenerator.h"
#include "../Util/GUIDCreator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   int PasswordGenerator::Seed = 0;

   PasswordGenerator::PasswordGenerator()
   {
      
   }

   PasswordGenerator::~PasswordGenerator()
   {

   }

   String
   PasswordGenerator::Generate()
   {
      String sGUID = GUIDCreator::GetGUID();

      String sOutString;

      for (int i = 0; i < sGUID.GetLength(); i++)
      {
         wchar_t c = sGUID[i];

         switch (c)
         {
         case '{':
         case '}':
         case '-':
            break;
         default:
            sOutString += c;
         }

      }

      String sRetVal = sOutString.Mid(0,12);
      return sRetVal;
   }

   bool 
   PasswordGenerator::IsStrongPassword(const String &sUsername, const String &sPassword)
   {

      // The password should not be a part of a user name.
      if (sUsername.FindNoCase(sPassword) >= 0)
         return false;

      // The password should be more than 4 characters long.
      if (sPassword.GetLength() <= 4)
         return false;
     
      // If the password is 6 characters or less, it should contain a number or special char
      if (sPassword.GetLength() <= 6)
      {
         if (sPassword.FindOneOf(_T("01234567890!\"#�%&/()=?^*_:;><,.-'��+")) < 0)
            return false;
      }

      if (sPassword.CompareNoCase(_T("secret")) == 0) return false;
      if (sPassword.CompareNoCase(_T("password")) == 0) return false;
      if (sPassword.CompareNoCase(_T("info")) == 0) return false;
      if (sPassword.CompareNoCase(_T("webmaster")) == 0) return false;
      if (sPassword.CompareNoCase(_T("admin")) == 0) return false;
      if (sPassword.CompareNoCase(_T("12345")) == 0) return false;
      if (sPassword.CompareNoCase(_T("qwerty")) == 0) return false;

      return true;
   }

}
