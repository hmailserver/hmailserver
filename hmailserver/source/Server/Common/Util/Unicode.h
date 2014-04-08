// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Unicode  
   {
   public:
	   Unicode();
	   virtual ~Unicode();

      static AnsiString ToANSI(const String &sString);
      static bool WideToMultiByte(const String &sInput, AnsiString &sOutput);
      static bool MultiByteToWide(const AnsiString &sInput, String &sOutput);

      static unsigned char* CharMoveNext(unsigned char*, bool utf8);
   private:

   };


}