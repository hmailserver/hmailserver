// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "Formatter.h"
#include "FormatArgument.h"

#include "../Unicode.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String 
   Formatter::Format(const AnsiString &fmt, const FormatArgument &argument1)
   {
      std::list<FormatArgument> arguments;
      arguments.push_back(argument1);

      return Format(fmt, arguments);
   }

   String 
   Formatter::Format(const AnsiString &fmt, const FormatArgument &argument1, const FormatArgument &argument2)
   {
      std::list<FormatArgument> arguments;
      arguments.push_back(argument1);
      arguments.push_back(argument2);

      return Format(fmt, arguments);
   }


   String 
   Formatter::Format(const AnsiString &fmt, const FormatArgument &argument1, const FormatArgument &argument2, const FormatArgument &argument3)
   {
      std::list<FormatArgument> arguments;
      arguments.push_back(argument1);
      arguments.push_back(argument2);
      arguments.push_back(argument3);

      return Format(fmt, arguments);
   }



   String 
   Formatter::Format(const AnsiString &fmt, const FormatArgument &argument1, const FormatArgument &argument2,const FormatArgument &argument3, const FormatArgument &argument4)
   {
      std::list<FormatArgument> arguments;
      arguments.push_back(argument1);
      arguments.push_back(argument2);
      arguments.push_back(argument3);
      arguments.push_back(argument4);

      return Format(fmt, arguments);
   }

   String 
   Formatter::Format(const AnsiString &fmt, const FormatArgument &argument1, const FormatArgument &argument2,const FormatArgument &argument3, const FormatArgument &argument4, const FormatArgument &argument5)
   {
      std::list<FormatArgument> arguments;
      arguments.push_back(argument1);
      arguments.push_back(argument2);
      arguments.push_back(argument3);
      arguments.push_back(argument4);
      arguments.push_back(argument5);

      return Format(fmt, arguments);
   }

   AnsiString 
   Formatter::FormatAsAnsi(const AnsiString &fmt, const FormatArgument &argument1)
   {
       String value = Format(fmt, argument1);

       return Unicode::ToANSI(value);
   }

   AnsiString 
   Formatter::FormatAsAnsi(const AnsiString &fmt, const FormatArgument &argument1, const FormatArgument &argument2)
   {
      String value = Format(fmt, argument1, argument2);

      return Unicode::ToANSI(value);
   }


   AnsiString 
   Formatter::FormatAsAnsi(const AnsiString &fmt, const FormatArgument &argument1, const FormatArgument &argument2, const FormatArgument &argument3)
   {
      String value = Format(fmt, argument1, argument2, argument3);

      return Unicode::ToANSI(value);
   }



   AnsiString 
   Formatter::FormatAsAnsi(const AnsiString &fmt, const FormatArgument &argument1, const FormatArgument &argument2,const FormatArgument &argument3, const FormatArgument &argument4)
   {
      String value = Format(fmt, argument1, argument2, argument3, argument4);

      return Unicode::ToANSI(value);
   }

   AnsiString 
   Formatter::FormatAsAnsi(const AnsiString &fmt, const FormatArgument &argument1, const FormatArgument &argument2,const FormatArgument &argument3, const FormatArgument &argument4, const FormatArgument &argument5)
   {
      String value = Format(fmt, argument1, argument2, argument5);

      return Unicode::ToANSI(value);
   }


   String 
   Formatter::Format(const String &fmt, const std::list<FormatArgument> &arguments)
   {
      String result = fmt;

#ifdef _DEBUG
         
         /*
            Validate that the number of placeholders in the format matches
            the number of arguments.
         */

         std::set<int> placeholders = GetPlaceholders(fmt);

         if (arguments.size() != placeholders.size())
         {
            return "";
         }
#endif

     int placeholderIndex = 0;
     for(FormatArgument argument : arguments)
     {
        String s;
        s.Format(_T("{%d}"), placeholderIndex);
        
        result.Replace(s, argument.GetValue());

        placeholderIndex++;
     }

     return result;
   }
   

   std::set<int>
   Formatter::GetPlaceholders(const String &format)
   {
      std::set<int> result;

      int length = format.GetLength();
      int placeholderStartPos = -1;

      for (int pos = 0; pos < length; pos++)
      {
         wchar_t c = format[pos];

         if (c == '{')
            placeholderStartPos = pos;
         else if (c == '}')
         {
            if (placeholderStartPos == -1)
            {
               std::set<int> emptyResult;
               return emptyResult;
            }

            // extract the placeholder position.
            int placeholderLength = pos - placeholderStartPos-1;
            String val = format.Mid(placeholderStartPos+1, placeholderLength);

            int idx = _ttoi(val);

            if (result.find(idx) == result.end())
               result.insert(idx);

            placeholderStartPos = -1;
         }
      }

      if (placeholderStartPos != -1)
      {
         std::set<int> emptyResult;
         return emptyResult;
      }

      return result;
   }

   void
   FormatterTester::Test()
   {
      // Single argument
      if (Formatter::Format(_T("{0}"), 25) != _T("25"))
         throw;
      
      // Argument sorting.
      if (Formatter::Format(_T("{0} {1}"), 25, 100) != _T("25 100"))
         throw;

      if (Formatter::Format(_T("{1} {0}"), 25, 100) != _T("100 25"))
         throw;

      // Ansi String parameters.
      if (Formatter::Format(_T("{0} {1}"), 25, "Test") != _T("25 Test"))
         throw;

      if (Formatter::Format(_T("{0}, {1}"), "String 1", "String 2") != _T("String 1, String 2"))
         throw;

      // Explicit ansi strings.
      if (Formatter::Format(_T("{0} {1}"), 25, AnsiString("Test")) != _T("25 Test"))
         throw;

      if (Formatter::Format(_T("{0}, {1}"), AnsiString("String 1"), AnsiString("String 2")) != _T("String 1, String 2"))
         throw;

      // Unicode strings
      if (Formatter::Format(_T("{0} {1}"), 25, _T("Test")) != _T("25 Test"))
         throw;

      if (Formatter::Format(_T("{0}, {1}"), _T("String 1"), _T("String 2")) != _T("String 1, String 2"))
         throw;

      // Explicit unicode strings
      if (Formatter::Format(_T("{0} {1}"), 25, String("Test")) != _T("25 Test"))
         throw;

      if (Formatter::Format(_T("{0}, {1}"), String("String 1"), String("String 2")) != _T("String 1, String 2"))
         throw;

      // Invalid format strings. These are only tested in debug mode since
      // format string validation isn't made in release for performance reasons.
#ifdef DEBUG

      if (Formatter::Format(_T("{0"), "Test") != _T(""))
         throw;

      if (Formatter::Format(_T("}"), "Test") != _T(""))
         throw;

      if (Formatter::Format(_T("{0}, {1}"), "Test") != _T(""))
         throw;
#endif

      // Large numeric values.
      if (Formatter::Format(_T("{0}"), INT_MAX) != _T("2147483647"))
         throw;

      if (Formatter::Format(_T("{0}"), UINT_MAX) != _T("4294967295"))
         throw;

      // UINT64 max.
      if (Formatter::Format(_T("{0}"), 18446744073709551614) != _T("18446744073709551614"))
         throw;

      // INT64 max
      if (Formatter::Format(_T("{0}"), 9223372036854775807) != _T("9223372036854775807"))
         throw;

      // Max parameter count.
      if (Formatter::Format(_T("{0}, {1}, {2}, {3}"), "Test", 1, _T("Test"), 0xff) != _T("Test, 1, Test, 255"))
         throw;

      // Boolean
      if (Formatter::Format(_T("{0} {1}"), true, false) != _T("True False"))
         throw;

   }
}