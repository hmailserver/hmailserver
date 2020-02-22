// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class FormatArgument;
   class StringFormat;

   class Formatter
   {
   public:
      static String Format(const AnsiString &fmt, const FormatArgument &argument1);
      static String Format(const AnsiString &fmt, const FormatArgument &argument1, const FormatArgument &argument2);
      static String Format(const AnsiString &fmt, const FormatArgument &argument1, 
                                              const FormatArgument &argument2,
                                              const FormatArgument &argument3);
      static String Format(const AnsiString &fmt, const FormatArgument &argument1, 
                                              const FormatArgument &argument2,
                                              const FormatArgument &argument3,
                                              const FormatArgument &argument4);
      static String Format(const AnsiString &fmt, const FormatArgument &argument1, 
                                             const FormatArgument &argument2,
                                             const FormatArgument &argument3,
                                             const FormatArgument &argument4,
                                             const FormatArgument &argument5);

      static AnsiString FormatAsAnsi(const AnsiString &fmt, const FormatArgument &argument1);
      static AnsiString FormatAsAnsi(const AnsiString &fmt, const FormatArgument &argument1, const FormatArgument &argument2);
      static AnsiString FormatAsAnsi(const AnsiString &fmt, const FormatArgument &argument1, 
         const FormatArgument &argument2,
         const FormatArgument &argument3);
      static AnsiString FormatAsAnsi(const AnsiString &fmt, const FormatArgument &argument1, 
         const FormatArgument &argument2,
         const FormatArgument &argument3,
         const FormatArgument &argument4);
      static AnsiString FormatAsAnsi(const AnsiString &fmt, const FormatArgument &argument1, 
         const FormatArgument &argument2,
         const FormatArgument &argument3,
         const FormatArgument &argument4,
         const FormatArgument &argument5);
   private:

      static std::set<int> GetPlaceholders(const String &fmt);

      static String Format(const String &fmt, const std::list<FormatArgument> &arguments);
   };

   class FormatterTester
   {
   public:
      void Test();
   };

}