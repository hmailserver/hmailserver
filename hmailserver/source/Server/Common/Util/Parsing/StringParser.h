// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class StringParser  
   {
   public:
	   StringParser();
	   virtual ~StringParser();

	   static String ExtractDomain(const String &sEMailAddress);
      static String ExtractAddress(const String &sEMailAddress);

      static bool IsValidEmailAddress(const String &sEmailAddress);
      static bool IsValidDomainName(const String &sEmailAddress);
      static bool WildcardMatch(const String &pattern, const String &value);
      static bool WildcardMatchNoCase(const String &sWildcard, const String &sString);

      static char* Search(const char *haystack, size_t haystackSize, const char *needle);

      static String IntToString(int lTheInt);
      static String IntToString(unsigned int lTheInt);
      static String IntToString(__int64 lTheInt);

      static bool TryParseInt(const std::string &str, int &value);

      
      static std::vector<String> SplitString(const String &sInput, const String &sSeperators);
      static std::vector<AnsiString> SplitString(const AnsiString &sInput, const AnsiString &sSeperators);
      // Splits a string into a vector
   
      static String JoinVector(const std::vector<String> &sVector, const String &sSeperators);
      // Joins a vector into a long string

      static void Base64Encode(const String &sInput, String &sOutput);
      static void Base64Decode(const String &sInput, String &sOutput);
      
      static bool IsValidIPAddress(const String &sAddress);
      static String ToUSASCII(const String &sInput);
      static std::vector<String> GetAllButFirst(std::vector<String> sInput);

      static bool IsNumeric(const String &sInput);

      static bool ValidateString(const String &sString, const String &sAllowedChars);

      static void RemoveDuplicateItems(std::vector<String> &items);
   private:

      static bool AnyOfCharsExists_(const String &sChars, const String &sLookIn);
   
   };

   class StringParserTester
   {
   public :
      StringParserTester () {};
      ~StringParserTester () {};      
      
      void Test();
   };

}