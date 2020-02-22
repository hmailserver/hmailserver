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

#include "../Util/Unicode.h"

#ifdef _DEBUG
   #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
   #define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// MimeEnvironment - global environment to manage encoding/decoding
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool MimeEnvironment::auto_folding_ = false;
   string MimeEnvironment::charset_;
   std::list<MimeEnvironment::CODER_PAIR> MimeEnvironment::coders_;
   std::list<MimeEnvironment::FIELD_CODER_PAIR> MimeEnvironment::field_coders_;
   std::list<MimeEnvironment::MEDIA_TYPE_PAIR> MimeEnvironment::media_types_;
   MimeEnvironment MimeEnvironment::mgr_;

   MimeEnvironment::MimeEnvironment()
   {
	   // initialize transfer encoding
	   //REGISTER_MIMECODER("7bit", MimeCode7bit);
	   //REGISTER_MIMECODER("8bit", MimeCode7bit);
	   REGISTER_MIMECODER("quoted-printable", MimeCodeQP);
	   REGISTER_MIMECODER("base64", MimeCodeBase64);

	   // initialize header fields encoding
	   REGISTER_FIELDCODER("Subject", FieldCodeText);
	   REGISTER_FIELDCODER("Comments", FieldCodeText);
	   REGISTER_FIELDCODER("Content-Description", FieldCodeText);

	   REGISTER_FIELDCODER("From", FieldCodeAddress);
	   REGISTER_FIELDCODER("To", FieldCodeAddress);
	   REGISTER_FIELDCODER("Resent-To", FieldCodeAddress);
	   REGISTER_FIELDCODER("Cc", FieldCodeAddress);
	   REGISTER_FIELDCODER("Resent-Cc", FieldCodeAddress);
	   REGISTER_FIELDCODER("Bcc", FieldCodeAddress);
	   REGISTER_FIELDCODER("Resent-Bcc", FieldCodeAddress);
	   REGISTER_FIELDCODER("Reply-To", FieldCodeAddress);
	   REGISTER_FIELDCODER("Resent-Reply-To", FieldCodeAddress);

	   REGISTER_FIELDCODER("Content-Type", FieldCodeParameter);
	   REGISTER_FIELDCODER("Content-Disposition", FieldCodeParameter);
   }

   void MimeEnvironment::SetAutoFolding(bool bAutoFolding)
   {
	   auto_folding_ = bAutoFolding;
	   if (!bAutoFolding)
	   {
		   DEREGISTER_MIMECODER("7bit");
		   DEREGISTER_MIMECODER("8bit");
	   }
	   else
	   {
		   REGISTER_MIMECODER("7bit", MimeCode7bit);
		   REGISTER_MIMECODER("8bit", MimeCode7bit);
	   }
   }

   void MimeEnvironment::RegisterCoder(const char* pszCodingName, CODER_FACTORY pfnCreateObject/*=NULL*/)
   {
	   ASSERT(pszCodingName != NULL);
	   auto it = coders_.begin();
	   while (it != coders_.end())
	   {
		   auto it2 = it;
		   it++;
         if (!::_stricmp(pszCodingName, (*it2).first))
			   coders_.erase(it2);
	   }
	   if (pfnCreateObject != NULL)
	   {
		   CODER_PAIR newPair(pszCodingName, pfnCreateObject);
		   coders_.push_front(newPair);
	   }
   }

   MimeCodeBase* MimeEnvironment::CreateCoder(const char* pszCodingName)
   {
	   if (!pszCodingName || !::strlen(pszCodingName))
		   pszCodingName = "7bit";

	   for (auto it=coders_.begin(); it!=coders_.end(); it++)
	   {
		   ASSERT((*it).first != NULL);
		   if (!::_stricmp(pszCodingName, (*it).first))
		   {
			   CODER_FACTORY pfnCreateObject = (*it).second;
			   ASSERT(pfnCreateObject != NULL);
			   return pfnCreateObject();
		   }
	   }
	   return new MimeCodeBase;		// default coder for unregistered Content-Transfer-Encoding
   }


   void MimeEnvironment::RegisterFieldCoder(const char* pszFieldName, FIELD_CODER_FACTORY pfnCreateObject/*=NULL*/)
   {
	   ASSERT(pszFieldName != NULL);
	   auto it = field_coders_.begin();
	   while (it != field_coders_.end())
	   {
		   auto it2 = it;
		   it++;
		   if (!::_stricmp(pszFieldName, (*it2).first))
			   field_coders_.erase(it2);
	   }
	   if (pfnCreateObject != NULL)
	   {
		   FIELD_CODER_PAIR newPair(pszFieldName, pfnCreateObject);
		   field_coders_.push_front(newPair);
	   }
   }

   FieldCodeBase* MimeEnvironment::CreateFieldCoder(const char* pszFieldName)
   {
	   ASSERT(pszFieldName != NULL);
	   for (auto it=field_coders_.begin(); it!=field_coders_.end(); it++)
	   {
		   ASSERT((*it).first != NULL);
		   if (::_stricmp(pszFieldName, (*it).first))
		   {
			   FIELD_CODER_FACTORY pfnCreateObject = (*it).second;
			   ASSERT(pfnCreateObject != NULL);
			   return pfnCreateObject();
		   }
	   }
	   return new FieldCodeBase;		// default coder for unregistered header fields
   }

   void MimeEnvironment::RegisterMediaType(const char* pszMediaType, BODY_PART_FACTORY pfnCreateObject/*=NULL*/)
   {
	   ASSERT(pszMediaType != NULL);
	   auto it = media_types_.begin();
	   while (it != media_types_.end())
	   {
		   auto it2 = it;
		   it++;
		   if (!::_stricmp(pszMediaType, (*it2).first))
			   media_types_.erase(it2);
	   }
	   if (pfnCreateObject != NULL)
	   {
		   MEDIA_TYPE_PAIR newPair(pszMediaType, pfnCreateObject);
		   media_types_.push_front(newPair);
	   }
   }

   std::shared_ptr<MimeBody> MimeEnvironment::CreateBodyPart(const char* pszMediaType)
   {
	   if (!pszMediaType || !::strlen(pszMediaType))
		   pszMediaType = "text";

	   ASSERT(pszMediaType != NULL);
	   for (auto it=media_types_.begin(); it!=media_types_.end(); it++)
	   {
		   ASSERT((*it).first != NULL);
		   if (!::_stricmp(pszMediaType, (*it).first))
		   {
			   BODY_PART_FACTORY pfnCreateObject = (*it).second;
			   ASSERT(pfnCreateObject != NULL);
			   return pfnCreateObject();
		   }
	   }
      
      std::shared_ptr<MimeBody> pBody = std::shared_ptr<MimeBody>(new MimeBody);
      return pBody;
      //return new MimeBody;			// default body part for unregistered media type
   }

   //////////////////////////////////////////////////////////////////////
   // MimeCode7bit - for 7bit/8bit encoding mechanism (fold long line)
   //////////////////////////////////////////////////////////////////////
   void MimeCode7bit::Encode(AnsiString &output) const
   {
	   const unsigned char* pbData = input_;
	   const unsigned char* pbEnd = input_ + input_size_;
	   unsigned char* pbSpace = NULL;
	   int nLineLen = 0;
      int lastSpacePos = -1;
	   
      while (pbData < pbEnd)
	   {
		   unsigned char ch = *pbData;
         
		   if (ch == '\r' || ch == '\n')
		   {
			   nLineLen = -1;
			   lastSpacePos = -1;
		   }
		   else if (nLineLen > 0 && CMimeChar::IsSpace(ch))
         {
            lastSpacePos = (int) output.size();
         }

		   // fold the line if it's longer than 76
		   if (nLineLen >= MAX_MIME_LINE_LEN && lastSpacePos != -1)
		   {
            int charsSinceSpace = (int) output.size() - lastSpacePos;
            output.Insert(lastSpacePos, "\r\n");

            lastSpacePos = -1;
            nLineLen = charsSinceSpace + 1;
		   }

         output.append(1, ch);

         pbData++;
		   nLineLen++;
	   }
   }

   //////////////////////////////////////////////////////////////////////
   // MimeCodeQP - for quoted-printable encoding mechanism
   //////////////////////////////////////////////////////////////////////
   void MimeCodeQP::Encode(AnsiString &output) const
   {
	   static const char* s_QPTable = "0123456789ABCDEF";

	   const unsigned char* pbData = input_;
	   const unsigned char* pbEnd = input_ + input_size_;
	   int nLineLen = 0;
   
      int lastSpacePos = -1;

	   while (pbData < pbEnd)
	   {
		   unsigned char ch = *pbData;
		   bool bQuote = false, bCopy = false;

		   // According to RFC 2045, TAB and SPACE MAY be represented as the ASCII characters.
		   // But it MUST NOT be so represented at the end of an encoded line.
		   if (ch == '\t' || ch == ' ')
		   {
			   if (pbData == pbEnd-1 || (!quote_line_break_ && *(pbData+1) == '\r'))
				   bQuote = true;		// quote the SPACE/TAB
			   else
				   bCopy = true;		// copy the SPACE/TAB
			   
            if (nLineLen > 0)
            {
               lastSpacePos = (int) output.size();
            }
		   }
		   else if (!quote_line_break_ && (ch == '\r' || ch == '\n'))
		   {
			   bCopy = true;			// keep 'hard' line break
			   nLineLen = -1;
			   lastSpacePos = -1;
		   }
		   else if (!quote_line_break_ && ch == '.')
		   {
			   if (pbData-input_ >= 2 &&
				   *(pbData-2) == '\r' && *(pbData-1) == '\n' &&
				   *(pbData+1) == '\r' && *(pbData+2) == '\n')
				   bQuote = true;		// avoid confusing with SMTP's message end flag
			   else
				   bCopy = true;
		   }
		   else if (ch < 33 || ch > 126 || ch == '=')
			   bQuote = true;			// quote this character
		   else
			   bCopy = true;			// copy this character

         if (add_line_break_)
         {
            if (nLineLen+(bQuote ? 3 : 1) >= MAX_MIME_LINE_LEN)
		      {
			      if (lastSpacePos != -1)
			      {
                  // TODO: Implement?!
                  ASSERT(0);
                  /*lastSpacePos++;
				      int nSize = output.size() - lastSpacePos;
                  
                  output = output.Mid(0, lastSpacePos+2) + output.Mid(;
                  

				      ::memmove(pbSpace+3, pbSpace, nSize);
				      nLineLen = nSize;*/
			      }
			      else
			      {
                  lastSpacePos = (int) output.size();
				      //pbSpace = pbOutput;
				      nLineLen = 0;
			      }

               output.append("=\r\n");
		      }
         }

		   if (bQuote)
		   {
            output.append("=");
            output.append(1, s_QPTable[(ch >> 4) & 0x0f]);
            output.append(1, s_QPTable[ch & 0x0f]);
			   nLineLen += 3;
		   }
		   else if (bCopy)
		   {
            output.append(1, ch);
			   nLineLen++;
		   }

		   pbData++;
	   }

   }

   void MimeCodeQP::Decode(AnsiString &output)
   {
	   const unsigned char* pbData = input_;
	   const unsigned char* pbEnd = input_ + input_size_;

	   while (pbData < pbEnd)
	   {
		   unsigned char ch = *pbData++;
		   if (ch == '=')
		   {
			   if (pbData+2 > pbEnd)
				   break;				// invalid endcoding
			   ch = *pbData++;
			   if (CMimeChar::IsHexDigit(ch))
			   {
				   ch -= ch > '9' ? 0x37 : '0';
               unsigned char charToAdd = ch << 4;
				   ch = *pbData++;
				   ch -= ch > '9' ? 0x37 : '0';
               charToAdd |= ch & 0x0f;

               output.append(1, charToAdd);
			   }
			   else if (ch == '\r' && *pbData == '\n')
				   pbData++;			// Soft Line Break, eat it
			   else					// invalid endcoding, let it go
				   output.append(1, ch);
		   }
         else
         {
            output.append(1, ch);
         }
			   
	   }
   }

   void MimeCodeQ::Decode(AnsiString &output)
   {
      const unsigned char* pbData = input_;
      const unsigned char* pbEnd = input_ + input_size_;

      while (pbData < pbEnd)
      {
         unsigned char ch = *pbData++;
         if (ch == '=')
         {
            if (pbData+2 > pbEnd)
               break;				// invalid endcoding
            ch = *pbData++;
            if (CMimeChar::IsHexDigit(ch))
            {
               ch -= ch > '9' ? 0x37 : '0';
               unsigned char charToAdd = ch << 4;
               ch = *pbData++;
               ch -= ch > '9' ? 0x37 : '0';
               charToAdd |= ch & 0x0f;

               output.append(1, charToAdd);
            }
            else if (ch == '\r' && *pbData == '\n')
               pbData++;			// Soft Line Break, eat it
            else					// invalid endcoding, let it go
               output.append(1, ch);
         }
         else
         {
            if (ch == '_')
            {
               // Why are we here? 
               // Q is similar to QP encoding, except for the fact
               // that spaces are replaced with _ when encoded.
               ch = ' ';
            }

            output.append(1, ch);
         }

      }
   }

   //////////////////////////////////////////////////////////////////////
   // MimeCodeBase64 - for base64 encoding mechanism
   //////////////////////////////////////////////////////////////////////
   void MimeCodeBase64::Encode(AnsiString &result) const
   {
	   static const char* s_Base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	   size_t nFrom, nLineLen = 0;
	   unsigned char chHigh4bits = 0;

	   for (nFrom=0; nFrom<input_size_; nFrom++)
	   {
		   unsigned char ch = input_[nFrom];
		   switch (nFrom % 3)
		   {
		   case 0:
            {
               char c = s_Base64Table[ch >> 2];
               result.append(1, c);

			      chHigh4bits = (ch << 4) & 0x30;
			      break;
            }

         case 1:
            {
               char c = s_Base64Table[chHigh4bits | (ch >> 4)];
               result.append(1, c);
			      chHigh4bits = (ch << 2) & 0x3c;
			      break;
            }

		   default:
            {
               char c = s_Base64Table[chHigh4bits | (ch >> 6)];
               result.append(1, c);

               c = s_Base64Table[ch & 0x3f];
               result.append(1, c);

			      nLineLen++;
            }
		   }

		   nLineLen++;

		   if (add_line_break_ && nLineLen >= MAX_MIME_LINE_LEN)
		   {
            result.append("\r\n");
			   nLineLen = 0;
		   }
	   }

	   if (nFrom % 3 != 0)	// 76 = 19 * 4, so the padding wouldn't exceed 76
	   {
         char c1 = s_Base64Table[chHigh4bits];

         result.append(1, c1);
		   int nPad = 4 - (nFrom % 3) - 1;
         result.append(nPad, '=');
	   }

	   if (add_line_break_ && nLineLen != 0)	// add CRLF
         result.append("\r\n");
	}

   void MimeCodeBase64::Decode(AnsiString &result)
   {
	   const unsigned char* pbData = input_;
	   const unsigned char* pbEnd = input_ + input_size_;

	   int nFrom = 0;
	   unsigned char chHighBits = 0;

	   while (pbData < pbEnd)
	   {
		   unsigned char ch = *pbData++;
		   if (ch == '\r' || ch == '\n')
			   continue;
		   ch = (unsigned char) DecodeBase64Char(ch);
		   if (ch >= 64)				// invalid encoding, or trailing pad '='
			   break;

		   switch ((nFrom++) % 4)
		   {
		   case 0:
			   chHighBits = ch << 2;
			   break;

		   case 1:
            {
               char charToAdd = (chHighBits | (ch >> 4));
               result.append(1, charToAdd);
               chHighBits = ch << 4;
               break;
            }
		   case 2:
            {
               char charToAdd = (chHighBits | (ch >> 2));
               result.append(1, charToAdd);
               chHighBits = ch << 6;
               break;
            }
		   default:
            {
               char charToAdd = (chHighBits | ch);
               result.append(1, charToAdd);
               break;
            }
            
		   }
	   }
   }

   //////////////////////////////////////////////////////////////////////
   // MimeEncodedWord - encoded word for non-ascii text (RFC 2047)
   //////////////////////////////////////////////////////////////////////
   void MimeEncodedWord::Encode(AnsiString &output) const
   {
	   if (charset_.empty())
      {
         MimeCodeBase::Encode(output);   
         return;
      }

	   if (!input_size_)
		   return;

      if (tolower(encoding_) == 'b')
      {
		   BEncode(output);
         return;
      }

	   QEncode(output);
   }


   void MimeEncodedWord::Decode(AnsiString &output)
   {
	   charset_.clear();
	   const char* pbData = (const char*) input_;
	   const char* pbEnd = pbData + input_size_;
	   
	   while (pbData < pbEnd)
	   {
		   const char* pszHeaderEnd = pbData;
		   const char* pszCodeEnd = pbEnd;
		   int nCoding = 0, nCodeLen = (int)(pbEnd - pbData);
		   if (pbData[0] == '=' && pbData[1] == '?')	// it might be an encoded-word
		   {
			   pszHeaderEnd = ::strchr(pbData+2, '?');
			   if (pszHeaderEnd != NULL && pszHeaderEnd[2] == '?' && pszHeaderEnd+3 < pbEnd)
			   {
				   nCoding = tolower(pszHeaderEnd[1]);
				   pszHeaderEnd += 3;

               int iDataLength = (int) (pbEnd - pszHeaderEnd);

               pszCodeEnd = StringParser::Search(pszHeaderEnd, iDataLength, "?=");	// look for the tailer
				   if (!pszCodeEnd || pszCodeEnd >= pbEnd)
					   pszCodeEnd = pbEnd;
				   nCodeLen = (int)(pszCodeEnd - pszHeaderEnd);
				   pszCodeEnd += 2;
				   if (charset_.empty())
				   {
					   charset_.assign(pbData+2, pszHeaderEnd-pbData-5);
					   encoding_ = nCoding;
				   }
			   }
		   }

         
		   int nDecoded;
		   if (nCoding == 'b')
		   {
			   MimeCodeBase64 base64;
			   base64.SetInput(pszHeaderEnd, nCodeLen, false);
			   base64.GetOutput(output);
		   }
		   else if (nCoding == 'q')
		   {
			   MimeCodeQ qp;
			   qp.SetInput(pszHeaderEnd, nCodeLen, false);
			   qp.GetOutput(output);
		   }
		   else
		   {
            int iDataLength = (int) (pbEnd - pbData);
            
			   pszCodeEnd = StringParser::Search(pbData+1, iDataLength - 1, "=?");	// find the next encoded-word
			   if (!pszCodeEnd || pszCodeEnd >= pbEnd)
				   pszCodeEnd = pbEnd;
			   else if (pbData > (const char*) input_)
			   {
				   const char* pszSpace = pbData;
				   while (CMimeChar::IsSpace((unsigned char)*pszSpace))
					   pszSpace++;
				   if (pszSpace == pszCodeEnd)	// ignore liner-white-spaces between adjacent encoded words
					   pbData = pszCodeEnd;
			   }
			   
            nDecoded = (int)(pszCodeEnd - pbData);
            output.append((char*) pbData, nDecoded);
			   
		   }

		   pbData = pszCodeEnd;
	   }
   }

   void MimeEncodedWord::BEncode(AnsiString &output) const
   {
      bool inputIsUTF8 = charset_.CompareNoCase("utf-8") == 0;
	   int nCharsetLen = (int)charset_.size();
	   int nMaxBlockSize = MAX_ENCODEDWORD_LEN - nCharsetLen - 7;	// a single encoded-word cannot exceed 75 bytes
	   nMaxBlockSize = nMaxBlockSize / 4 * 3;
	   ASSERT(nMaxBlockSize > 0);

	   size_t processedBytes = 0;

      bool firstWord = true;

	   for (;;)
	   {
         const unsigned char *thisPartStartPosition = input_ + processedBytes;
         
         // Calculate the number of input bytes to encode. It must be less than the max block size, and it must
         // cover an integral number of characters in the input stream. We can't encode 'half'
         // a chinese character into one stream, and the second half into the next.
         unsigned char* endChar = (unsigned char*) thisPartStartPosition;

         // currentSafeChar points at a character we know are within limits.
         unsigned char* currentSafeChar = endChar;

         for (; endChar < (unsigned char*) input_ + input_size_; )
         {
            endChar = Unicode::CharMoveNext(endChar, inputIsUTF8);

            int movedBytes = (int) (endChar - thisPartStartPosition);
            if (movedBytes > nMaxBlockSize)
            {
               // There's a limit on the max number of encoded bytes in a string
               // and we've reached it. We need to encode the current data, and
               // continue with the remaining in a new encoding section.
               break;
            }

            currentSafeChar = endChar;
         }

         size_t currentEncodeBlockSize = (currentSafeChar - thisPartStartPosition);

         output.append("=?");
         output.append(charset_);
         output.append("?B?");

         size_t inputEncodeSize = min(input_size_ - processedBytes, currentEncodeBlockSize);

         assert(inputEncodeSize == currentEncodeBlockSize);

		   MimeCodeBase64 base64;
		   base64.SetInput((const char*) thisPartStartPosition, inputEncodeSize, true);
		   base64.AddLineBreak(false);
		   base64.GetOutput(output);
		   output.append("?=");

		   processedBytes += inputEncodeSize;
		   if (processedBytes >= input_size_)
			   break;

         output.append(" ");
	   }
   }

   void MimeEncodedWord::QEncode(AnsiString &output) const
   {
	   static const char* s_QPTable = "0123456789ABCDEF";

	   const unsigned char* pbData = input_;
	   const unsigned char* pbEnd = input_ + input_size_;

      int nCodeLen, nCharsetLen = (int)charset_.size();
	   int nLineLen = 0, nMaxLine = MAX_ENCODEDWORD_LEN - nCharsetLen - 7;

	   while (pbData < pbEnd)
	   {
		   unsigned char ch = *pbData++;
		   if (ch < 33 || ch > 126 || ch == '=' || ch == '?' || ch == '_')
			   nCodeLen = 3;
		   else
			   nCodeLen = 1;

		   if (nLineLen+nCodeLen > nMaxLine)	// add encoded word tailer
		   {
			   output.append("?= ");
			   nLineLen = 0;
		   }

		   if (!nLineLen)				// add encoded word header
		   {
			   output.append("=?");
            output.append(charset_);
            output.append("?Q?");
		   }

		   nLineLen += nCodeLen;

         if (nCodeLen > 1)
		   {
            output.append("=");
            output.append(1, s_QPTable[(ch >> 4) & 0x0f]);
            output.append(1, s_QPTable[ch & 0x0f]);
		   }
		   else
			   output.append(1, ch);
	   }

      output.append("?=");
   }

   //////////////////////////////////////////////////////////////////////
   // FieldCodeBase - base class to encode/decode header fields
   // default coder for any unregistered fields
   //////////////////////////////////////////////////////////////////////
   void FieldCodeBase::Encode(AnsiString &output) const
   {

	   // use the global charset if there's no specified charset
	   string strCharset = charset_;
	   if (strCharset.empty())
		   strCharset = MimeEnvironment::GetGlobalCharset();
	   if (strCharset.empty() && !MimeEnvironment::AutoFolding())
		   return MimeCodeBase::Encode(output);

	   const char* pszInput = (const char*) input_;
      size_t nInputSize = input_size_;
	   int nNonAsciiChars, nDelimeter = GetDelimeter();
	   int nLineLen = 0;
	   
      AnsiString strUnit;

	   // divide the field into syntactic units to encode
	   for (;;)
	   {
		   size_t nUnitSize = FindSymbol(pszInput, nInputSize, nDelimeter, nNonAsciiChars);
		   if (!nNonAsciiChars || strCharset.empty())
         {
			   strUnit.assign(pszInput, nUnitSize);
         }
		   else
		   {
            strUnit = "";
			   MimeEncodedWord coder;
			   coder.SetEncoding(SelectEncoding(nUnitSize, nNonAsciiChars), strCharset.c_str());
			   coder.SetInput(pszInput, nUnitSize, true);
            coder.GetOutput(strUnit);
		   }
		   if (nUnitSize < nInputSize)
			   strUnit += pszInput[nUnitSize];		// add the following delimeter (space or special char)

		   // copy the encoded string to target buffer and perform folding if needed
		   if (!MimeEnvironment::AutoFolding())
		   {
            output.append(strUnit);
		   }
		   else
		   {
			   const char* pszData = strUnit.c_str();
			   const char* pszEnd = pszData + strUnit.size();

            int lastSpacePos = -1;

			   while (pszData < pszEnd)
			   {
				   char ch = *pszData++;
				   if (ch == '\r' || ch == '\n')
				   {
					   nLineLen = -1;
					   lastSpacePos = -1;
				   }
				   else if (nLineLen > 0 && CMimeChar::IsSpace(ch))
               {
					   lastSpacePos = (int) output.size();
               }

				   if (nLineLen >= MAX_MIME_LINE_LEN && lastSpacePos != -1)		// fold at the position of the previous space
				   {
                  /*
                     2008-12-04
                     Before we used to fold using \t. Now we fold with a whitespace instead.
                     This change was made as a workaround to Thunderbird bug 
                     https://bugzilla.mozilla.org/show_bug.cgi?id=460443
                  */

                  int charsSinceSpace = (int) output.size() - lastSpacePos;
                  output.Insert(lastSpacePos, "\r\n");
                  
   				   lastSpacePos = -1;
					   nLineLen = charsSinceSpace + 1;
				   }
				   
               output.append(1, ch);

				   nLineLen++;
			   }
		   }

		   pszInput += nUnitSize + 1;

         if (nInputSize >= nUnitSize + 1)
		      nInputSize -= nUnitSize + 1;
         else
            break;

         // Martin: Commented. If the header contained a ; we always folded
         // the header. If the content of the email was PGP encoded, this had
         // the effect that the content was changed and the checksum became invalid.

         // fold at the position of the specific char and eat the following spaces
         /*if (IsFoldingChar(pszInput[-1]) && pbOutput+3 <= pbOutEnd)
		   {
			   ::memcpy(pbOutput, "\r\n\t", 3);
			   pbOutput += 3;
			   pbSpace = NULL;
			   nLineLen = 1;
			   while (nInputSize > 0 && CMimeChar::IsSpace(*pszInput))
			   {
				   pszInput++;
				   nInputSize--;
			   }
		   }
         */
	   }
   }

   void FieldCodeBase::Decode(AnsiString &output)
   {
	   MimeEncodedWord coder;
	   coder.SetInput((const char*)input_, input_size_, false);

	   AnsiString field;
	   coder.GetOutput(field);

	   charset_ = coder.GetCharset();

	   if (MimeEnvironment::AutoFolding())
		   UnfoldField(field);

      output.append(field);
   }

   void FieldCodeBase::UnfoldField(string& strField) const
   {
	   for (;;)
	   {
		   string::size_type pos = strField.rfind("\r\n");
		   if (pos == string::npos)
			   break;

		   strField.erase(pos, 2);

		   int nSpaces = 0;
		   while (CMimeChar::IsSpace((unsigned char)strField[pos+nSpaces]))
			   nSpaces++;
		   strField.replace(pos, nSpaces, " ");
	   }
   }

   int FieldCodeBase::FindSymbol(const char* pszData, size_t nSize, int& nDelimeter, int& nNonAscChars) const
   {
	   nNonAscChars = 0;
	   const char* pszDataStart = pszData;
	   const char* pszEnd = pszData + nSize;

	   while (pszData < pszEnd)
	   {
		   char ch = *pszData;
		   if (CMimeChar::IsNonAscii((unsigned char)ch))
			   nNonAscChars++;
		   else
		   {
			   if (ch == (char) nDelimeter)
			   {
				   nDelimeter = 0;		// stop at any delimeters (space or specials)
				   break;
			   }

            /*
            We used to stop at Space here, but spaces should be encoded.

            RFC 2047: http://www.faqs.org/rfcs/rfc2047.html
            The notation of RFC 822 is used, with the exception that white space
            characters MUST NOT appear between components of an 'encoded-word'.
            */
            if (!nDelimeter && CMimeChar::IsSpecial(ch))
			   {
               if (pszData > pszDataStart)
               {
                  char previousChar = *(pszData -1);
                  if (CMimeChar::IsSpace(previousChar))
                  {
                     pszData--;
                     nDelimeter = ' ';
                  }
               }
               
               if (nDelimeter == 0 )
               {
				      switch (ch)
				      {
				      case '"':
					      nDelimeter = '"';	// quoted-string, delimeter is '"'
					      break;
				      case '(':
					      nDelimeter = ')';	// comment, delimeter is ')'
					      break;
				      case '<':
					      nDelimeter = '>';	// address, delimeter is '>'
					      break;
				      }
               }

				   break;
			   }
		   }
		   pszData++;
	   }

	   return (int)(pszData - pszDataStart);
   }

   AnsiString
   MimeParameterRFC2184Decoder::Decode(const AnsiString &input)
   {
      try
      {
         int endOfCharacterSet = input.Find("'");
         int endOfLanguage = input.Find("'", endOfCharacterSet+1);

         AnsiString characterSet = input.Mid(0, endOfCharacterSet);
         AnsiString language = input.Mid(endOfCharacterSet, endOfLanguage - endOfCharacterSet - 1);
         AnsiString encodedValue = input.Mid(endOfLanguage + 1);
         AnsiString resultValue;

         resultValue.append("=?");
         resultValue.append(characterSet);
         resultValue.append("?Q?");

         const int inputLength = encodedValue.GetLength();
         for (int i = 0; i < inputLength; i++)
         {
            char c = encodedValue[i];

            if (c == '%')
            {
               if (i > inputLength - 3)
                  break;

               char char1 = encodedValue[i+1];
               char char2 = encodedValue[i+2];

               resultValue.append("=");
               resultValue += char1;
               resultValue += char2;

               i+= 2;
            }
            else
               resultValue += c;
         }

         resultValue.append("?=");

         return resultValue;     
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5203, "MimeParameterRFC2184Decoder::Decode", 
            "Unable to decode MIME parameter according to RFC 2184.");

         return "";
      }
   };
}
