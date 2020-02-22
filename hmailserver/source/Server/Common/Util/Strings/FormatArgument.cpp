// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "FormatArgument.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   FormatArgument::FormatArgument(const String &value) :
      type_(TypeUnicodeString),
      unicode_string_value_(value),
      numeric_value_(0),
      unsigned_numeric_value_(0),
      bool_value_(false)
   {
      
   }

   FormatArgument::FormatArgument(const AnsiString &value) :
      type_(TypeAnsiString),
      ansi_string_value_(value),
      numeric_value_(0),
      unsigned_numeric_value_(0),
      bool_value_(false)
   {

   }

   FormatArgument::FormatArgument(const char *value) :
      type_(TypeAnsiString),
      ansi_string_value_(value),
      numeric_value_(0),
      unsigned_numeric_value_(0),
      bool_value_(false)
   {

   }

   FormatArgument::FormatArgument(const wchar_t *value) :
      type_(TypeUnicodeString),
      unicode_string_value_(value),
      numeric_value_(0),
      unsigned_numeric_value_(0),
      bool_value_(false)
   {

   }

   FormatArgument::FormatArgument(const int &value) :
      type_(TypeNumber),
      numeric_value_(value),
      unsigned_numeric_value_(0),
      bool_value_(false)
   {

   }


   FormatArgument::FormatArgument(const unsigned int &value) :
      type_(TypeUnsignedNumber),
      numeric_value_(0),
      unsigned_numeric_value_(value),
      bool_value_(false)
   {

   }


   FormatArgument::FormatArgument(const __int64 &value) :
      type_(TypeNumber),
      numeric_value_(value),
      unsigned_numeric_value_(0),
      bool_value_(false)
   {

   }

   FormatArgument::FormatArgument(const unsigned __int64 &value) :
      type_(TypeUnsignedNumber),
      numeric_value_(0),
      unsigned_numeric_value_(value),
      bool_value_(false)
   {

   }
 
   FormatArgument::FormatArgument(const bool &value) :
      type_(TypeBoolean),
      numeric_value_(0),
      unsigned_numeric_value_(0),
      bool_value_(value)
   {

   }

   String 
   FormatArgument::GetValue()
   {
      String result;

      switch (type_)
      {
      case TypeUnicodeString:
         result = unicode_string_value_;
         break;
      case TypeAnsiString:
         result = String(ansi_string_value_);
         break;
      case TypeNumber:
         result.Format(_T("%I64d"), numeric_value_);
         break;
      case TypeUnsignedNumber:
         result.Format(_T("%I64u"), unsigned_numeric_value_);
         break;
      case TypeBoolean:
         if (bool_value_)
            result = "True";
         else
            result = "False";
         break;
      default:
         assert(0);
      }

      return result;
   }

}