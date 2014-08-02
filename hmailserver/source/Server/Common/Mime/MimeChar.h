//////////////////////////////////////////////////////////////////////
//
// MIME Character Classification
//
// Jeff Lee
// Dec 16, 2000
//
//////////////////////////////////////////////////////////////////////

#pragma once

namespace HM
{
   class CMimeChar
   {
   public:
	   static bool IsNonAscii(unsigned char ch) { return (ch & 0x80) != 0; }
	   static bool IsControl(unsigned char ch) { return !(ch & 0x80) && (char_tbl_[ch] & CONTROL) != 0; }
	   static bool IsSpace(unsigned char ch) { return !(ch & 0x80) && (char_tbl_[ch] & SPACE) != 0; }
	   static bool IsPrintable(unsigned char ch) { return !(ch & 0x80) && (char_tbl_[ch] & PRINT) != 0; }
	   static bool IsSpecial(unsigned char ch) { return !(ch & 0x80) && (char_tbl_[ch] & SPECIAL) != 0; }
	   static bool IsHexDigit(unsigned char ch) { return !(ch & 0x80) && (char_tbl_[ch] & HEXDIGIT) != 0; }
	   static bool IsDelimiter(unsigned char ch) { return IsSpace(ch) || IsSpecial(ch); }
	   static bool IsToken(unsigned char ch) { return IsNonAscii(ch) || (ch > ' ' && !IsSpecial(ch)); }

   private:
	   enum { CONTROL=0x01, SPACE=0x02, PRINT=0x04, PUNCT=0x10, SPECIAL=0x20, HEXDIGIT=0x40 };
	   static const unsigned char char_tbl_[128];
   };

}