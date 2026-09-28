// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "ARCAuthenticationResults.h"

#include "../DMARC/DMARCEvaluator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   std::vector<ARCAuthenticationResults::MethodResult>
   ARCAuthenticationResults::Parse(const AnsiString &results)
   {
      std::vector<MethodResult> methodResults;

      AnsiString value = RemoveComments_(results);
      value.Replace("\t", " ");
      value.Replace("\r", " ");
      value.Replace("\n", " ");

      std::vector<AnsiString> resinfos = Split_(value, ';');

      // The first element is the authserv-id.
      for (size_t i = 1; i < resinfos.size(); i++)
      {
         std::vector<AnsiString> tokens = Split_(resinfos[i], ' ');

         MethodResult methodResult;

         for (AnsiString token : tokens)
         {
            int equals = token.Find("=");
            if (equals <= 0)
               continue;

            AnsiString name = token.Mid(0, equals);
            AnsiString tokenValue = Unquote_(token.Mid(equals + 1));
            name.ToLower();

            if (methodResult.method.IsEmpty())
            {
               // method[/version]=result
               int slash = name.Find("/");
               methodResult.method = slash >= 0 ? name.Mid(0, slash) : name;
               methodResult.result = tokenValue;
               methodResult.result.ToLower();
            }
            else
            {
               methodResult.properties[name] = tokenValue;
            }
         }

         if (!methodResult.method.IsEmpty())
            methodResults.push_back(methodResult);
      }

      return methodResults;
   }

   bool
   ARCAuthenticationResults::ShowsAlignedPass(const AnsiString &results, const String &headerFromDomain,
                                              DMARCRecord::Alignment spfAlignment, DMARCRecord::Alignment dkimAlignment)
   {
      for (const MethodResult &methodResult : Parse(results))
      {
         if (methodResult.result != "pass")
            continue;

         auto property = [&methodResult](const AnsiString &name) -> AnsiString
         {
            auto iter = methodResult.properties.find(name);
            return iter != methodResult.properties.end() ? iter->second : AnsiString();
         };

         if (methodResult.method == "dmarc")
         {
            if (String(property("header.from")).CompareNoCase(headerFromDomain) == 0)
               return true;
         }
         else if (methodResult.method == "dkim")
         {
            AnsiString domain = property("header.d");

            if (domain.IsEmpty())
               domain = StringParser::ExtractDomain(property("header.i"));

            if (DMARCEvaluator::IsAligned(domain, headerFromDomain, dkimAlignment))
               return true;
         }
         else if (methodResult.method == "spf")
         {
            // smtp.mailfrom holds either the address or only its domain.
            AnsiString domain = StringParser::ExtractDomain(property("smtp.mailfrom"));

            if (DMARCEvaluator::IsAligned(domain, headerFromDomain, spfAlignment))
               return true;
         }
      }

      return false;
   }

   AnsiString
   ARCAuthenticationResults::RemoveComments_(const AnsiString &value)
   {
      AnsiString result;

      int depth = 0;
      bool quoted = false;
      bool escaped = false;

      for (int i = 0; i < value.GetLength(); i++)
      {
         char character = value.GetAt(i);

         if (escaped)
         {
            escaped = false;

            if (depth == 0)
               result += character;

            continue;
         }

         if (character == '\\')
         {
            escaped = true;

            if (depth == 0)
               result += character;

            continue;
         }

         if (depth == 0 && character == '"')
            quoted = !quoted;

         if (!quoted)
         {
            if (character == '(')
            {
               depth++;
               continue;
            }

            if (character == ')' && depth > 0)
            {
               depth--;

               // Keeps the words on either side of the comment apart.
               if (depth == 0)
                  result += ' ';

               continue;
            }
         }

         if (depth == 0)
            result += character;
      }

      return result;
   }

   std::vector<AnsiString>
   ARCAuthenticationResults::Split_(const AnsiString &value, char separator)
   {
      std::vector<AnsiString> parts;

      AnsiString current;
      bool quoted = false;
      bool escaped = false;

      for (int i = 0; i < value.GetLength(); i++)
      {
         char character = value.GetAt(i);

         if (escaped)
         {
            escaped = false;
            current += character;
            continue;
         }

         if (character == '\\')
         {
            escaped = true;
            current += character;
            continue;
         }

         if (character == '"')
            quoted = !quoted;

         if (character == separator && !quoted)
         {
            current.Trim();

            if (!current.IsEmpty() || separator == ';')
               parts.push_back(current);

            current = "";
            continue;
         }

         current += character;
      }

      current.Trim();

      if (!current.IsEmpty())
         parts.push_back(current);

      return parts;
   }

   AnsiString
   ARCAuthenticationResults::Unquote_(AnsiString value)
   {
      value.Trim();

      if (value.GetLength() < 2 || value.GetAt(0) != '"' || value.GetAt(value.GetLength() - 1) != '"')
         return value;

      AnsiString result;
      bool escaped = false;

      for (int i = 1; i < value.GetLength() - 1; i++)
      {
         char character = value.GetAt(i);

         if (!escaped && character == '\\')
         {
            escaped = true;
            continue;
         }

         escaped = false;
         result += character;
      }

      return result;
   }
}
