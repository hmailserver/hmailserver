/* Copyright (c) Martin Knafve / The hMailServer Community Developers (HCD) hMailServer.com */

#include <stdafx.h>

#include "Assert.h"
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   void
   Assert::IsTrue(bool argument)
   {
      if (!argument)
         throw;
   }

   void
   Assert::IsFalse(bool argument)
   {
      if (argument)
         throw;
   }


   void
   Assert::AreEqual(const String &str1, const String &str2)
   {
      if (str1 != str2)
         throw;

   }


}