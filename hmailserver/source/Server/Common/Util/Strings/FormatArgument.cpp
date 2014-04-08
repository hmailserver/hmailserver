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
      _type(TypeUnicodeString),
      _unicodeStringValue(value),
      _numericValue(0),
      _unsignedNumericValue(0),
      _boolValue(false)
   {
      
   }

   FormatArgument::FormatArgument(const AnsiString &value) :
      _type(TypeAnsiString),
      _ansiStringValue(value),
      _numericValue(0),
      _unsignedNumericValue(0),
      _boolValue(false)
   {

   }

   FormatArgument::FormatArgument(const char *value) :
      _type(TypeAnsiString),
      _ansiStringValue(value),
      _numericValue(0),
      _unsignedNumericValue(0),
      _boolValue(false)
   {

   }

   FormatArgument::FormatArgument(const wchar_t *value) :
      _type(TypeUnicodeString),
      _unicodeStringValue(value),
      _numericValue(0),
      _unsignedNumericValue(0),
      _boolValue(false)
   {

   }

   FormatArgument::FormatArgument(const int &value) :
      _type(TypeNumber),
      _numericValue(value),
      _unsignedNumericValue(0),
      _boolValue(false)
   {

   }


   FormatArgument::FormatArgument(const unsigned int &value) :
      _type(TypeUnsignedNumber),
      _numericValue(0),
      _unsignedNumericValue(value),
      _boolValue(false)
   {

   }


   FormatArgument::FormatArgument(const __int64 &value) :
      _type(TypeNumber),
      _numericValue(value),
      _unsignedNumericValue(0),
      _boolValue(false)
   {

   }

   FormatArgument::FormatArgument(const unsigned __int64 &value) :
      _type(TypeUnsignedNumber),
      _numericValue(0),
      _unsignedNumericValue(value),
      _boolValue(false)
   {

   }
 
   FormatArgument::FormatArgument(const bool &value) :
      _type(TypeBoolean),
      _numericValue(0),
      _unsignedNumericValue(0),
      _boolValue(value)
   {

   }

   String 
   FormatArgument::GetValue()
   {
      String result;

      switch (_type)
      {
      case TypeUnicodeString:
         result = _unicodeStringValue;
         break;
      case TypeAnsiString:
         result = String(_ansiStringValue);
         break;
      case TypeNumber:
         result.Format(_T("%I64d"), _numericValue);
         break;
      case TypeUnsignedNumber:
         result.Format(_T("%I64u"), _unsignedNumericValue);
         break;
      case TypeBoolean:
         if (_boolValue)
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