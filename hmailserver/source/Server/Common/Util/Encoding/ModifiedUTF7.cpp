// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "ModifiedUTF7.h"
#include "../Charset.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   
   /*
   Converts an UTF-8 string to modified UTF-7.

   Example: 
      Input: TE&AOUA5AD2-ST
      Output: TEåäöST

   Implementation:
      We go through the input string one character at a time.
      If the character is not a special character, we'll add it
      directly to the output string. If the character IS a special 
      character, it needs to be encoded. When we detect a special
      character, we continue going through the string until we find
      a normal char or end of string. We then encode that subset of 
      the string using the Windows API.
   */
   AnsiString
   ModifiedUTF7::Encode(const String &sUnicodeString)
   {
      AnsiString sRetVal;
      int iCurrentPos = 0;

      while (iCurrentPos <  sUnicodeString.GetLength())
      {
         wchar_t sCurrentChar = sUnicodeString[iCurrentPos];

         if (IsSpecialCharacter_(sCurrentChar) == false)
         {
            // This isn't a special character, so we can append it without
            // doing any encoding.
            sRetVal += (char) sCurrentChar;

            if (sCurrentChar == '&')
               sRetVal += "-";

            iCurrentPos++;
            sCurrentChar = sUnicodeString[iCurrentPos];

            continue;
         }

         // Locate the position of the next special character.
         int subsetStartPos = iCurrentPos;

         for (; iCurrentPos < sUnicodeString.GetLength(); iCurrentPos++)
         {
            sCurrentChar = sUnicodeString[iCurrentPos];
            if (!IsSpecialCharacter_(sCurrentChar))
               break;
         } 
         
         int subsetLength = iCurrentPos - subsetStartPos;
         String subSet = sUnicodeString.Mid(subsetStartPos, subsetLength);
         if (!subSet.IsEmpty())
         {
            AnsiString utf7String = Charset::ToMultiByte(subSet, _T("utf-7"));
            utf7String = utf7String.TrimLeft("+");
            utf7String = utf7String.TrimRight("-");

            sRetVal += "&"; 
            sRetVal += utf7String;
            sRetVal += "-";
         }
        
      }

      return sRetVal;
   }

   /*
   Converts a modified UTF-7 string to UTF-8.

   Example: 
      Input: TE&AOUA5AD2-ST
      Output: TEåäöST

   Implementation:
      We go through the input string one character at a time.
      We add the input character to the output buffer. If we find
      a &, that means we're entering an encoded subset. We locate
      the end of this subset (-) and the decode the subset.
   */

   String 
   ModifiedUTF7::Decode(const AnsiString &s)
   {
      String sRetVal;

      for (int iPos=0 ; iPos < s.GetLength(); iPos++)
      {
         char sCurrentChar = s[iPos];

         if ( IsSpecialCharacter_(sCurrentChar) )
         {
            // We should not have a special character here.
            return "";
         }

         if ( sCurrentChar != '&' )
         {
            sRetVal += sCurrentChar;
            continue;
         }

         // Move to next character
         iPos++;

         sCurrentChar = s[iPos];

         if ( sCurrentChar == '-' )
         {
            sRetVal += '&';
            continue;
         }

         // Locate the position of the next special character.
         int subsetStartPos = iPos;

         for (; iPos < s.GetLength(); iPos++)
         {
            sCurrentChar = s[iPos];
            if (sCurrentChar == '-')
               break;
         } 

         int subsetLength = iPos - subsetStartPos;
         String subSet = s.Mid(subsetStartPos, subsetLength);
         if (!subSet.IsEmpty())
         {
            String unicodeString = Charset::ToWideChar("+" + subSet + "-", _T("utf-7"));
            sRetVal += unicodeString;
         }
      }

      return sRetVal;
   }


   bool
   ModifiedUTF7::IsSpecialCharacter_(wchar_t c)
   {
      return c < 32 || c >= 127;
   }


   void
   ModifiedUTF7Tester::Test()
   {
      String s = ModifiedUTF7::Encode(_T("test"));
      if (s.Compare(_T("test")) != 0)
         throw;

      s = ModifiedUTF7::Encode(_T("&"));
      if (s.Compare(_T("&-")) != 0)
         throw;

      s = ModifiedUTF7::Encode(_T("TEST"));
      if (s.Compare(_T("TEST")) != 0)
         throw;

      s = ModifiedUTF7::Encode(_T(""));
      if (s.Compare(_T("")) != 0)
         throw;

      s = ModifiedUTF7::Encode(_T("TE\\ST"));
      if (s.Compare(_T("TE\\ST")) != 0)
         throw;

      s = ModifiedUTF7::Encode(_T("TEåäöST"));
      if (s.Compare(_T("TE&AOUA5AD2-ST")) != 0)
         throw;

      s = ModifiedUTF7::Encode(_T("T"));
      if (s.Compare(_T("T")) != 0)
         throw;

      s = ModifiedUTF7::Encode(_T("Å"));
      if (s.Compare(_T("&AMU-")) != 0)
         throw;

      s = ModifiedUTF7::Encode(_T("Ä"));
      if (s.Compare(_T("&AMQ-")) != 0)
         throw;

      s = ModifiedUTF7::Encode(_T("ÄÄÄÄÄÄÄÄÄÄÄÄÄ"));
      if (s.Compare(_T("&AMQAxADEAMQAxADEAMQAxADEAMQAxADEAMQ-")) != 0)
         throw;

      s = ModifiedUTF7::Encode(_T("ABCDEFGHIJKLMNOPQRSTUVXYZÅÄÖ,.<>-1234567890!¤%&/()=?"));
      if (s.Compare(_T("ABCDEFGHIJKLMNOPQRSTUVXYZ&AMUAxADW-,.<>-1234567890!&AKQ-%&-/()=?")) != 0)
         throw;

      s = ModifiedUTF7::Decode(ModifiedUTF7::Encode(_T("TEST")));
      if (s.Compare(_T("TEST")))
         throw;

      s = ModifiedUTF7::Decode(ModifiedUTF7::Encode(_T("ÅÄÖ")));
      if (s.Compare(_T("ÅÄÖ")))
         throw;

      s = ModifiedUTF7::Decode(ModifiedUTF7::Encode(_T("1234567890ABCDEFGHIJKLMNOPQRSTUVXYZÅÄÖ!#¤&%&()=?,<>")));
      if (s.Compare(_T("1234567890ABCDEFGHIJKLMNOPQRSTUVXYZÅÄÖ!#¤&%&()=?,<>")))
         throw;

      s = ModifiedUTF7::Decode("&AMQAxADEAMQAxADEAMQAxADEAMQAxADEAMQ-");
      if (s.Compare(_T("ÄÄÄÄÄÄÄÄÄÄÄÄÄ")) != 0)
         throw;

      s = ModifiedUTF7::Decode("&AMU-");
      if (s.Compare(_T("Å")) != 0)
         throw;

      s = ModifiedUTF7::Decode("&AMQ-");
      if (s.Compare(_T("Ä")) != 0)
         throw;
 
      s = ModifiedUTF7::Decode(ModifiedUTF7::Encode(_T("Ελληνικά")));
      if (s.Compare(_T("Ελληνικά")))
         throw;

      s = ModifiedUTF7::Decode(ModifiedUTF7::Encode(_T("&&&&----&")));
      if (s.Compare(_T("&&&&----&")))
         throw;
   }


}