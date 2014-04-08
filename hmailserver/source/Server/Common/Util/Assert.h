// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Assert
   {
   public:
      Assert(void);
      ~Assert(void);

      static void IsTrue(bool argument);
      static void IsFalse(bool argument);
      static void AreEqual(const String &str1, const String &str2);

   private:

    
   };
}