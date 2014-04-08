// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


namespace HM
{
   class PasswordGenerator
   {
   public:
	   PasswordGenerator();
	   virtual ~PasswordGenerator();

      static String Generate();
      static bool IsStrongPassword(const String &sUsername, const String &sPassword);

   private:
      
      static int Seed;

   };

}

