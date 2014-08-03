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

      String unicode_string_value_;
      AnsiString ansi_string_value_;
      __int64 numeric_value_;
      unsigned __int64 unsigned_numeric_value_;
      bool bool_value_;

      Type type_;

   };
}