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
   static const char* FindString(const char* pszStr1, const char* pszStr2, const char* pszEnd)
   {
      pszEnd -= ::strlen(pszStr2);
      const char *s1, *s2;
      while (pszStr1 <= pszEnd)
      {
         s1 = pszStr1;
         s2 = pszStr2;
         while (*s1 == *s2 && *s2)
            s1++, s2++;
         if (!*s2)
            return pszStr1;
         pszStr1++;
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////////////////
   // MimeField class - Represents a field of a MIME body part header
   //////////////////////////////////////////////////////////////////////

   void MimeField::GetValue(string& strValue) const
   {
      string::size_type nEnd = m_strValue.find(';');
      if (nEnd != string::npos)
      {
         while (nEnd > 0 && CMimeChar::IsSpace((unsigned char)m_strValue[nEnd-1]))
            nEnd--;
         strValue.assign(m_strValue.c_str(), nEnd);
      }
      else
         strValue = m_strValue;
   }

   // set a parameter (attribute=value) of the field
   void MimeField::SetParameter(const char* pszAttr, const char* pszValue)
   {
      int nSize = pszValue ? (int)::strlen(pszValue) : 0;
      string strValue;
      strValue.reserve(nSize+3);
      if (!pszValue || *pszValue != '"')
         strValue = "\"";
      if (pszValue != NULL)
         strValue += pszValue;
      if (nSize < 2 || pszValue[nSize-1] != '"')
         strValue += "\"";

      bool encodedParameter;
      int nPos;
      if (!FindParameter(pszAttr, nPos, nSize, encodedParameter))	// add new parameter
      {
         m_strValue.reserve(m_strValue.size() + ::strlen(pszAttr) + strValue.size() + 5);
         //if (MimeEnvironment::AutoFolding())
         //	m_strValue += ";\r\n\t";
         //else
         //	m_strValue += "; ";
         m_strValue += "; ";
         m_strValue += pszAttr;
         m_strValue += '=';
         m_strValue += strValue;
      }
      else							// update existing parameter
         m_strValue.replace(nPos, nSize, strValue);
   }

   // get the value of a parameter
   bool MimeField::GetParameter(const char* pszAttr, AnsiString& strValue) const
   {
      strValue = "";

      bool encodedParameter = false;

      vector<AnsiString> parameters = StringParser::SplitString(AnsiString(m_strValue), ";");

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
         // 
         // For instance, this is perfectly valid 
         // Content-Type: text/plain; charset = "iso-8859-1"
         //
         for (; valuePos < value.GetLength(); valuePos++)
         {
            char c = value[valuePos];

            if (c == ' ' || c == '"')
               continue;
            else
               break;
         }

         // Locate the end of the value. The value may contain
         // pretty much any character, including space.
         int valueEndPos = valuePos;
         for (; valueEndPos < value.GetLength(); valueEndPos++)
         {
            char c = value[valueEndPos];

            if (c == ';' || c == '"')
               break;
            else
               continue;
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

   int MimeField::GetLength() const
   {
      int nLength = (int) m_strName.size() + 4;
      FieldCodeBase* pCoder = MimeEnvironment::CreateFieldCoder(GetName());
      pCoder->SetCharset(m_strCharset.c_str());
      pCoder->SetInput(m_strValue.c_str(), (int)m_strValue.size(), true);
      delete pCoder;
      return nLength;
   }

   // store a field to string buffer
   void MimeField::Store(AnsiString &output) const
   {
      output.append(m_strName);
      output.append(": ");

      FieldCodeBase* pCoder = MimeEnvironment::CreateFieldCoder(GetName());
      pCoder->SetCharset(m_strCharset.c_str());
      pCoder->SetInput(m_strValue.c_str(), (int)m_strValue.size(), true);
      pCoder->GetOutput(output);
      delete pCoder;

      output.append("\r\n");

   }

   // load a field from string buffer
   int MimeField::Load(const char* pszData, int nDataSize, bool unfold)
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

      // get the field name
      pszEnd = LineFind(pszStart, ':');
      if (pszEnd != NULL)				// if colon not found, Name would be empty
      {
         m_strName.assign(pszStart, (pszEnd-pszStart));
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

      // BEGIN change for hMailServer
      int lLength = (int)(pszEnd-pszStart)-2;
      char *pValue = new char[lLength + 1];
      memset(pValue, 0, lLength+1);
      strncpy(pValue, pszStart, lLength);
      m_strValue = pValue;
      delete [] pValue;

      // We need to unfold the field value
      if (unfold)
         UnfoldField(m_strValue);

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
      const char* pszParms = ::strchr(m_strValue.data(), ';');

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
         if (!pszParmEnd)			// non quoted string
         {
            pszParmEnd = pszParms;

            // Locate end of parameter value.
            while (CMimeChar::IsToken(*pszParmEnd) || (*pszParmEnd == '.'))
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
            nPos = (int)(pszParms - m_strValue.data());
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
      while (m_TypeTable[nIndex] != NULL &&
         ::_memicmp(pszType, m_TypeTable[nIndex], ::strlen(m_TypeTable[nIndex])) != 0)
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
         m_listFields.push_back(fd);
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
            while (m_TypeCvtTable[nIndex].nMediaType != MEDIA_UNKNOWN)
            {
               if (!::_stricmp(pszFileExt, m_TypeCvtTable[nIndex].pszFileExt))
               {
                  strType = m_TypeTable[m_TypeCvtTable[nIndex].nMediaType];
                  strType += '/';
                  strType += m_TypeCvtTable[nIndex].pszSubType;
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
         m_listFields.push_back(fd);
      }
      else
         pfd->SetParameter(CMimeConst::Name(), pszName);
   }

   // set 'boundary' parameter (for multipart) of Content-Type field
   void MimeHeader::SetBoundary(const char* pszBoundary/*=NULL*/)
   {
      static int s_nPartNumber = 0;
      char buf[80];
      if (!pszBoundary)				// generate a new boundary delimeter
      {
         ::srand(((unsigned)::time(NULL)) ^ (unsigned)this);
         ::sprintf(buf, "__=_Part_Boundary_%03d_%06d.%06d", ++s_nPartNumber, rand(), rand());
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
         m_listFields.push_back(fd);
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
      m_listFields.clear();
   }

   // return the length needed to store this header to string buffer
   int MimeHeader::GetLength() const
   {
      int nLength = 0;
      vector<MimeField>::const_iterator it;
      for (it = m_listFields.begin(); it != m_listFields.end(); it++)
         nLength += (*it).GetLength();
      return nLength + 2;				// a pair of CRLF indicate the end of header
   }


   void 
      MimeHeader::DeleteField(const AnsiString &fieldName)
   {
      MimeField *field = GetField(fieldName);
      if (field)
         DeleteField(field);
   }

   void 
      MimeHeader::DeleteField(MimeField *pField)
   {
      vector<MimeField>::iterator iter = m_listFields.begin(); 
      vector<MimeField>::const_iterator iterEnd = m_listFields.end();

      for (; iter != iterEnd; iter++)
      {
         const MimeField& fd = (*iter);

         if (&fd == pField)
         {
            m_listFields.erase(iter);
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
      vector<MimeField>::const_iterator it;
      for (it = m_listFields.begin(); it != m_listFields.end(); it++)
      {
         const MimeField& fd = *it;
         fd.Store(output);
      }

      output.append("\r\n");
   }


   // load a header from string buffer
   int MimeHeader::Load(const char* pszData, int nDataSize, bool unfold)
   {
      ASSERT(pszData != NULL);

      int nInput = 0;
      while (pszData[nInput] != 0 && pszData[nInput] != '\r')
      {
         MimeField fd;
         int nSize = fd.Load(pszData+nInput, nDataSize-nInput, unfold);
         if (nSize <= 0)
            return nSize;

         nInput += nSize;
         m_listFields.push_back(fd);	// don't use SetField in case of same name fields
      }

      return nInput + 2;				// skip the ending CRLF
   }

   // returns the encoded version of the header.
   AnsiString MimeHeader::GetHeaderContents()
   {
      AnsiString sResponse;
      // Iterate through the header fields and add to sResponse.
      std::vector<MimeField> oFields =  Fields();
      std::vector<MimeField>::iterator iterField = oFields.begin();

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
      std::vector<MimeField>::iterator iterField = oFields.begin();

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
      vector<MimeField>::iterator iter = FindField(pszFieldName);
      if (iter == m_listFields.end())
         return false;

      return true;
   }


   vector<MimeField>::iterator MimeHeader::FindField(const char* pszFieldName) const
   {
      vector<MimeField>::iterator it;
      MimeHeader *pThis = const_cast<MimeHeader*>(this);
      for (it = pThis->m_listFields.begin(); it != pThis->m_listFields.end(); it++)
      {
         MimeField& fd = *it;
         if (!::_stricmp(fd.GetName(), pszFieldName))
            break;
      }
      return it;
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
      m_pbText = sText;
      return 0;
   }

   // initialize the content with text
   int MimeBody::SetUnicodeText(const String &sText)
   {
      // Retrieve the current charset for this field.
      std::string strCharset = GetCharset();

      if (strCharset.size() == 0)
      {
         return SetRawText(sText);
      }

      AnsiString sMBText = Charset::ToMultiByte(sText, strCharset);
      AnsiString sEncodedValue;
      MimeCodeBase* pCoder = MimeEnvironment::CreateCoder("quoted-printable");
      pCoder->SetInput(sMBText, sMBText.GetLength(), true);
      pCoder->GetOutput(sEncodedValue);
      delete pCoder;   

      SetTransferEncoding("quoted-printable");

      return SetRawText(sEncodedValue);
   }   

   string MimeBody::GetRawText()
   {
      return m_pbText;
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

   shared_ptr<MimeBody> 
      MimeBody::LoadEncapsulatedMessage() const
   {
      // try to generate a file name using the message subject.
      shared_ptr<MimeBody> pEncapsulatedMessage = shared_ptr<MimeBody>(new MimeBody);

      int iLength = GetContentLength();
      char *pData = new char[iLength+1];
      strncpy(pData, (const char*) GetContent(), iLength);
      int index = 0;
      pEncapsulatedMessage->Load(pData, iLength, index);
      delete [] pData;

      return pEncapsulatedMessage;
   }

   String 
      MimeBody::GenerateFileNameFromEncapsulatedSubject(bool unicode) const
   {
      if (!IsEncapsulatedRFC822Message())
         return "";

      shared_ptr<MimeBody> pEncapsulatedMessage = LoadEncapsulatedMessage();

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
         sFilename.Format(_T("ATT-%d.dat"), m_iPartIndex);

      return sFilename;
   }

   String
      MimeBody::GetRawFilename() const
   {
      String sFilename = MimeHeader::GetRawFilename();

      if (sFilename.IsEmpty())
         sFilename = GenerateFileNameFromEncapsulatedSubject(false);

      if (sFilename.IsEmpty())
         sFilename.Format(_T("ATT-%d.dat"), m_iPartIndex);

      return sFilename;
   }


   // initialize the content of this body part with a mail message
   bool MimeBody::SetMessage(const MimeBody* pMM)
   {
      ASSERT(pMM != NULL);
      int nSize = pMM->GetLength();
      if (!AllocateBuffer(nSize+4))
         return false;

      pMM->Store(m_pbText);

      const char* pszType = GetContentType();
      if (!pszType || ::_memicmp(pszType, "message", 7) != 0)
         SetContentType("message/rfc822", "");

      return true;
   }

   void MimeBody::GetMessage(MimeBody* pMM) const
   {
      ASSERT(pMM != NULL);
      ASSERT(m_pbText != NULL);
      int index = 0;

      pMM->Load((const char*)m_pbText, (int) m_pbText.size(), index);
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
      shared_ptr<ByteBuffer> pFileContents = oFile.ReadTextFile();

      FreeBuffer();
      if (pFileContents->GetSize() > 0)
      {
         try
         {
            // Minus one, since the last character is the null...
            int index = 0;
            Load(pFileContents->GetCharBuffer(), pFileContents->GetSize() - 1, index);
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

   // write the entire header and content to file.
   bool MimeBody::SaveAllToFile(const AnsiString & pszFilename)
   {
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

      shared_ptr<ByteBuffer> pUnencodedBuffer = oFile.ReadFile();

      if (!pUnencodedBuffer)
         return false;

      // Encode the file, to base64 or likewise.
      MimeCodeBase* pCoder = MimeEnvironment::CreateCoder(GetTransferEncoding());
      ASSERT(pCoder != NULL);
      pCoder->SetInput((const char*) pUnencodedBuffer->GetCharBuffer(), pUnencodedBuffer->GetSize(), true);

      // Copy the buffer
      pCoder->GetOutput(m_pbText);

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
      pCoder->SetInput(m_pbText, m_pbText.GetLength(), false);

      AnsiString decoded;
      pCoder->GetOutput(decoded);

      FileUtilities::WriteToFile(sFilename, decoded);

      return true;
   }

   // delete all child body parts
   void MimeBody::DeleteAll()
   {
      while (!m_listBodies.empty())
      {
         shared_ptr<MimeBody> pBP = m_listBodies.back();
         m_listBodies.pop_back();
         ASSERT(pBP != NULL);
      }
   }

   // create a new child body part, and add it to body part list
   shared_ptr<MimeBody> MimeBody::CreatePart(const char* pszMediaType/*=NULL*/, shared_ptr<MimeBody> pWhere/*=NULL*/)
   {
      shared_ptr<MimeBody> pBP = MimeEnvironment::CreateBodyPart(pszMediaType);
      ASSERT(pBP != NULL);
      if (pWhere != NULL)
      {
         for (BodyList::iterator it = m_listBodies.begin(); it != m_listBodies.end(); it++)
            if (*it == pWhere)
            {
               m_listBodies.insert(it, pBP);
               return pBP;
            }
      }
      m_listBodies.push_back(pBP);
      return pBP;
   }

   // create a new child body part, and add it to body part list
   void MimeBody::AddPart(shared_ptr<MimeBody> part)
   {
      m_listBodies.push_back(part);
   }

   int 
      MimeBody::GetPartCount()
   {
      return (int) m_listBodies.size();
   }
   // remove and delete a child body part
   void MimeBody::ErasePart(shared_ptr<MimeBody> pBP)
   {
      ASSERT(pBP != NULL);
      m_listBodies.remove(pBP);
   }

   // return a list of all attachment body parts belong to this body part
   // Since we are using smart pointers, and it's not possible to cast
   // from <this> to a smart_ptr, we need to give this function a pointer
   // to itself. Really ugly but should work fine.
   int MimeBody::GetAttachmentList(shared_ptr<MimeBody> pThis, BodyList& rList) const
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
         list<shared_ptr<MimeBody> >::const_iterator it;
         for (it=m_listBodies.begin(); it!=m_listBodies.end(); it++)
         {
            shared_ptr<MimeBody> pBP = *it;
            ASSERT(pBP != NULL);
            nCount += pBP->GetAttachmentList(pBP, rList);
         }
      }
      return nCount;
   }

   // clear all attachments from this subtype.
   void MimeBody::ClearAttachments() 
   {
      if (GetMediaType() ==MEDIA_MULTIPART)
      {
         list<shared_ptr<MimeBody> >::iterator it = m_listBodies.begin();
         while (it != m_listBodies.end())
         {
            shared_ptr<MimeBody> pBody = (*it);
            if (pBody->IsAttachment())
               it = m_listBodies.erase(it);
            else
               it++;
         }
      }
   }

   // clear all attachments from this subtype.
   void MimeBody::RemoveAttachment(shared_ptr<MimeBody> pAttachment) 
   {
      if (GetMediaType() ==MEDIA_MULTIPART)
      {
         list<shared_ptr<MimeBody> >::iterator it = m_listBodies.begin();
         while (it != m_listBodies.end())
         {
            shared_ptr<MimeBody> pBody = (*it);
            if (pBody->IsAttachment() && pBody == pAttachment)
            {
               it = m_listBodies.erase(it);
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
      m_itFind = m_listBodies.end();
      FreeBuffer();
      MimeHeader::Clear();
   }

   // return the length needed to store this body part 
   // in an de-coded buffer
   int MimeBody::GetLength(bool bIncludeHeader) const
   {
      int nLength = 0;

      if (bIncludeHeader)
         nLength += MimeHeader::GetLength();

      if (m_listBodies.empty())
         return nLength;

      string strBoundary = GetBoundary();
      int nBoundSize = (int) strBoundary.size();
      list<shared_ptr<MimeBody> >::const_iterator it;
      for (it=m_listBodies.begin(); it!=m_listBodies.end(); it++)
      {
         nLength += nBoundSize + 6;	// include 2 leading hyphens and 2 pair of CRLFs
         shared_ptr<MimeBody> pBP = *it;
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
      int nSize = 0;

      if (bIncludeHeader)
         MimeHeader::Store(output);

      // Copy the data to the output buffer. 
      output.append(m_pbText);

      if (m_listBodies.empty())
         return;

      // store child body parts
      string strBoundary = GetBoundary();
      if (strBoundary.empty())
         return;					// boundary not be set

      int nBoundSize = (int)strBoundary.size() + 6;
      for (BodyList::const_iterator it=m_listBodies.begin(); it!=m_listBodies.end(); it++)
      {
         // If the initial body ends with \r\n, remove them. We add new ones below.
         if (m_listBodies.begin() == it && output.size() >= 2 && 
            output[output.size()-2] == '\r' && output[output.size()-1] == '\n')
         {
            output = output.Mid(0, output.GetLength() - 2);
         }

         AnsiString boundaryLine = Formatter::Format(_T("\r\n--{0}\r\n"), String(strBoundary));
         output.append(boundaryLine);

         shared_ptr<MimeBody> pBP = *it;
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
         assert(0);
         return 0;
      }

      const char* possibleEnding = FindString(startSearch, boundary.c_str(), endSearch);

      int counter = 1000;
      while (possibleEnding != 0 && counter > 0)
      {
         counter--;

         // return if the string after the boundary is either a newline, or a --.
         // this is to prevent the problem that we return incorrect boundaries
         // if the boundary string is a part of another boundary string.
         int sizeRemainingAfterBoundaryString = endSearch - possibleEnding;
         if (sizeRemainingAfterBoundaryString <= 2)
         {
            // malformed message. the end of the character string is the boundary line with no trailing crlf or --.
            assert(0);
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

      assert(0);
      return 0;
   }

   // load a body part from string buffer
   int MimeBody::Load(const char* pszData, int nDataSize, int &index)
   {
      index++;
      m_iPartIndex = index;

      // load header fields
      int nSize = MimeHeader::Load(pszData, nDataSize, true);
      if (nSize <= 0)
         return nSize;

      const char* pszDataBegin = pszData;	// preserve start position
      pszData += nSize;
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
            m_pbText.append(pszData, nSize);

            pszData += nSize;
            nDataSize -= nSize;
         }
         else
            return -1;
      }
      if (nDataSize <= 0)
         return (int)(pszData - pszDataBegin);

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
         const char* pszStart = FindString(pszBound1+2, "\r\n", pszEnd);
         if (!pszStart)
            break;
         pszStart += 2;
         if (pszBound1[strBoundary.size()] == '-' && pszBound1[strBoundary.size()+1] == '-')
            return (int)(pszStart - pszDataBegin);	// reach the closing boundary

         // look for the next boundary
         string strBoundaryLine = strBoundary + "\r\n";

         const char* pszBound2 = GetBoundaryEnd(pszStart, pszEnd, strBoundary.c_str());

         if (!pszBound2)				// overflow, boundary may be truncated
            pszBound2 = pszEnd;
         int nEntitySize = (int) (pszBound2 - pszStart);

         shared_ptr<MimeBody> pBP = shared_ptr<MimeBody>(new MimeBody());

         m_listBodies.push_back(pBP);

         int nInputSize = pBP->Load(pszStart, nEntitySize, m_iPartIndex);
         if (nInputSize < 0)
         {
            ErasePart(pBP);
            return nInputSize;
         }
         pszBound1 = pszBound2;
      }
      return (int)(pszEnd - pszDataBegin);
   }

}
