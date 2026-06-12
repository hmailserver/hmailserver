/*
Copyright (c) 2003, Jeff Lee
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

//
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MimeCode.h"
#include "MimeChar.h"
#include "Mime.h"
#include <stdlib.h>
#include <time.h>

#include "../Util/ByteBuffer.h"
#include "../Util/File.h"
#include "../Util/Charset.h"

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
   // search for a character in the current line (before CRLF)
   static const char* LineFind(const char* pszString, int ch)
   {
      ASSERT(pszString != NULL);
      while (*pszString != 0 && *pszString != ch && *pszString != '\r' && *pszString != '\n')
         pszString++;
      return *pszString == ch ? pszString : NULL;
   }

   // search for string2 in string1 (strstr)
   const char* FindString(const char* haystack,
      const char* needle,
      const char* haystackEnd) // one past last valid char
   {
      if (haystack == NULL || needle == NULL || haystackEnd == NULL)
         return NULL;

      const size_t needleLength = std::strlen(needle);
      if (needleLength == 0)
         return haystack;

      if (haystackEnd <= haystack)
         return NULL;

      const size_t haystackLength = static_cast<size_t>(haystackEnd - haystack);
      if (needleLength > haystackLength)
         return NULL;

      const char* const lastStart = haystack + (haystackLength - needleLength);
      while (haystack <= lastStart)
      {
         if (std::memcmp(haystack, needle, needleLength) == 0)
            return haystack;
         ++haystack;
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////
   // MimeField class - Represents a field of a MIME body part header
   //////////////////////////////////////////////////////////////////////

   void MimeField::GetValue(string& strValue) const
   {
      string::size_type nEnd = value_.find(';');
      if (nEnd != string::npos)
      {
         while (nEnd > 0 && CMimeChar::IsSpace((unsigned char)value_[nEnd-1]))
            nEnd--;
         strValue.assign(value_.c_str(), nEnd);
      }
      else
         strValue = value_;
   }

   // set a parameter (attribute=value) of the field
   void MimeField::SetParameter(const char* pszAttr, const char* pszValue)
   {
      modified_ = true;
      int nSize = pszValue ? (int)::strlen(pszValue) : 0;
      string strValue;
      strValue.reserve(nSize+3);
      
      if (*pszValue != '"')
         strValue = "\"";
      
      strValue += pszValue;

      if (nSize < 2 || pszValue[nSize-1] != '"')
         strValue += "\"";

      bool encodedParameter;
      int nPos;
      if (!FindParameter(pszAttr, nPos, nSize, encodedParameter))	// add new parameter
      {
         value_.reserve(value_.size() + ::strlen(pszAttr) + strValue.size() + 5);
         //if (MimeEnvironment::AutoFolding())
         //	value_ += ";\r\n\t";
         //else
         //	value_ += "; ";
         value_ += "; ";
         value_ += pszAttr;
         value_ += '=';
         value_ += strValue;
      }
      else							// update existing parameter
         value_.replace(nPos, nSize, strValue);
   }

   // get the value of a parameter
   bool MimeField::GetParameter(const char* pszAttr, AnsiString& strValue) const
   {
      strValue = "";

      bool encodedParameter = false;

      // Split on ';' while respecting quoted strings, so that parameter values
      // which legally contain semicolons (e.g. filename="semi;colon.dll") are
      // not broken across segments.
      std::vector<AnsiString> parameters;
      {
         AnsiString current;
         bool inQuote = false;
         for (size_t i = 0; i < value_.size(); i++)
         {
            char c = value_[i];
            if (c == '"') inQuote = !inQuote;
            if (!inQuote && c == ';')
            {
               parameters.push_back(current);
               current = "";
            }
            else
               current += c;
         }
         parameters.push_back(current);
      }

      for (unsigned int i = 1; i < parameters.size(); i++)
      {
         AnsiString value = parameters[i];
         value.TrimLeft();

         // Locate end of parameter name.
         int nameEndPos = 0;
         for (nameEndPos = 0; nameEndPos < value.GetLength(); nameEndPos++)
         {
            char c = value[nameEndPos];

            if (c == ' ' || c == '*' || c == '=')
               break;
         }

         // If we haven't found any value for this parameter, bail out.
         if (nameEndPos == 0 && strValue.IsEmpty())
            return false;

         AnsiString parameterName = value.Mid(0, nameEndPos);

         if (parameterName.CompareNoCase(pszAttr) != 0)
            continue;

         // Locate start of parameter value.
         int valuePos = 0;
         for (valuePos = nameEndPos; valuePos < value.GetLength(); valuePos++)
         {
            char c = value[valuePos];

            if (c == '=')
               break;
         }

         // We want the char before = NOT char after param name
         // to detect encoding per RFC 2231 4.1
         // http://www.hmailserver.com/forum/viewtopic.php?f=10&t=21417
         char characterBeforeEquals = value[valuePos - 1];

         if (characterBeforeEquals == '*')
            encodedParameter = true;

         // Skip past the equal sign.
         valuePos++;

         // Locate the start of the actual value. May be enclosed with quotes.
         // Track whether it is quoted so we can find the correct closing delimiter.
         //
         // For instance, this is perfectly valid
         // Content-Type: text/plain; charset = "iso-8859-1"
         //
         bool isQuoted = false;
         for (; valuePos < value.GetLength(); valuePos++)
         {
            char c = value[valuePos];

            if (c == ' ')
               continue;
            else if (c == '"') { isQuoted = true; valuePos++; break; }
            else break;
         }

         // Locate the end of the value.
         // For quoted values scan to the closing '"'; for unquoted values scan
         // to the next ';' or '"'.  This ensures semicolons inside quoted values
         // (e.g. filename="semi;colon.dll") are included in the result.
         int valueEndPos = valuePos;
         for (; valueEndPos < value.GetLength(); valueEndPos++)
         {
            char c = value[valueEndPos];

            if (isQuoted)
            {
               if (c == '"') break;
            }
            else
            {
               if (c == ';' || c == '"') break;
            }
         }

         int valueLength = valueEndPos - valuePos;

         value = value.Mid(valuePos, valueLength);

         // If the value is
         //    Content-Type: text/plain; charset = "iso-8859-1"
         // it needs to be trimmed.
         value.TrimRight();

         strValue.append(value);
      }

      if (strValue.IsEmpty())
         return false;

      /*
      (2)   MIME headers, like the RFC 822 headers they often
      appear in, are limited to 7bit US-ASCII, and the
      encoded-word mechanisms of RFC 2047 are not available
      to parameter values.  This makes it impossible to have
      parameter values in character sets other than US-ASCII
      without specifying some sort of private per-parameter
      encoding.

      http://tools.ietf.org/html/draft-freed-pvcsc-03

      Examples:
      Input: ISO-8859-1''%E9%2E%70%6E%67
      Output: =?ISO-8859-1?Q?=E9=2Epng?=
      */

      if (encodedParameter)
      {
         MimeParameterRFC2184Decoder decoder;
         strValue.assign(decoder.Decode(strValue));
      }

      strValue.TrimLeft();

      return true;
   }

   // Remove all parameters whose base name matches pszAttr, including RFC 2231
   // continuation parameters (filename*0, filename*1, ...) and encoded variants (filename*).
   void MimeField::RemoveParameter(const char* pszAttr)
   {
      bool encodedParameter;
      int nPos, nSize;
      bool changed = false;

      while (FindParameter(pszAttr, nPos, nSize, encodedParameter))
      {
         // nPos points to the value start (right after '=').
         // Walk backwards to find the ';' that begins this parameter segment.
         // rfind is safe here: any ';' inside a preceding quoted value is at a position
         // less than that value's opening '"', which is itself less than nPos.
         string::size_type segStart = value_.rfind(';', nPos);
         if (segStart == string::npos)
            break;

         size_t segEnd = nPos + nSize;

         value_.erase(segStart, segEnd - segStart);
         changed = true;
      }

      if (changed)
         modified_ = true;
   }

   int MimeField::GetLength() const
   {
      int nLength = (int) name_.size() + 4;
      FieldCodeBase* pCoder = MimeEnvironment::CreateFieldCoder(GetName());
      pCoder->SetCharset(charset_.c_str());
      pCoder->SetInput(value_.c_str(), (int)value_.size(), true);
      delete pCoder;
      return nLength;
   }

   // store a field to string buffer
   void MimeField::Store(AnsiString &output) const
   {
      if (!modified_ && !is_new_ && !raw_line_.empty())
      {
         output.append(raw_line_);
         return;
      }

      output.append(name_);
      output.append(": ");

      FieldCodeBase* pCoder = MimeEnvironment::CreateFieldCoder(GetName());
      pCoder->SetCharset(charset_.c_str());
      pCoder->SetInput(value_.c_str(), (int)value_.size(), true);
      pCoder->GetOutput(output);
      delete pCoder;

      output.append("\r\n");

   }

   // load a field from string buffer
   size_t MimeField::Load(const char* pszData, size_t nDataSize, bool unfold)
   {
      Clear();
      ASSERT(pszData != NULL);
      const char *pszEnd, *pszStart = pszData;
      // find the next field (e.g. "\r\nContent...")
      while (CMimeChar::IsSpace((unsigned char)*pszStart))
      {
         if (*pszStart == '\r')		// end of header ?
            return 0;
         pszStart = FindString(pszStart, "\r\n", pszData+nDataSize);
         if (!pszStart)
            return 0;
         pszStart += 2;
      }

      // save start of field name for raw capture
      const char* pszFieldStart = pszStart;

      // get the field name
      pszEnd = LineFind(pszStart, ':');
      if (pszEnd != NULL)				// if colon not found, Name would be empty
      {
         name_.assign(pszStart, (pszEnd-pszStart));
         pszStart = pszEnd + 1;
      }

      // find the end of the field
      while (*pszStart == ' ' || *pszStart == '\t')
         pszStart++;
      pszEnd = pszStart;
      do
      {
         pszEnd = FindString(pszEnd, "\r\n", pszData+nDataSize);
         if (!pszEnd)
            return 0;
         pszEnd += 2;
      } while (*pszEnd == '\t' || *pszEnd == ' ');	// linear-white-space

      // Capture the raw line before any processing (includes folding and trailing \r\n)
      raw_line_.assign(pszFieldStart, pszEnd - pszFieldStart);
      modified_ = false;
      is_new_ = false;

      // BEGIN change for hMailServer
      int lLength = (int)(pszEnd-pszStart)-2;
      char *pValue = new char[lLength + 1];
      memset(pValue, 0, lLength+1);
      strncpy_s(pValue, lLength+1, pszStart, lLength);
      value_ = pValue;
      delete [] pValue;

      // We need to unfold the field value
      if (unfold)
         UnfoldField(value_);

      // END change for hMailServer

      return (int) (pszEnd - pszData);
   }

   void MimeField::UnfoldField(string& strField)
   {
      string sRetVal;
      bool bNewLinePassed = false;
      char cLastAddedChar = 0;
      for (unsigned int i = 0; i < strField.length(); i++)
      {
         unsigned char sChar = strField[i];

         // Unfolded strings should not contain any crlf.
         if (sChar == '\r' || sChar == '\n')
         {
            // If we find a new line, we should replace it with
            // a single space. So we just add a space here once
            if (!bNewLinePassed)
               sRetVal += " ";

            bNewLinePassed = true;
            continue;
         }

         // We should skip any space directly after a newline.
         if (CMimeChar::IsSpace(sChar) && bNewLinePassed)
            continue;

         bNewLinePassed = false;

         cLastAddedChar = sChar;
         sRetVal += sChar;
      }

      strField = sRetVal;
   }

   bool MimeField::FindParameter(const char* pszAttr, int& nPos, int& nSize, bool &encodedParameter) const
   {
      encodedParameter = false;

      ASSERT(pszAttr != NULL);
      const char* pszParms = ::strchr(value_.data(), ';');

      int nAttrSize = (int)::strlen(pszAttr);

      while (pszParms != NULL)
      {
         while (CMimeChar::IsSpace((unsigned char)*pszParms) || *pszParms == ';')
            pszParms++;

         const char* pszName = pszParms;		// pszName -> attribute
         pszParms = ::strchr(pszParms, '=');
         if (!pszParms)
            break;

         pszParms++;					// pszParams -> parameter value
         while (*pszParms == ' ' || *pszParms == '\t' && *pszParms > 0)
            pszParms++;

         // If a parameter value is encoded, it might have the
         // syntax field*=value. If this is the case. However, this
         // function should just be able to fetch the value. It won't
         // take care of decoding any contents.
         int nActualAttrNameSize = (int) (pszParms - pszName - 1);

         if (*pszParms == 0)
            return false;

         const char* pszParmEnd = NULL;
         if (*pszParms == '"')		// quoted string
            pszParmEnd = ::strchr(pszParms+1, '"');
         if (!pszParmEnd)			// non quoted string (includes RFC 2231 values like UTF-8''name)
         {
            pszParmEnd = pszParms;

            // Scan to the next ';' or end. Using IsToken here is insufficient because
            // RFC 2231 unquoted values contain non-token characters such as apostrophes.
            while (*pszParmEnd && *pszParmEnd != ';')
               pszParmEnd++;
         }
         else  pszParmEnd++;			// pszParmEnd -> end of parameter value

         // Check if we've found a correct parameter. The parameter may be 
         // formatted in a few different ways, such as
         //
         // name=value
         // name*=value
         // name value

         if (!::_memicmp(pszAttr, pszName, nAttrSize) &&
            (CMimeChar::IsSpace((unsigned char)pszName[nActualAttrNameSize]) || pszName[nActualAttrNameSize] == '='))
         {
            nPos = (int)(pszParms - value_.data());
            nSize = (int)(pszParmEnd - pszParms);

            if (pszName + nAttrSize < pszParmEnd)
            {
               if (*(pszName + nAttrSize) == '*')
                  encodedParameter = true;
            }

            return true;
         }

         pszParms = pszParmEnd;
      }
      return false;
   }

   AnsiString
      MIMEUnicodeEncoder::EncodeValue(const AnsiString &sCharSet, const String &sUnicodeValue)
   {
      AnsiString sMBText = Charset::ToMultiByte(sUnicodeValue, sCharSet);

      FieldCodeBase pCoder;

      pCoder.SetCharset(sCharSet);
      pCoder.SetInput(sMBText, sMBText.GetLength(), true);

      AnsiString encodedValue;
      pCoder.GetOutput(encodedValue);
      return encodedValue;
   }

   //////////////////////////////////////////////////////////////////////
   // MimeHeader class - Represents the header of a MIME body part
   //////////////////////////////////////////////////////////////////////

   // Return the media type represented by Content-Type field (see RFC 2046)
   MimeHeader::MediaType MimeHeader::GetMediaType() const
   {
      const char* pszType = GetContentType();
      if (!pszType)
         pszType = "text";

      int nIndex = 0;
      while (type_table_[nIndex] != NULL &&
         ::_memicmp(pszType, type_table_[nIndex], ::strlen(type_table_[nIndex])) != 0)
         nIndex++;
      return (MediaType) nIndex;
   }

   String
   MimeHeader::GetUnicodeFilename() const
   {
      String rawValue = GetRawFilename();
      String unicodeValue = GetUnicodeFieldValue(CMimeConst::ContentDisposition(), rawValue);

      return unicodeValue;
   }

   String
   MimeHeader::GetRawFilename() const
   {
      AnsiString sRawValue = GetParameter(CMimeConst::ContentDisposition(), CMimeConst::Filename()); 
      if (sRawValue.IsEmpty())
         sRawValue = GetParameter(CMimeConst::ContentType(), CMimeConst::Name());

      return sRawValue;
   }

   void
   MimeHeader::SetFileName(const String &file_name)
   {
      AnsiString encoded_filename = MIMEUnicodeEncoder::EncodeValue("utf-8", file_name);

      MimeField* pfd = GetField(CMimeConst::ContentDisposition());
      if (pfd != nullptr)
      {
         AnsiString existingValue;
         if (pfd->GetParameter(CMimeConst::Filename(), existingValue))
         {
            // Remove all existing filename parameters, including RFC 2231 continuations
            // (filename*0, filename*1, filename*), before setting the new single value.
            pfd->RemoveParameter(CMimeConst::Filename());
            pfd->SetParameter(CMimeConst::Filename(), encoded_filename);
            return;
         }
      }

      pfd = GetField(CMimeConst::ContentType());
      if (pfd != nullptr)
      {
         pfd->RemoveParameter(CMimeConst::Name());
         pfd->SetParameter(CMimeConst::Name(), encoded_filename);
      }
   }

   String
   MimeHeader::GetUnicodeFieldValue(const AnsiString &pszFieldName) const
   {
      AnsiString sRawFieldValue = GetRawFieldValue(pszFieldName);
      return GetUnicodeFieldValue(pszFieldName, sRawFieldValue);
   }

   String
   MimeHeader::GetUnicodeFieldValue(const AnsiString &pszFieldName, const AnsiString &sRawFieldValue)
   {
      if (sRawFieldValue.IsEmpty())
         return sRawFieldValue;


      // De-code the value to plain text.
      AnsiString sRetVal;
      FieldCodeBase* pCoder = MimeEnvironment::CreateFieldCoder(pszFieldName);
      pCoder->SetInput(sRawFieldValue, sRawFieldValue.GetLength(), false);
      pCoder->GetOutput(sRetVal);

      AnsiString sCharset = pCoder->GetCharset();

      delete pCoder;

      String sWideStr = Charset::ToWideChar(sRetVal, sCharset);

      return sWideStr;
   }

   void 
   MimeHeader::SetUnicodeFieldValue(const AnsiString &sFieldName, const String & sFieldValue, const AnsiString & sCharset)
   {
      // Retrieve the current charset for this field.
      MimeField* pfd = GetField(sFieldName);

      if (sFieldValue.GetLength() == 0)
      {
         SetRawFieldValue(sFieldName, "", "");
         return;
      }

      AnsiString sCharsetToUse = sCharset;

      // If client hasn't specified character set, assume it's the existing one.
      if (pfd && pfd->GetCharset() && sCharsetToUse.IsEmpty())
         sCharsetToUse = pfd->GetCharset();

      // If there's no existing charset, assume it's the same as for the email.
      if (sCharsetToUse.IsEmpty())
         sCharsetToUse = GetCharset();

      if (sCharsetToUse.IsEmpty())
         sCharsetToUse = "utf-8";

      AnsiString sMBText = Charset::ToMultiByte(sFieldValue, sCharsetToUse);

      // Encode the value
      FieldCodeBase* pCoder = MimeEnvironment::CreateFieldCoder(sFieldName);
      pCoder->SetInput(sMBText, sMBText.GetLength(), true);
      pCoder->SetCharset(sCharsetToUse.c_str());

      AnsiString sEncodedValue;
      pCoder->GetOutput(sEncodedValue);

      delete pCoder;   

      SetRawFieldValue(sFieldName, sEncodedValue, sCharsetToUse);
   }


   // get the top-level media type
   string MimeHeader::GetMainType() const
   {
      string strType;
      const char* pszType = GetContentType();
      if (pszType != NULL)
      {
         const char* pszSlash = ::strchr(pszType, '/');
         if (pszSlash != NULL)
            strType.assign(pszType, pszSlash-pszType);
         else
            strType = pszType;
      }
      else
         strType = "text";
      return strType;
   }

   // get the subtype
   string MimeHeader::GetSubType() const
   {
      string strSubType;
      MimeField *pfd = GetField(CMimeConst::ContentType());
      if (pfd != NULL)
      {
         string strType;
         pfd->GetValue(strType);
         string::size_type nSlash = strType.find('/');
         if (nSlash > 0)
            strSubType = strType.substr(nSlash+1);
      }
      else
         strSubType = "plain";
      return strSubType;
   }

   // set the 'charset' parameter (for text) of Content-Type field
   void MimeHeader::SetCharset(const char* pszCharset)
   {
      MimeField *pfd = GetField(CMimeConst::ContentType());
      if (!pfd)
      {
         MimeField fd;
         fd.SetName(CMimeConst::ContentType());
         fd.SetValue("text/plain");
         fd.SetParameter(CMimeConst::Charset(), pszCharset);
         fields_.push_back(fd);
      }
      else
         pfd->SetParameter(CMimeConst::Charset(), pszCharset);
   }

   // set the 'name' parameter (for attachment) of Content-Type field
   void MimeHeader::SetName(const char* pszName)
   {
      MimeField *pfd = GetField(CMimeConst::ContentType());
      if (!pfd)
      {
         // get the appropriate media-type/subtype according to file extension
         ASSERT(pszName != NULL);
         string strType;
         const char* pszType = "application/octet-stream";
         const char* pszFileExt = ::strrchr(pszName, '.');
         if (pszFileExt != NULL)
         {
            pszFileExt++;
            int nIndex = 0;
            while (type_cvt_table_[nIndex].nMediaType != MEDIA_UNKNOWN)
            {
               if (!::_stricmp(pszFileExt, type_cvt_table_[nIndex].pszFileExt))
               {
                  strType = type_table_[type_cvt_table_[nIndex].nMediaType];
                  strType += '/';
                  strType += type_cvt_table_[nIndex].pszSubType;
                  pszType = strType.c_str();
                  break;
               }
               nIndex++;
            }
         }

         MimeField fd;
         fd.SetName(CMimeConst::ContentType());
         fd.SetValue(pszType);
         fd.SetParameter(CMimeConst::Name(), pszName);
         fields_.push_back(fd);
      }
      else
         pfd->SetParameter(CMimeConst::Name(), pszName);
   }

   // set 'boundary' parameter (for multipart) of Content-Type field
   void MimeHeader::SetBoundary(const char* pszBoundary/*=NULL*/)
   {
      static int s_nPartNumber = 0;
      char buf[80];
      if (!pszBoundary)				// generate a new boundary delimiter
      {
         unsigned __int64 value = (unsigned __int64)::time(NULL) ^ (unsigned __int64)this;
         ::srand((unsigned int) value);
         ::sprintf_s(buf, 80, "__=_Part_Boundary_%03d_%06d.%06d", ++s_nPartNumber, rand(), rand());
         if (s_nPartNumber >= 9)
            s_nPartNumber = 0;
         pszBoundary = buf;
      }

      MimeField *pfd = GetField(CMimeConst::ContentType());
      if (!pfd)
      {
         MimeField fd;
         fd.SetName(CMimeConst::ContentType());
         fd.SetValue("multipart/mixed");
         fd.SetParameter(CMimeConst::Boundary(), pszBoundary);
         fields_.push_back(fd);
      }
      else
      {
         if (::_memicmp(pfd->GetValue(), "multipart", 9) != 0)
            pfd->SetValue("multipart/mixed");
         pfd->SetParameter(CMimeConst::Boundary(), pszBoundary);
      }
   }

   void MimeHeader::Clear()
   {
      fields_.clear();
   }

   // return the length needed to store this header to string buffer
   int MimeHeader::GetLength() const
   {
      int nLength = 0;
      std::vector<MimeField>::const_iterator it;
      for (it = fields_.begin(); it != fields_.end(); it++)
         nLength += (*it).GetLength();
      return nLength + 2;				// a pair of CRLF indicate the end of header
   }


   void
      MimeHeader::DeleteField(const AnsiString &fieldName)
   {
      headers_modified_ = true;
      MimeField *field = GetField(fieldName);
      if (field)
         DeleteField(field);
   }

   void
      MimeHeader::DeleteField(MimeField *pField)
   {
      headers_modified_ = true;
      auto iter = fields_.begin(); 
      std::vector<MimeField>::const_iterator iterEnd = fields_.end();

      for (; iter != iterEnd; iter++)
      {
         const MimeField& fd = (*iter);

         if (&fd == pField)
         {
            fields_.erase(iter);
            return;
         }

      }
   }

   // store the header to string buffer
   AnsiString MimeHeader::Store() const
   {
      AnsiString headerString;
      Store(headerString);

      return headerString;
   }

   // store the header to string buffer
   void MimeHeader::Store(AnsiString &output) const
   {
      std::vector<MimeField>::const_iterator it;
      for (it = fields_.begin(); it != fields_.end(); it++)
      {
         const MimeField& fd = *it;
         fd.Store(output);
      }

      output.append("\r\n");
   }


   // load a header from string buffer
   size_t MimeHeader::Load(const char* pszData, size_t nDataSize, bool unfold)
   {
      ASSERT(pszData != NULL);

      size_t nInput = 0;
      while (pszData[nInput] != 0 && pszData[nInput] != '\r')
      {
         MimeField fd;
         size_t nSize = fd.Load(pszData + nInput, nDataSize - nInput, unfold);
         if (nSize == 0)
            return nSize;

         nInput += nSize;
         fields_.push_back(fd);	// don't use SetField in case of same name fields
      }

      headers_modified_ = false;
      return nInput + 2;				// skip the ending CRLF
   }

   // returns the encoded version of the header.
   AnsiString MimeHeader::GetHeaderContents()
   {
      AnsiString sResponse;
      // Iterate through the header fields and add to sResponse.
      std::vector<MimeField> oFields =  Fields();
      auto iterField = oFields.begin();

      while (iterField != oFields.end())
      {
         String sHeaderField = (*iterField).GetName();
         String sHeaderValue = (*iterField).GetValue();

         sResponse += sHeaderField + _T(": ") + sHeaderValue + _T("\r\n");

         iterField++;
      }

      sResponse += "\r\n";

      return sResponse;

   }

   // returns the content of the header in unicode (decoded)
   String MimeHeader::GetUnicodeHeaderContents()
   {
      String sResponse;
      // Iterate through the header fields and add to sResponse.
      std::vector<MimeField> oFields =  Fields();
      auto iterField = oFields.begin();

      while (iterField != oFields.end())
      {

         AnsiString sHeaderField = (*iterField).GetName();
         AnsiString sHeaderValue = (*iterField).GetValue();

         String decodedValue = GetUnicodeFieldValue(sHeaderField, sHeaderValue);

         sResponse += String(sHeaderField) + _T(": ") + decodedValue + _T("\r\n");

         iterField++;
      }

      sResponse += "\r\n";

      return sResponse;

   }

   bool 
      MimeHeader::FieldExists(const char *pszFieldName) const
   {
      auto iter = FindField(pszFieldName);
      if (iter == fields_.end())
         return false;

      return true;
   }


   std::vector<MimeField>::iterator MimeHeader::FindField(const char* pszFieldName) const
   {
      MimeHeader *pThis = const_cast<MimeHeader*>(this);
      for (auto it = pThis->fields_.begin(); it != pThis->fields_.end(); it++)
      {
         MimeField& fd = *it;
         if (!::_stricmp(fd.GetName(), pszFieldName))
            return it;
      }

      return pThis->fields_.end();
   }

   //////////////////////////////////////////////////////////////////////
   // MimeBody class - Represents a body part in a MIME message
   //////////////////////////////////////////////////////////////////////
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

   // initialize the content with text
   int MimeBody::SetRawText(const AnsiString &sText)
   {
      body_modified_ = true;
      text_ = sText;
      return 0;
   }

   // initialize the content with text
   int MimeBody::SetUnicodeText(const String &sText)
   {
      body_modified_ = true;
      // Retrieve the current charset for this field.
      std::string strCharset = GetCharset();

      if (strCharset == "")
      {
         strCharset = "utf-8";
         SetCharset(strCharset.c_str());
      }

      AnsiString sMBText = Charset::ToMultiByte(sText, strCharset);

      // Normalise bare LFs to CRLF before QP-encoding so the encoder does not
      // write literal \n bytes into the message file (which CheckLineEndings_
      // would then correctly flag as bare LFs and reject).
      sMBText.Replace("\r\n", "\n");
      sMBText.Replace("\n", "\r\n");

      AnsiString sEncodedValue;
      MimeCodeBase* pCoder = MimeEnvironment::CreateCoder("quoted-printable");
      static_cast<MimeCodeQP*>(pCoder)->AddLineBreak(true);
      pCoder->SetInput(sMBText, sMBText.GetLength(), true);
      pCoder->GetOutput(sEncodedValue);
      delete pCoder;

      SetTransferEncoding("quoted-printable");

      return SetRawText(sEncodedValue);
   }   

   string MimeBody::GetRawText()
   {
      return text_;
   }

   String
      MimeBody::GetUnicodeText()
   {
      AnsiString sRawText = GetRawText();

      if (sRawText.IsEmpty())
         return "";

      std::string strCharset = GetCharset();
      if (strCharset.size() == 0)
         return sRawText;

      // De-code the value to plain text.
      AnsiString sRetVal;
      MimeCodeBase* pCoder = MimeEnvironment::CreateCoder(GetTransferEncoding());
      pCoder->SetInput(sRawText, sRawText.GetLength(), false);
      pCoder->GetOutput(sRetVal);
      delete pCoder;

      String sWideStr = Charset::ToWideChar(sRetVal, strCharset);

      return sWideStr;
   }

   std::shared_ptr<MimeBody> 
      MimeBody::LoadEncapsulatedMessage() const
   {
      // try to generate a file name using the message subject.
      std::shared_ptr<MimeBody> pEncapsulatedMessage = std::shared_ptr<MimeBody>(new MimeBody);

      size_t iLength = GetContentLength();
      char *pData = new char[iLength+1];
      strncpy_s(pData, iLength+1, (const char*) GetContent(), iLength);
      size_t index = 0;
      bool part_loaded;
      pEncapsulatedMessage->Load(pData, iLength, index, part_loaded);
      delete [] pData;

      return pEncapsulatedMessage;
   }

   String 
      MimeBody::GenerateFileNameFromEncapsulatedSubject(bool unicode) const
   {
      if (!IsEncapsulatedRFC822Message())
         return "";

      std::shared_ptr<MimeBody> pEncapsulatedMessage = LoadEncapsulatedMessage();

      String sFilename = unicode ? 
         pEncapsulatedMessage->GetUnicodeFieldValue("Subject") :
      pEncapsulatedMessage->GetRawFieldValue("Subject");

      if (!sFilename.IsEmpty())
      {
         // Do some cleaning to reduce the risk of vulnerability in
         // COM client which does not check files before saving them
         // locally.
         sFilename.Replace(_T("\\"), _T(""));
         sFilename.Replace(_T("/"), _T(""));
         sFilename.Replace(_T(":"), _T(""));

         sFilename = sFilename + ".eml";
      }

      return sFilename;
   }

   String
      MimeBody::GetUnicodeFilename() const
   {
      String sFilename = MimeHeader::GetUnicodeFilename();

      if (sFilename.IsEmpty())
         sFilename = GenerateFileNameFromEncapsulatedSubject(true);

      if (sFilename.IsEmpty())
         sFilename.Format(_T("ATT-%d.dat"), part_index_);

      return sFilename;
   }

   String
      MimeBody::GetRawFilename() const
   {
      String sFilename = MimeHeader::GetRawFilename();

      if (sFilename.IsEmpty())
         sFilename = GenerateFileNameFromEncapsulatedSubject(false);

      if (sFilename.IsEmpty())
         sFilename.Format(_T("ATT-%d.dat"), part_index_);

      return sFilename;
   }


   // initialize the content of this body part with a mail message
   bool MimeBody::SetMessage(const MimeBody* pMM)
   {
      ASSERT(pMM != NULL);
      int nSize = pMM->GetLength();
      if (!AllocateBuffer(nSize+4))
         return false;

      pMM->Store(text_);

      const char* pszType = GetContentType();
      if (!pszType || ::_memicmp(pszType, "message", 7) != 0)
         SetContentType("message/rfc822", "");

      return true;
   }

   void MimeBody::GetMessage(MimeBody* pMM) const
   {
      ASSERT(pMM != NULL);
      ASSERT(text_ != NULL);
      size_t index = 0;

      bool part_loaded;
      pMM->Load((const char*)text_, (int) text_.size(), index, part_loaded);
   }

   bool 
      MimeBody::IsEncapsulatedRFC822Message() const
   {
      if (!IsAttachment())
         return false;

      String sMainType = GetMainType();
      String sSubType = GetSubType();

      sMainType.ToUpper();
      sSubType.ToUpper();

      if (sMainType == _T("MESSAGE") && sSubType == _T("RFC822"))
         return true;
      else
         return false;
   }

   bool
      MimeBody::IsAttachment()  const
   {
      /*
      Previously we looked at the ContentDisposition header and the Name header to determine
      whether it's an attachment or not. This was not safe, since a lot of attachments did
      not have these headers but just a Content-type header. The new strategy is:


      1) If the ContentDisposition is of type attachment, we assume it's an attachment
      2) If the main ContentType is text or multipart, we assume that it's not an attachment
      3) In all other cases, we treat it as an attachment.

      discrete-type := "text" / "image" / "audio" / "video" / "application" / extension-token
      composite-type := "message" / "multipart" / extension-token
      */

      // If the content-disposition is set to attachment, we always treats it as an attachment
      // even if the main type is set to multipart or text.
      AnsiString sDisposition = GetRawFieldValue(CMimeConst::ContentDisposition());
      if (sDisposition.StartsWith(CMimeConst::Attachment()))
         return true;

      if (sDisposition.StartsWith(CMimeConst::Inline()))
      {
         AnsiString sFileName = GetParameter(CMimeConst::ContentDisposition(), "filename");

         if (!sFileName.IsEmpty())
            return true;
      }

      String sMainType = GetMainType();

      if (sMainType.CompareNoCase(_T("multipart")) == 0)
      {
         // Multipart ...
         return false;
      }

      if (sMainType.CompareNoCase(_T("text")) == 0)
      {
         // This is just a text part.
         return false;
      }

      return true;
   }

   int 
      MimeBody::LoadFromFile(const AnsiString &pszFilename)
   {
      File oFile;
      if (!oFile.Open(pszFilename, File::OTReadOnly))
         return false;

      // Read the file as a text file. This will cause a null
      // to be added by File, which is required by Load() below.
      std::shared_ptr<ByteBuffer> pFileContents = oFile.ReadTextFile();

      FreeBuffer();
      if (pFileContents->GetSize() > 0)
      {
         try
         {
            // Minus one, since the last character is the null...
            size_t index = 0;
            bool part_loaded;
            size_t nLoaded = Load(pFileContents->GetCharBuffer(), pFileContents->GetSize() - 1, index, part_loaded);

            // Record source file and body offset for body preservation during save.
            // For multipart messages, last_multipart_end_ mirrors what the parser
            // actually consumed for the closing boundary, including the trailing
            // CRLF when present in the source file. For simple messages, use the
            // full Load() return value.
            source_file_ = pszFilename;
            body_byte_offset_ = last_header_size_;
            body_byte_end_ = (last_multipart_end_ > 0) ? last_multipart_end_ : nLoaded;

            // Some SMTP senders unconditionally append \r\n before the dot terminator
            // even when the message already ends with \r\n, resulting in a spurious blank
            // line at the end of the stored file. Strip it so re-serialization does not
            // alter the body (which would invalidate DKIM body hashes).
            // Detection: body ends with \r\n\r\n (content's own final \r\n + the extra one).
            if (last_multipart_end_ == 0 && body_byte_end_ > body_byte_offset_ + 4)
            {
               const char* buf = pFileContents->GetCharBuffer();
               size_t end = body_byte_end_;
               if (buf[end-1] == '\n' && buf[end-2] == '\r' &&
                   buf[end-3] == '\n' && buf[end-4] == '\r')
               {
                  body_byte_end_ -= 2;
               }
            }

            body_modified_ = false;
         }
         catch (...)
         {
            String sFilename = pszFilename;

            String sFileNameExclPath = FileUtilities::GetFileNameFromFullPath(sFilename);
            String sMessageBackupPath = IniFileSettings::Instance()->GetLogDirectory() + "\\Problematic messages\\" + sFileNameExclPath;
            FileUtilities::Copy(sFilename, sMessageBackupPath, true);

            String sErrorMessage = Formatter::Format("An unknown error occurred while loading message. File: {0}. Backuped to: {1}", pszFilename, sMessageBackupPath);

            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4228, "MimeBody::LoadFromFile", sErrorMessage);
         }
      }

      return true;
   }

   // Returns true if any child body part has been modified (body text or headers).
   // Used by SaveAllToFile to detect when full re-serialization is required even if
   // the top-level body_modified_ flag is false.
   bool MimeBody::IsAnyChildModified() const
   {
      for (const auto& child : bodies_)
      {
         if (child->body_modified_ || child->headers_modified_)
            return true;
         if (child->IsAnyChildModified())
            return true;
      }
      return false;
   }

   // Read the body bytes from the original source file, from body_byte_offset_ to body_byte_end_.
   AnsiString MimeBody::ReadBodyFromSourceFile() const
   {
      File oFile;
      if (!oFile.Open(source_file_, File::OTReadOnly))
         return "";

      std::shared_ptr<ByteBuffer> contents = oFile.ReadTextFile();
      if (!contents || contents->GetSize() <= body_byte_offset_)
         return "";

      // Use body_byte_end_ to copy only the bytes the MIME parser actually consumed,
      // which excludes any trailing garbage bytes beyond the parsed message.
      size_t end = (body_byte_end_ > 0 && body_byte_end_ <= contents->GetSize() - 1)
                   ? body_byte_end_
                   : contents->GetSize() - 1;  // -1 to exclude null terminator

      if (end <= body_byte_offset_)
         return "";

      return AnsiString(contents->GetCharBuffer() + body_byte_offset_,
                        end - body_byte_offset_);
   }

   // write the entire header and content to file.
   bool MimeBody::SaveAllToFile(const AnsiString & pszFilename)
   {
      // If body content was not modified and we have a source file, serialize only
      // headers (using raw_line_ for unmodified fields) and copy body from disk.
      // This preserves exact byte sequences required for DKIM signature validity.
      if (!body_modified_ && !IsAnyChildModified() && !source_file_.IsEmpty())
      {
         AnsiString headerBuffer;
         MimeHeader::Store(headerBuffer);  // includes trailing blank line \r\n

         AnsiString bodyFromDisk = ReadBodyFromSourceFile();

         AnsiString fullMessage = headerBuffer + bodyFromDisk;
         bool result = FileUtilities::WriteToFile(pszFilename, fullMessage);
         if (result)
         {
            // Update source tracking so subsequent saves also use the fast path
            size_t newHeaderSize = (size_t)headerBuffer.GetLength();
            size_t newBodySize = (size_t)bodyFromDisk.GetLength();
            source_file_ = pszFilename;
            body_byte_offset_ = newHeaderSize;
            body_byte_end_ = newHeaderSize + newBodySize;
         }
         return result;
      }

      // Fallback: full re-serialization (body was modified or no source file available)
      AnsiString buffer;
      Store(buffer);
      return FileUtilities::WriteToFile(pszFilename, buffer);
   }

   // initialize the content (attachment) by reading from a file
   bool MimeBody::ReadFromFile(const String &pszFilename)
   {
      File oFile;
      if (!oFile.Open(pszFilename, File::OTReadOnly))
         return false;

      std::shared_ptr<ByteBuffer> pUnencodedBuffer = oFile.ReadFile();

      if (!pUnencodedBuffer)
         return false;

      // Encode the file, to base64 or likewise.
      MimeCodeBase* pCoder = MimeEnvironment::CreateCoder(GetTransferEncoding());
      ASSERT(pCoder != NULL);
      pCoder->SetInput((const char*) pUnencodedBuffer->GetCharBuffer(), pUnencodedBuffer->GetSize(), true);

      // Copy the buffer
      pCoder->GetOutput(text_);

      AnsiString sCharset = "utf-8";

      // Set params to this
      String sFileName = FileUtilities::GetFileNameFromFullPath(pszFilename);
      AnsiString sEncodedValue = MIMEUnicodeEncoder::EncodeValue(sCharset, sFileName);

      SetName(sEncodedValue);				// set 'name' parameter:      

      // Create an content-disposition header as well.
      SetRawFieldValue(CMimeConst::ContentDisposition(), CMimeConst::Inline(), "");
      SetParameter(CMimeConst::ContentDisposition(), CMimeConst::Filename(), sEncodedValue);

      return true;
   }

   // write the content (attachment) to a file
   bool MimeBody::WriteToFile(const String &sFilename)
   {
      // First de-code the content.
      MimeCodeBase* pCoder = MimeEnvironment::CreateCoder(GetTransferEncoding());
      ASSERT(pCoder != NULL);
      pCoder->SetInput(text_, text_.GetLength(), false);

      AnsiString decoded;
      pCoder->GetOutput(decoded);

      FileUtilities::WriteToFile(sFilename, decoded);

      return true;
   }

   // delete all child body parts
   void MimeBody::DeleteAll()
   {
      body_modified_ = true;
      while (!bodies_.empty())
      {
         std::shared_ptr<MimeBody> pBP = bodies_.back();
         bodies_.pop_back();
         ASSERT(pBP != NULL);
      }
   }

   // create a new child body part, and add it to body part list
   std::shared_ptr<MimeBody> MimeBody::CreatePart(const char* pszMediaType/*=NULL*/, std::shared_ptr<MimeBody> pWhere/*=NULL*/)
   {
      body_modified_ = true;
      std::shared_ptr<MimeBody> pBP = MimeEnvironment::CreateBodyPart(pszMediaType);
      ASSERT(pBP != NULL);
      if (pWhere != NULL)
      {
         for (auto it = bodies_.begin(); it != bodies_.end(); it++)
            if (*it == pWhere)
            {
               bodies_.insert(it, pBP);
               return pBP;
            }
      }
      bodies_.push_back(pBP);
      return pBP;
   }

   // create a new child body part, and add it to body part list
   void MimeBody::AddPart(std::shared_ptr<MimeBody> part)
   {
      body_modified_ = true;
      bodies_.push_back(part);
   }

   int 
      MimeBody::GetPartCount()
   {
      return (int) bodies_.size();
   }
   // remove and delete a child body part
   void MimeBody::ErasePart(std::shared_ptr<MimeBody> pBP)
   {
      body_modified_ = true;
      ASSERT(pBP != NULL);
      bodies_.remove(pBP);
   }

   // return a list of all attachment body parts belong to this body part
   // Since we are using smart pointers, and it's not possible to cast
   // from <this> to a smart_ptr, we need to give this function a pointer
   // to itself. Really ugly but should work fine.
   int MimeBody::GetAttachmentList(std::shared_ptr<MimeBody> pThis, BodyList& rList) const
   {
      int nCount = 0;
      int nMediaType = GetMediaType();

      if (MEDIA_MULTIPART != nMediaType)
      {
         if (IsAttachment())
         {
            rList.push_back(pThis);
            nCount++;
         }
      }

      else
      {
         std::list<std::shared_ptr<MimeBody> >::const_iterator it;
         for (it=bodies_.begin(); it!=bodies_.end(); it++)
         {
            std::shared_ptr<MimeBody> pBP = *it;
            ASSERT(pBP != NULL);
            nCount += pBP->GetAttachmentList(pBP, rList);
         }
      }
      return nCount;
   }

   // clear all attachments from this subtype.
   void MimeBody::ClearAttachments()
   {
      body_modified_ = true;
      if (GetMediaType() ==MEDIA_MULTIPART)
      {
         auto it = bodies_.begin();
         while (it != bodies_.end())
         {
            std::shared_ptr<MimeBody> pBody = (*it);
            if (pBody->IsAttachment())
               it = bodies_.erase(it);
            else
               it++;
         }
      }
   }

   // clear all attachments from this subtype.
   void MimeBody::RemoveAttachment(std::shared_ptr<MimeBody> pAttachment)
   {
      body_modified_ = true;
      if (GetMediaType() ==MEDIA_MULTIPART)
      {
         auto it = bodies_.begin();
         while (it != bodies_.end())
         {
            std::shared_ptr<MimeBody> pBody = (*it);
            if (pBody->IsAttachment() && pBody == pAttachment)
            {
               it = bodies_.erase(it);
               return;
            }
            else
               it++;
         }
      }
   }


   void MimeBody::Clear()
   {
      DeleteAll();
      find_ = bodies_.end();
      FreeBuffer();
      MimeHeader::Clear();
      source_file_ = "";
      body_byte_offset_ = 0;
      body_byte_end_ = 0;
      body_modified_ = false;
      last_header_size_ = 0;
      last_multipart_end_ = 0;
   }

   // return the length needed to store this body part 
   // in an de-coded buffer
   int MimeBody::GetLength(bool bIncludeHeader) const
   {
      int nLength = 0;

      if (bIncludeHeader)
         nLength += MimeHeader::GetLength();

      if (bodies_.empty())
         return nLength;

      string strBoundary = GetBoundary();
      int nBoundSize = (int) strBoundary.size();
      std::list<std::shared_ptr<MimeBody> >::const_iterator it;
      for (it=bodies_.begin(); it!=bodies_.end(); it++)
      {
         nLength += nBoundSize + 6;	// include 2 leading hyphens and 2 pair of CRLFs
         std::shared_ptr<MimeBody> pBP = *it;
         ASSERT(pBP != NULL);
         nLength += pBP->GetLength();
      }
      nLength += nBoundSize + 8;		// include 2 leading hyphens, 2 trailng hyphens and 2 pair of CRLFs
      return nLength;
   }

   // store the body part to un-encoded string buffer
   void MimeBody::Store(AnsiString &output, bool bIncludeHeader) const
   {
      // store header fields
      if (bIncludeHeader)
         MimeHeader::Store(output);

      // Copy the data to the output buffer. 
      output.append(text_);

      if (bodies_.empty())
         return;

      // store child body parts
      string strBoundary = GetBoundary();
      if (strBoundary.empty())
         return;					// boundary not be set

      for (BodyList::const_iterator it=bodies_.begin(); it!=bodies_.end(); it++)
      {
         // If the initial body ends with \r\n, remove them. We add new ones below.
         if (bodies_.begin() == it && output.size() >= 2 && 
            output.at(output.size()-2) == '\r' && output.at(output.size()-1) == '\n')
         {
            output = output.Mid(0, output.GetLength() - 2);
         }

         AnsiString boundaryLine = Formatter::Format(_T("\r\n--{0}\r\n"), String(strBoundary));
         output.append(boundaryLine);

         std::shared_ptr<MimeBody> pBP = *it;
         ASSERT(pBP != NULL);	

         pBP->Store(output);
      }

      AnsiString endBoundaryLine = Formatter::Format(_T("\r\n--{0}--\r\n"), String(strBoundary));
      output.append(endBoundaryLine);
   }

   String
      MimeBody::GetCleanContentType() const
   {
      String sMainPart = GetMainType();
      String sSubPart = GetSubType();

      return sMainPart + "/" + sSubPart;
   }

   const char *
      GetBoundaryEnd(const char *startSearch, const char *endSearch, string boundary)
   {
      if (endSearch <= startSearch ||
         endSearch == 0 || 
         startSearch == 0 ||
         boundary.length() == 0)
      {
         return 0;
      }

      const char* possibleEnding = FindString(startSearch, boundary.c_str(), endSearch);

      int counter = 1000;
      while (possibleEnding != 0 && counter > 0)
      {
         counter--;

         // We inspect the 2 bytes immediately following the full boundary text
         // to verify that this is a real boundary line ("--" or "\r\n"), not
         // just a boundary prefix found inside other content.
         size_t bytesRemainingFromCandidate = endSearch - possibleEnding;
         size_t bytesRequiredForBoundaryAndSuffix = boundary.length() + 2;
         if (bytesRemainingFromCandidate < bytesRequiredForBoundaryAndSuffix)
         {
            // malformed message. the end of the character string is the boundary line with no trailing crlf or --.
            return 0;
         }

         if ((possibleEnding[boundary.size()] == '-' && possibleEnding[boundary.size()+1] == '-') ||
            (possibleEnding[boundary.size()] == '\r' && possibleEnding[boundary.size()+1] == '\n'))
         {
            return possibleEnding;
         }

         // false. try again.
         possibleEnding = FindString(possibleEnding+1, boundary.c_str(), endSearch);
      }

      return 0;
   }

   // load a body part from string buffer
   size_t MimeBody::Load(const char* pszData, size_t nDataSize, size_t &index, bool &part_loaded)
   {
      part_loaded = true;
      index++;
      part_index_ = index;

      // load header fields
      size_t nSize = MimeHeader::Load(pszData, nDataSize, true);
      if (nSize == 0)
         return nSize;

      last_header_size_ = nSize;  // record for body offset calculation in LoadFromFile

      const char* pszDataBegin = pszData;	// preserve start position
      pszData += nSize;
      
      if (nSize >= nDataSize)
         return (int)(pszData - pszDataBegin);

      nDataSize -= nSize;
      FreeBuffer();

      // determine the length of the content
      const char* pszEnd = pszData + nDataSize;
      int nMediaType = GetMediaType();
      if (MEDIA_MULTIPART == nMediaType)
      {
         // find the begin boundary
         string strBoundary = GetBoundary();
         if (!strBoundary.empty())
         {
            strBoundary = "\r\n--" + strBoundary + "\r\n";
            pszEnd = FindString(pszData-2, strBoundary.c_str(), pszEnd);
            if (!pszEnd)
               pszEnd = pszData + nDataSize;
            else
               pszEnd += 2;
         }
      }

      // load content
      nSize = (int)(pszEnd - pszData);
      if (nSize > 0)
      {
         if (AllocateBuffer(nSize+4))
         {
            text_.append(pszData, nSize);

            pszData += nSize;

            if (nSize >= nDataSize)
               return (int)(pszData - pszDataBegin);
         }
         else
         {
            part_loaded = false;
            return 0;
         }
      }

      // load child body parts
      string strBoundary = GetBoundary();
      ASSERT(strBoundary.size() > 0);
      strBoundary = "\r\n--" + strBoundary;

      // look for the first boundary (case sensitive)
      pszData -= 2;					// go back to CRLF
      nDataSize += 2;
      pszEnd = pszData + nDataSize;

      const char* pszBound1 = GetBoundaryEnd(pszData, pszEnd, strBoundary.c_str());

      int counter = 10000;
      while (pszBound1 != NULL && pszBound1 < pszEnd && counter > 0)
      {
         counter--;
         // pszBound1 points at the start of "\r\n--boundary". Move past the
         // boundary text so we can inspect what terminates this boundary line.
         const char* pszAfterBoundary = pszBound1 + strBoundary.size();

         // Need at least 2 bytes available to distinguish a closing boundary
         // ("--") from a normal part boundary ("\r\n").
         if (pszAfterBoundary + 2 > pszEnd)
            break;

         if (pszAfterBoundary[0] == '-' && pszAfterBoundary[1] == '-')
         {
            const char* pszAfterClosingBoundary = pszAfterBoundary + 2;

            // Preserve the trailing CRLF after the closing boundary when it
            // exists in the source file. Some malformed messages end directly
            // at "--boundary--", so accept EOF there as well.
            if (pszAfterClosingBoundary + 2 <= pszEnd &&
                pszAfterClosingBoundary[0] == '\r' &&
                pszAfterClosingBoundary[1] == '\n')
            {
               // Include the trailing CRLF after "--boundary--" in the preserved byte range.
               last_multipart_end_ = (pszAfterClosingBoundary + 2) - pszDataBegin;
               return (int)(pszAfterClosingBoundary + 2 - pszDataBegin);	// reach the closing boundary
            }

            // Preserve EOF exactly as it appeared on disk when the closing
            // boundary is the final bytes in the file, but reject any extra
            // trailing bytes such as "--boundary--garbage".
            if (pszAfterClosingBoundary == pszEnd)
            {
               last_multipart_end_ = pszAfterClosingBoundary - pszDataBegin;
               return (int)(pszAfterClosingBoundary - pszDataBegin);
            }

            break;
         }

         // A non-closing part boundary must be followed by CRLF before the
         // next part headers begin. If not, stop parsing rather than scanning
         // past the valid input range looking for a newline.
         if (pszAfterBoundary[0] != '\r' || pszAfterBoundary[1] != '\n')
            break;

         const char* pszStart = pszAfterBoundary + 2;

         // look for the next boundary
         const char* pszBound2 = GetBoundaryEnd(pszStart, pszEnd, strBoundary.c_str());

         if (!pszBound2)				// overflow, boundary may be truncated
            pszBound2 = pszEnd;
         int nEntitySize = (int) (pszBound2 - pszStart);

         std::shared_ptr<MimeBody> pBP = std::shared_ptr<MimeBody>(new MimeBody());

         bodies_.push_back(pBP);

         bool part_loaded;
         size_t nInputSize = pBP->Load(pszStart, nEntitySize, part_index_, part_loaded);
         if (!part_loaded)
         {
            ErasePart(pBP);
            return nInputSize;
         }
         pszBound1 = pszBound2;
      }


      return (int)(pszEnd - pszDataBegin);
   }

}
