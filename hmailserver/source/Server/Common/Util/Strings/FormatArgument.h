// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class FormatArgument
   {
   public:
      FormatArgument(const String &value);
      FormatArgument(const AnsiString &value);
      FormatArgument(const int &value);
      FormatArgument(const unsigned int &value);
      FormatArgument(const __int64 &value);
      FormatArgument(const unsigned __int64 &value);
      FormatArgument(const char *value);
      FormatArgument(const wchar_t *value);
      FormatArgument(const bool &value);
   
      String GetValue();
   
   private:

      enum Type
      {
         TypeUnicodeString = 1,
         TypeAnsiString = 2,
         TypeNumber = 3,
         TypeUnsignedNumber = 4,
         TypeBoolean = 5

      };

      String _unicodeStringValue;
      AnsiString _ansiStringValue;
      __int64 _numericValue;
      unsigned __int64 _unsignedNumericValue;
      bool _boolValue;

      Type _type;

   };
}