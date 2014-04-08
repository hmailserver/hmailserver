// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../SpamTest.h"

namespace HM
{
   class DKIMParameters
   {
   public:
      
      void Load(const AnsiString &parameters);
      bool GetIsSet(const AnsiString &paramName) const;
      AnsiString GetValue(const AnsiString &paramName) const;

      

      unsigned int GetParamCount() const;

   private:

      map<AnsiString, AnsiString> _parameters;

      AnsiString  _DKIMQPDecode(AnsiString input) const;
   };

}