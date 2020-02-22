// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Charset.h"

#include "../Mime/Mime.h"
#include "../Mime/CodePages.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Charset::Charset()
   {

   }

   Charset::~Charset()
   {

   }

   bool
   Charset::IsProperlyEncoded(const String &sInput)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Runs some checks to see if sInput is properly MIME-encoded. hMailServer normally
   // just transmits data in the email message directly to the client without any
   // decoding and encoding. However, often message ehaders are not properly formatted. 
   // In these cases, hMailServer needs to format them. Otherwise, certain email clients
   // such as Outlook Express will fail to display the messages.
   //---------------------------------------------------------------------------()
   {
      // Defined in  :
      // Format should be:
      //    encoded-word = "=?" charset "?" encoding "?" encoded-text "?="
      //    charset = token    
      //    encoding = token   
      //    token = 1*<Any CHAR except SPACE, CTLs, and especials>
      //    especials = "(" / ")" / "<" / ">" / "@" / "," / ";" / ":" / "
      //         <"> / "/" / "[" / "]" / "?" / "." / "="
      // 
      //    encoded-text = 1*<Any printable ASCII character other than "?"
      //               or SPACE>
      //            ; (but see "Use of encoded-words in message
      //            ; headers", section 5)
      //
      //    encoding should be B for base64 and Q for QP.
      
      int iSize = sInput.GetLength();
      const wchar_t *pCharBuf = sInput.data();

      bool bInsideEncodedString = false;

      for (int i = 0; i < iSize; i++)
      {
         const wchar_t s = *(pCharBuf + i);
         wchar_t sNextChar = 0;
         if (i < iSize -1) 
            sNextChar = *(pCharBuf + i +1);

         if (bInsideEncodedString)
         {
            if (s == '?' && sNextChar && sNextChar == '=')
            {
               // Encoding ends here.
               i++;
               bInsideEncodedString = false;
               continue;
            }
         }
         else
         {
            if (s == '=' && sNextChar && sNextChar == '?')
            {
               // Encoding starts here.
               i++;
               bInsideEncodedString = true;
               continue;
            }
         }

         if (bInsideEncodedString)
         {
            // encoded-text = 1*<Any printable ASCII character other than "?" or <SPACE>
               
            if (s < 33 || s > 126)
               return false;

            continue;
         }

         // Check if the character is US-ASCII printable
         if (s >= 32 && s <= 126)
            continue;

         // We found a character not matching our restrictions.
         return false;
      }


      return true;
      
   }


   String
   Charset::Encode(const String &sUnencoded)
   {
      if (IsProperlyEncoded(sUnencoded))
         return sUnencoded;

      return B64Encode(sUnencoded);
   }

   String
   Charset::GetIMAPEncoded(const String &sUnencoded)
   {
      // When sending a string to the client and the IMAP protocol
      // is in use, the string can't include the following characters:
      // " / 
      
      if (IsProperlyEncoded(sUnencoded) && 
          sUnencoded.Find(_T("\"")) == -1 && 
          sUnencoded.Find(_T("\\")) == -1) 
         return sUnencoded;

      return B64Encode(sUnencoded);

   }
   
   String 
   Charset::B64Encode(const String &sInput)
   {
      // Defined in rfc2047:
      // Convert to us-ascii format.
      // Format should be:
      // encoded-word = "=?" charset "?" encoding "?" encoded-text "?="
      // charset = token    
      // encoding = token   
      // token = 1*<Any CHAR except SPACE, CTLs, and especials>
      // especials = "(" / ")" / "<" / ">" / "@" / "," / ";" / ":" / "
      //         <"> / "/" / "[" / "]" / "?" / "." / "="
      // 
      // encoded-text = 1*<Any printable ASCII character other than "?"
      //               or SPACE>
      //            ; (but see "Use of encoded-words in message
      //            ; headers", section 5)
      //
      // encoding should be B for base64 and Q for QP.
      //
      // B64 encode the word.

      String sB64Encoded;
      StringParser::Base64Encode(sInput, sB64Encoded);

      // Format encording string according to RFC (above)
      String sEncodedWord;
      sEncodedWord.Format(_T("=?%s?%s?%s?="), _T("iso-8859-1"), _T("B"), sB64Encoded.c_str());

      return sEncodedWord; 
   }

   AnsiString 
   Charset::ToMultiByte(const String &sWideCharString, const AnsiString &sCharacterSet)
   {
      if (sCharacterSet.size() == 0)
         return sWideCharString;

      int iToCodePage = CodePages::Instance()->GetCodePage(sCharacterSet);

      int nNeedSize = WideCharToMultiByte( iToCodePage, 0, sWideCharString, sWideCharString.GetLength(), NULL, 0, NULL, NULL );
      
      char *pOutput = new char[nNeedSize + 1];
      memset(pOutput, 0, nNeedSize + 1);

      if( WideCharToMultiByte( iToCodePage, 0, sWideCharString, sWideCharString.GetLength(), pOutput, nNeedSize+1, NULL, NULL ) == 0 )
      {
         ASSERT(0);
      }
      
      AnsiString sRetVal = pOutput;
      delete [] pOutput;

      return sRetVal;
   }

   String 
   Charset::ToWideChar(const AnsiString &sMultiByte, const AnsiString &sCharacterSet)
   {
      if (sCharacterSet.IsEmpty())
         return sMultiByte;

      int iToCodePage = CodePages::Instance()->GetCodePage(sCharacterSet);

      int iNeedSize = MultiByteToWideChar(iToCodePage, 0, sMultiByte.c_str(), -1, NULL, NULL);

      wchar_t *pOutput = new wchar_t[iNeedSize + 1];
      int i = MultiByteToWideChar(iToCodePage, 0, sMultiByte.c_str(), -1, pOutput, iNeedSize + 1);
      String sWideStr = pOutput;
      delete [] pOutput;

      return sWideStr;
   }

   void CharsetTester::Test()
   {
      std::vector<String> setDoesNotNeedEncoding;
      setDoesNotNeedEncoding.push_back("A");
      setDoesNotNeedEncoding.push_back("=?ISO-8859-1?Q?Re:_Reuni=E3o?=");
      setDoesNotNeedEncoding.push_back("Re: =?ISO-8859-1?Q?Reuni=E3o?=");
      setDoesNotNeedEncoding.push_back("Re: =?ISO-8859-1?Q?Reuni=E3o?= =?ISO-8859-1?Q?Reuni=E3o?= ABCD =?ISO-8859-1?Q?Reuni=E3o?= ABCD");
      setDoesNotNeedEncoding.push_back("=?iso-8859-1?q?this=20is=20some=20text?=");
      setDoesNotNeedEncoding.push_back("=?US-ASCII?Q?Keith_Moore?= <moore@cs.utk.edu>");
      setDoesNotNeedEncoding.push_back("=?ISO-8859-1?Q?Keld_J=F8rn_Simonsen?= <keld@dkuug.dk>");
      setDoesNotNeedEncoding.push_back("=?ISO-8859-1?Q?Andr=E9?= Pirard <PIRARD@vm1.ulg.ac.be>");
      setDoesNotNeedEncoding.push_back("=?ISO-8859-1?B?SWYgeW91IGNhbiByZWFkIHRoaXMgeW8=?= =?ISO-8859-2?B?dSB1bmRlcnN0YW5kIHRoZSBleGFtcGxlLg==?=");
      setDoesNotNeedEncoding.push_back("=?ISO-8859-1?Q?Olle_J=E4rnefors?= <ojarnef@admin.kth.se>");
      setDoesNotNeedEncoding.push_back("=?ISO-8859-1?Q?Patrik_F=E4ltstr=F6m?= <paf@nada.kth.se>");
      setDoesNotNeedEncoding.push_back("Nathaniel Borenstein <nsb@thumper.bellcore.com> (=?iso-8859-8?b?7eXs+SDv4SDp7Oj08A==?=)");

      auto iter = setDoesNotNeedEncoding.begin();
      auto iterEnd = setDoesNotNeedEncoding.end();

      for (; iter != iterEnd; iter++)
      {
         String sText = (*iter);

         if (!Charset::IsProperlyEncoded(sText))
            throw 0;

      }

      std::vector<String> setNeedsEncoding;
      setNeedsEncoding.push_back("=?iso-8859-1?q?this is some text?=");
      setNeedsEncoding.push_back("åäö");
      setNeedsEncoding.push_back("ÅÄÖ");
      setDoesNotNeedEncoding.push_back("Re: =?ISO-8859-1?Q?Reuni=E3o?= =?ISO-8859-1?Q?Reuni=E3o?=ÖABCD =?ISO-8859-1?Q?Reuni=E3o?= ABCD");

      iter = setNeedsEncoding.begin();
      iterEnd = setNeedsEncoding.end();

      for (; iter != iterEnd; iter++)
      {
         String sText = (*iter);

         if (Charset::IsProperlyEncoded(sText))
            throw 0;
      }

   }
      
}
