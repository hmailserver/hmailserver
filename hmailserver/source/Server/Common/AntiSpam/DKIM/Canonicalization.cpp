// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Canonicalization.h"
#include "../../MIME/MimeCode.h"
#include "../../MIME/Mime.h"
#include "..\../Util\Parsing\StringParser.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   AnsiString 
   Canonicalization::GetDKIMWithoutSignature_(AnsiString value)
   {
      // locate the b= tag
      int pos = value.Find("b=");
      if (pos < 0)
         return "";

      // Locate end of b-tag. We need to use the ;-separator
      // here. The signature may contain spaces and newlines
      // if it's folded (and canon-mode is set to simple).
      int end = value.Find(";", pos);

      int actualEnd = 0;
      if (end > pos)
         actualEnd = end;
      else
         actualEnd = value.GetLength();

      int len = actualEnd - pos;

      AnsiString before = value.Mid(0, pos+2);
      AnsiString after = value.Mid(actualEnd);

      AnsiString result = before + after;

      return result;
   }


   AnsiString
   RelaxedCanonicalization::CanonicalizeBody(AnsiString value)
   {
      /*
         Ignores all whitespace at the end of lines.  Implementations MUST
         NOT remove the CRLF at the end of the line.

         Reduces all sequences of WSP within a line to a single SP
         character.

         Ignores all empty lines at the end of the message body.  "Empty
         line" is defined in Section 3.4.3.
      */
      
      std::vector<AnsiString> lines = StringParser::SplitString(value, "\r\n");

      std::vector<String> output;
      std::vector<String> cleanedLines;
      for (AnsiString line : lines)
      {
         line.Replace("\t", " ");

         //   o  Reduces all sequences of WSP within a line to a single SP character.
         while (line.Find("  ") >= 0)
            line.Replace("  ", " ");

         line.TrimRight();

         cleanedLines.push_back(line);

         if (!line.IsEmpty())
         {
            for (String cleaned : cleanedLines)
            {
               output.push_back(cleaned);
            }
            cleanedLines.clear();
         }
      }

      value = StringParser::JoinVector(output, "\r\n");

      if (output.size() > 0)
      {
         // The last line should have a linefeed. JoinVector above won't
         // add that...
         value += "\r\n";
      }

      return value;
   }

   AnsiString 
   RelaxedCanonicalization::CanonicalizeHeader(AnsiString header, const std::pair<AnsiString, AnsiString> &signatureField, const std::vector<AnsiString> &fieldsToInclude, AnsiString &fieldList)
   {
      MimeHeader mimeHeader;
      mimeHeader.Load(header, header.GetLength(), true);

      std::vector<MimeField> fields = mimeHeader.Fields();

      String result;
      for(AnsiString headerField : fieldsToInclude)
      {  
         headerField.Trim();

         // locate this field, starting from the end.
         String value;
         for (size_t i = fields.size(); i > 0; i--)
         {
            size_t fieldIndex = i - 1;
            MimeField field = fields[fieldIndex];

            if (headerField.CompareNoCase(field.GetName()) == 0)
            {
               // found
               fields.erase(fields.begin() + fieldIndex);

               value = field.GetValue();
               // Fix for DKIM Header verification failing on empty header value, for example: subject header
               if (value.GetLength() == 0)
               {
                  value += "\r\n";
               }
               break;
            }
         }

         if (value.GetLength() > 0)
         {
            /* 
            Convert all header field names (not the header field values) to
            lowercase.  For example, convert "SUBJect: AbC" to "subject: AbC".
            Delete any WSP characters remaining before and after the colon
            separating the header field name from the header field value.  The
            colon separator MUST be retained.
            */

            String relaxedFieldValue;
            String relaxedHeaderName = CanonicalizeHeaderName(headerField);
            relaxedFieldValue = CanonicalizeHeaderValue(value);

            result += relaxedHeaderName + ":" + relaxedFieldValue + "\r\n";

            if (!fieldList.IsEmpty())
               fieldList += ":";

            fieldList += headerField;
         }
      }

      if (!signatureField.first.IsEmpty())
      {
         // Don't pick the value from the actual header, use the header we're verifying instead
         // If there are more than one DKIM-signature fields in the header, this will be important.
         AnsiString  relaxedHeaderName = CanonicalizeHeaderName(signatureField.first);
         AnsiString relaxedFieldValue = CanonicalizeHeaderValue(signatureField.second);

         //and without a trailing CRLF.
         result += relaxedHeaderName + ":" + GetDKIMWithoutSignature_(relaxedFieldValue);
      }

      return result;
   }     


   AnsiString 
   RelaxedCanonicalization::CanonicalizeHeaderName(AnsiString name)
   {
      /*
      Unfold all header field continuation lines as described in
      [RFC2822]; in particular, lines with terminators embedded in
      continued header field values (that is, CRLF sequences followed by
      WSP) MUST be interpreted without the CRLF.  Implementations MUST
      NOT remove the CRLF at the end of the header field value.
      */

      name.ToLower();
      name.Trim();

      return name;
   }


   AnsiString 
   RelaxedCanonicalization::CanonicalizeHeaderValue(AnsiString value)
   {
      /*
      Unfold all header field continuation lines as described in
      [RFC2822]; in particular, lines with terminators embedded in
      continued header field values (that is, CRLF sequences followed by
      WSP) MUST be interpreted without the CRLF.  Implementations MUST
      NOT remove the CRLF at the end of the header field value.
      */

      value.Replace("\r\n ", " ");
      value.Replace("\r\n\t", " ");

      /*
      Convert all sequences of one or more WSP characters to a single SP
      character.  WSP characters here include those before and after a
      line folding boundary.
      */

      value.Replace("\t ", " ");
      value.Replace(" \t", " ");
      
      while (value.Find("  ") >= 0)
         value.Replace("  ", " ");

      /*
      Delete all WSP characters at the end of each unfolded header field value.
      */

      while (value.EndsWith(" ") || value.EndsWith("\t"))
         value = value.Mid(0, value.GetLength() -1);

      /* Delete any WSP characters remaining before and after the colon
      separating the header field name from the header field value.  The
      colon separator MUST be retained.
      */
      value.Trim();


      return value;
   }


   AnsiString 
   SimpleCanonicalization::CanonicalizeBody(AnsiString value)
   {
      // remove all empty lines.
      while (value.EndsWith("\r\n"))
         value = value.Mid(0, value.GetLength()-2);
      
      value += "\r\n";
      
      return value;
   }

   AnsiString 
   SimpleCanonicalization::CanonicalizeHeader(AnsiString header, const std::pair<AnsiString, AnsiString> &signatureField, const std::vector<AnsiString> &fieldsToInclude, AnsiString &fieldList)
   {
      // first build a formatted list of header lines.
      std::vector<AnsiString> formattedHeaderLines;

      AnsiString result;
      std::vector<AnsiString> headerLines = StringParser::SplitString(header, "\r\n");

      AnsiString foldedLines;
      for (size_t i = headerLines.size(); i > 0; i--)
      {
         AnsiString line = headerLines[i-1];

         if (line.StartsWith(" ") || line.StartsWith("\t"))
         {
            // line is folded. append to next.
            foldedLines = line + "\r\n" + foldedLines;
         }
         else
         {
            // we have a line!
            int colonPos = line.Find(":");
            if (colonPos < 0)
            {
               assert(0); // broken header.
               continue;
            }

            AnsiString entireHeaderField = line + "\r\n" + foldedLines;

            formattedHeaderLines.push_back(entireHeaderField);

            foldedLines = "";
         }
      }

      for(AnsiString fieldToInclude : fieldsToInclude)
      {
         fieldToInclude.Trim();

         // locate the header line.
         auto iter = formattedHeaderLines.begin();
         auto iterEnd = formattedHeaderLines.end();
         
         for (; iter != iterEnd; iter++)
         {
            AnsiString headerLine = (*iter);

            int colonPos = headerLine.Find(":");
            AnsiString headerName = headerLine.Mid(0, colonPos);

            if (headerName.CompareNoCase(fieldToInclude) == 0) 
            {
               result += headerLine;

               if (!fieldList.IsEmpty())
                  fieldList += ":";

               fieldList += headerName;

               formattedHeaderLines.erase(iter);
               break;
            }
         }

      }

      if (!signatureField.first.IsEmpty())
      {
         // Don't pick the value from the actual header, use the header we're verifying instead
         // If there are more than one DKIM-signature fields in the header, this will be important.
         AnsiString headerName = signatureField.first;

         AnsiString headerLine = headerName + ": " + GetDKIMWithoutSignature_(signatureField.second);

         if (headerLine.EndsWith("\r\n"))
            headerLine = headerLine.Mid(0, headerLine.GetLength()-2);

         result += headerLine;
      }


      return result;
   }



}