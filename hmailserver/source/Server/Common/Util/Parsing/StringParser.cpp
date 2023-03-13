// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include <iosfwd>

#include "StringParser.h"

#include "../RegularExpression.h"
#include "../../MIME/MimeCode.h"
#include <boost/lexical_cast.hpp>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   StringParser::StringParser()
   {

   }

   StringParser::~StringParser()
   {

   }

   String
   StringParser::ExtractDomain(const String &sEMailAddress)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Extracts the domain from an email address string. (the part after @)
   //---------------------------------------------------------------------------()
   {
      int iAtSign = sEMailAddress.ReverseFind(_T("@"));
      String sDomain = sEMailAddress.Mid(iAtSign+1);
      return sDomain;
   }

   String
   StringParser::ExtractAddress(const String &sEMailAddress)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Extracts the adress from an email address string. (the part before @)
   //---------------------------------------------------------------------------()
   {
      int iAtSign = sEMailAddress.ReverseFind(_T("@"));
      String sDomain = sEMailAddress.Mid(0, iAtSign);
      return sDomain;
   }

   bool
   StringParser::IsValidEmailAddress(const String &sEmailAddress)
   {
      // Original: ^(("[^<>@\\]+")|([^<> @\\"]+))@(\[([0-9]{1,3}\.){3}[0-9]{1,3}\]|(?=.{1,255}$)((?!-|\.)[a-zA-Z0-9-]{0,62}[a-zA-Z0-9])(|\.(?!-|\.)[a-zA-Z0-9-]{0,62}[a-zA-Z0-9]){1,126})$
      // 
      // Conversion:
      // 1) Replace \ with \\
      // 2) Replace " with \"

      String regularExpression = "^((\"[^<>@\\\\]+\")|([^<> @\\\\\"]+))@(\\[([0-9]{1,3}\\.){3}[0-9]{1,3}\\]|(?=.{1,255}$)((?!-|\\.)[a-zA-Z0-9-]{0,62}[a-zA-Z0-9])(|\\.(?!-|\\.)[a-zA-Z0-9-]{0,62}[a-zA-Z0-9]){1,126})$";

      RegularExpression regexpEvaluator;
      bool result = regexpEvaluator.TestExactMatch(regularExpression, sEmailAddress);

      const int maxEmailAddressLength = 254;
      if (sEmailAddress.GetLength() > maxEmailAddressLength)
      {
         // https://stackoverflow.com/a/574698
         // An email address must not exceed 254 characters.
         return false;
      }

      return result;
   }

   bool
   StringParser::IsValidDomainName(const String &sDomainName)
   {
      // Original: ^(\[([0-9]{1,3}\.){3}[0-9]{1,3}\]|(?=.{1,255}$)((?!-|\.)[a-zA-Z0-9-]{0,62}[a-zA-Z0-9])(|\.(?!-|\.)[a-zA-Z0-9-]{0,62}[a-zA-Z0-9]){1,126})$
      // Conversion:
      // 1) Replace \ with \\
      // 2) Replace " with \"

      String regularExpression = "^(\\[([0-9]{1,3}\\.){3}[0-9]{1,3}\\]|(?=.{1,255}$)((?!-|\\.)[a-zA-Z0-9-]{0,62}[a-zA-Z0-9])(|\\.(?!-|\\.)[a-zA-Z0-9-]{0,62}[a-zA-Z0-9]){1,126})$";

      RegularExpression regexpEvaluator;
      bool result = regexpEvaluator.TestExactMatch(regularExpression, sDomainName);
      return result;
   }

   
   std::vector<String>
   StringParser::SplitString(const String &sInput, const String &sSeperators)
   {
      // Previously, this code used boost::tokenizer to split
      // the contents of string, but I did some tests and it
      // showed that the below code was 50% faster. Not so 
      // unexpected since tokenizer is much more advanced.

      std::vector<String> vecResult;
      int iBeginning = 0;
      int iEnd = sInput.Find(sSeperators);

      if (iEnd == -1)
      {
         // The separator was not found in the string. 
         // We should put the entire string in the result.
         if (!sInput.IsEmpty())
            vecResult.push_back(sInput);

      }

      int iSeperatorLen = sSeperators.GetLength();

      while (iEnd >= 0)
      {
         int iSubStrLength = iEnd - iBeginning;
         
         String sSubString;
         sSubString = sInput.Mid(iBeginning, iSubStrLength);
         
         vecResult.push_back(sSubString);

         // Skip to the position where the next substring
         // can start
         iBeginning = iEnd + iSeperatorLen;
         iEnd = sInput.Find(sSeperators, iBeginning);   
      }

      if (iBeginning > 0)
      {
         String sSubString = sInput.Mid(iBeginning);
         if (!sSubString.IsEmpty())
            vecResult.push_back(sSubString);
      }

      return vecResult;
    
  
   }

   std::vector<AnsiString>
   StringParser::SplitString(const AnsiString &sInput, const AnsiString &sSeperators)
   {
      std::vector<AnsiString> vecResult;
      int iBeginning = 0;
      int iEnd = sInput.Find(sSeperators);

      if (iEnd == -1)
      {
         // The separator was not found in the string. 
         // We should put the entire string in the result.
         if (!sInput.IsEmpty())
            vecResult.push_back(sInput);

      }

      int iSeperatorLen = sSeperators.GetLength();

      while (iEnd >= 0)
      {
         int iSubStrLength = iEnd - iBeginning;

         String sSubString;
         sSubString = sInput.Mid(iBeginning, iSubStrLength);

         vecResult.push_back(sSubString);

         // Skip to the position where the next substring
         // can start
         iBeginning = iEnd + iSeperatorLen;
         iEnd = sInput.Find(sSeperators, iBeginning);   
      }

      if (iBeginning > 0)
      {
         String sSubString = sInput.Mid(iBeginning);
         if (!sSubString.IsEmpty())
            vecResult.push_back(sSubString);
      }

      return vecResult;


   }

   String
   StringParser::JoinVector(const std::vector<String> &sVector, const String &sSeperator)
   {
      std::vector<String>::const_iterator iterVec = sVector.begin();
      std::vector<String>::const_iterator iterEnd = sVector.end();

      String result;

      for (; iterVec != iterEnd; iterVec++)
      {
         result += (*iterVec);

         if (iterVec + 1 != iterEnd)
            result += sSeperator;
      }     
   
      return result;


   }


   std::vector<String>
   StringParser::GetAllButFirst(std::vector<String> sInput)
   {
      std::vector<String> vecResult;
      auto iterCur = sInput.begin();

      if (iterCur == sInput.end())
         return vecResult;

      iterCur++;

      while (iterCur != sInput.end())
      {
         
         vecResult.push_back(*iterCur);   

         iterCur++;
      }

      return vecResult;

   }

   bool
   StringParser::ValidateString(const String &sString, const String &sAllowedChars)
   {
      for (int i = 0; i < sString.GetLength(); i++)
      {
         if (sAllowedChars.Find(sString.GetAt(i)) < 0)
            return false;
      }

      return true;   
   }

   bool
   StringParser::AnyOfCharsExists_(const String &sChars, const String &sLookIn)
   {

      for (int i = 0; i < sChars.GetLength(); i++)
      {
         if (sLookIn.Find(sChars.GetAt(i)) >= 0)
            return true;
      }
      return false;
   }

   int _wildcmp(const wchar_t *wild, const wchar_t *string, int recursions)
   {
      const int maxRecursions = 300;

      recursions++;

      if (recursions > maxRecursions)
         return 0;

      if(*wild == *string)
         return '\0' == *string || _wildcmp(++wild, ++string, recursions);

      if('\0' == *string)
         return '*' == *wild && _wildcmp(++wild, string, recursions);

      switch(*wild)
      {
      case '?':
         return _wildcmp(++wild, ++string, recursions);

      case '*':
         wild++;

         if('\0' == *wild)
            return 1;

         while(*string != '\0')
            if(_wildcmp(wild, string++, recursions))
               return 1;

      default:
         return 0;
      }
   }

   bool
   StringParser::WildcardMatchNoCase(const String &sWildcard, const String &sString)
   {
      String sLowerWild = sWildcard;
      String sLowerStr = sString;

      sLowerWild.ToLower();
      sLowerStr.ToLower();

      return WildcardMatch(sLowerWild, sLowerStr);
   }


   bool
   StringParser::WildcardMatch(const String &pattern, const String &value)
   {
      // Convert the pattern to a regular expression.
      String regularExpression;

      for (int i = 0; i < pattern.GetLength(); i++)
      {
         wchar_t c = pattern[i];

         switch (c)
         {
            case '\\':
            case '|':
            case '.':
            case '^':
            case '$':
            case '+':
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
               regularExpression.append(_T("\\"));
               regularExpression += c;
               break;
            case '*':
               regularExpression.append(_T(".*"));
               break;
            case '?':
               regularExpression.append(_T("."));
               break;
            default:
               regularExpression += c;
               break;

         }
      }

      RegularExpression regexpEvaluator;
      bool result = regexpEvaluator.TestExactMatch(regularExpression, value);
      return result;
   }

   String 
   StringParser::IntToString(int lTheInt)
   {
      return boost::lexical_cast<std::string>(lTheInt);
   }

   String 
   StringParser::IntToString(unsigned int lTheInt)
   {
      return boost::lexical_cast<std::string>(lTheInt);
   }

   String 
   StringParser::IntToString(__int64 lTheInt)
   {
      String sRetVal;
      sRetVal.Format(_T("%I64d"), lTheInt);
      return sRetVal;
   }

   void
   StringParser::Base64Decode(const String &sInput, String &sOutput)
   {

      if (sInput.GetLength() == 0)
      {
         sOutput.Empty();
         return;
      }

      AnsiString sInputStr = sInput;

      MimeCodeBase64 DeCoder;
      DeCoder.AddLineBreak(false);
      DeCoder.SetInput(sInputStr, sInputStr.GetLength(), false);
      
      AnsiString output;
      DeCoder.GetOutput(output);

      int length = output.GetLength();
      // Since we're going to store the result in
      // a normal StdString, we can't return null
      // characters.
      for (int i = 0; i < length; i++)
      {
         if (output[i] == 0)
            output[i] = '\t';
      }

      sOutput = output;
   }

   void
   StringParser::Base64Encode(const String &sInput, String &sOutput)
   {
      if (sInput.GetLength() == 0)
      {
         sOutput.Empty();
         return;
      }
         
      AnsiString sInputStr = sInput;
      
      MimeCodeBase64 Coder;
      Coder.SetInput(sInputStr, sInputStr.GetLength(), true);
      Coder.AddLineBreak(false);
      
      AnsiString output;
      Coder.GetOutput(output);

      sOutput = output;
   }

   bool
   StringParser::IsNumeric(const String &sInput)
   {

      String sNumbers = "1234567890";
      int l = sInput.GetLength();
      for (int i = 0; i < l; i++)
      {
         if (sNumbers.Find(sInput.GetAt(i)) < 0)
            return false;
      }

      return true;
   }


   bool 
   StringParser::IsValidIPAddress(const String &sAddress)
   {
      IPAddress address;
      return address.TryParse(sAddress, false);
   }

   bool 
   StringParser::TryParseInt(const std::string &str, int &value)
   {
      try
      {
         value = boost::lexical_cast<int>(str);
         return true;
      }
      catch (boost::bad_lexical_cast&)
      {
         return false;
      }
      
   }

   char*
   StringParser::Search(const char *haystack, size_t haystackSize, const char *needle)
   {
      if (haystack == 0 || needle == 0)
         return 0;

      size_t needleSize = strlen(needle);

      for (size_t haystackIndex = 0; haystackIndex < haystackSize; haystackIndex++)
      {
         size_t remainingHaystackSize = haystackSize - haystackIndex;

         // If the string we're searching for is longer than the string
         // we're searching in, there's no point in performing the search.
         if (needleSize > remainingHaystackSize)
            return 0;
         
         const char *currentHaystackPosition = haystack + haystackIndex;

         if (memcmp(currentHaystackPosition, needle, needleSize) == 0)
            return (char*) currentHaystackPosition;
      }

      return 0;
   }

   // Removes all duplicate items from the items collection.
   void 
   StringParser::RemoveDuplicateItems(std::vector<String> &items)
   {

      // Remove duplicate names.
      auto iter = items.begin();
      std::set<String> duplicateCheck;

      while (iter != items.end())
      {
         String name = (*iter);
         if (duplicateCheck.find(name) != duplicateCheck.end())
         {
            // We found a duplicate. Remove it.
            iter = items.erase(iter);
         }
         else
         {
            // This is not a duplicate. Move to next.
            iter++;

            duplicateCheck.insert(name);
         }
      }
   }

   void Assert(bool result)
   {
      if (result == false)
         throw;
   }

   void StringParserTester::Test()
   {  
      String s = "AAAA";
      if (s.CompareNoCase(_T("aaaa")) != 0) throw;

      s = "AAAA";
      if (s.CompareNoCase(_T("bbbb")) == 0) throw;

      s = "AAAA";
      if (s.Equals(_T("aaaa"), true)) throw;

      s = "AAAA";
      if (!s.Equals(_T("aaaa"), false)) throw;

      s = "ZZZZZ";
      if (!s.Equals(_T("ZZZZZ"), false)) throw;

      s = "";
      if (!s.Equals(_T(""), false)) throw;

      // Test a few String functions
      String sTest = "TEST";
      sTest.Replace(_T("TEST"), _T("test"));
      if (sTest != _T("test")) throw;

      sTest = "test";
      sTest.Replace(_T("TEST"), _T("dummy"));
      if (sTest != _T("test")) throw;

      sTest = "test";
      sTest.ReplaceNoCase(_T("TEST"), _T("TEST"));
      if (sTest != _T("TEST")) throw;

      sTest = "TeSt";
      sTest.ReplaceNoCase(_T("TEST"), _T("TEST"));
      if (sTest != _T("TEST")) throw;

      sTest = "TestDummy";
      sTest.ReplaceNoCase(_T("testdummy"), _T("TestDummyReplaced"));
      if (sTest != _T("TestDummyReplaced")) throw;

      sTest = "TestDummy";
      sTest.ReplaceNoCase(_T("test"), _T("TestA"));
      if (sTest != _T("TestADummy")) throw;

      sTest = "Test Test Test Test";
      sTest.ReplaceNoCase(_T("Test"), _T("TEST"));
      if (sTest != _T("TEST TEST TEST TEST")) throw;
      
      sTest = "Test TestA Test Test";
      sTest.ReplaceNoCase(_T("TestA"), _T("TESTB"));
      if (sTest != _T("Test TESTB Test Test")) throw;
 
      // Check email addresses

      
      if (StringParser::IsValidEmailAddress("@")) throw;
      if (StringParser::IsValidEmailAddress("a")) throw;      
      if (StringParser::IsValidEmailAddress("test@")) throw;
      if (StringParser::IsValidEmailAddress("@.com")) throw;
      if (StringParser::IsValidEmailAddress("\"va@ff\"@test.co.uk")) throw;
      if (StringParser::IsValidEmailAddress("some one@test.co.uk")) throw;
      if (StringParser::IsValidEmailAddress("<someone@test.co.uk>")) throw;
      if (StringParser::IsValidEmailAddress("va ff@test.co.uk")) throw;
      if (!StringParser::IsValidEmailAddress("test@test.com")) throw;
      if (!StringParser::IsValidEmailAddress("test@hmailserver.com")) throw;
      if (!StringParser::IsValidEmailAddress("test_test@hmailserver.com")) throw;
      if (!StringParser::IsValidEmailAddress("bill@microsoft.com")) throw;
      if (!StringParser::IsValidEmailAddress("martin@hmailserver.com")) throw;
      if (!StringParser::IsValidEmailAddress("vaff@test.co.uk")) throw;
      if (!StringParser::IsValidEmailAddress("va'ff@test.co.uk")) throw;
      if (!StringParser::IsValidEmailAddress("\"va ff\"@test.co.uk")) throw;
      

      if (StringParser::ExtractAddress("\"va@ff\"@test.co.uk").Compare(_T("\"va@ff\"")) != 0) throw;
      if (StringParser::ExtractAddress("test@test.co.uk").Compare(_T("test")) != 0) throw;
      if (StringParser::ExtractAddress("t'est@test.co.uk").Compare(_T("t'est")) != 0) throw;
      if (StringParser::ExtractAddress("\"t@es@\"@test.co.uk").Compare(_T("\"t@es@\"")) != 0) throw;
      if (StringParser::ExtractAddress("test@test").Compare(_T("test")) != 0) throw;
      if (StringParser::ExtractAddress("t\"est@test.com").Compare(_T("t\"est")) != 0) throw;

      if (StringParser::ExtractDomain("t\"est@test.com").Compare(_T("test.com")) != 0) throw;
      if (StringParser::ExtractDomain("t'est@test.co.uk").Compare(_T("test.co.uk")) != 0) throw;
      if (StringParser::ExtractDomain("\"t@est\"@test.co.uk").Compare(_T("test.co.uk")) != 0) throw;
      if (StringParser::ExtractDomain("\"t@es@\"@test.co.uk").Compare(_T("test.co.uk")) != 0) throw;
      if (StringParser::ExtractDomain("test@test.com").Compare(_T("test.com")) != 0) throw;

      if (!StringParser::WildcardMatch("Test", "Test")) throw;
      if (!StringParser::WildcardMatch("", "")) throw;
      if (!StringParser::WildcardMatch("Test*", "Testar")) throw;
      if (!StringParser::WildcardMatch("Test*", "Test")) throw;
      if (StringParser::WildcardMatch("Test*", "Te")) throw;

	  if (!StringParser::WildcardMatch("*two*", "one-two-three")) throw;
	  if (StringParser::WildcardMatch("*two*", "one-three")) throw;
	  if (StringParser::WildcardMatch("?two?", "one-two-three")) throw;
	  if (!StringParser::WildcardMatch("?two?", "-two-")) throw;

     // Short strings.
     if (!StringParser::WildcardMatch("?", "A")) throw;
     if (StringParser::WildcardMatch("?", "AA")) throw;
     if (!StringParser::WildcardMatch("*", "A")) throw;
     if (!StringParser::WildcardMatch("*", "AA")) throw;
     if (!StringParser::WildcardMatch("*", "")) throw;
     if (StringParser::WildcardMatch("?", "")) throw;

     // Unicode strings
     if (!StringParser::WildcardMatch(_T("??語"), _T("標準語"))) throw;
     if (StringParser::WildcardMatch(_T("?語"), _T("標準語"))) throw;
     if (!StringParser::WildcardMatch(_T("?準?"), _T("標準語"))) throw;
     if (StringParser::WildcardMatch(_T("?準"), _T("標準語"))) throw;
     if (!StringParser::WildcardMatch(_T("標*"), _T("標準語"))) throw;

     // Matching email addresses
     if (!StringParser::WildcardMatch("test@*", "test@test.com")) throw;
     if (!StringParser::WildcardMatch("test@test.co.*", "test@test.co.uk")) throw;
     if (StringParser::WildcardMatch("test@test.co.*", "test@test.com")) throw;
     if (StringParser::WildcardMatch("test@test.co.*", "test@test.co")) throw;

     // Long strings.
     String k10String;
     for (int i = 0; i < 1000; i++)
        k10String  += "AAAAAAAAAA";

     String s310CharString;
     for (int i = 0; i < 31; i++)
        s310CharString  += "AAAAAAAAAA";

     if (!StringParser::WildcardMatch(_T("*"), k10String)) throw;
     if (!StringParser::WildcardMatch(s310CharString, s310CharString)) throw;

     char *p = 0;
     p = StringParser::Search("test", 4, "e");
     if (*p != 'e') throw;
     p = StringParser::Search("test", 4, "es");
     if (*p != 'e') throw;     
     p = StringParser::Search("test", 4, "n");
     if (p != 0) throw;      
     p = StringParser::Search("test", 4, "t");
     if (*p != 't') throw; 
     p = StringParser::Search("test ", 5, " ");
     if (*p != ' ') throw;  
     p = StringParser::Search("lest ", 5, "l");
     if (*p != 'l') throw; 
     p = StringParser::Search("testp", 4, "p");
     if (p != 0) throw;  
     p = StringParser::Search("testp", 5, "");
     if (*p != 't') throw;  
     p = StringParser::Search("", 0, "test");
     if (p != 0) throw;  
     p = StringParser::Search("", 0, "");
     if (p != 0) throw;  
     p = StringParser::Search("test", 4, "p");
     if (p != 0) throw;  
     p = StringParser::Search("test", 4, "feb");
     if (p != 0) throw;

      // RESULT:
      /*
         Strings containing 80% us-ascii characters:
         QP is about 50% faster.

         Strings containing 100% non-usascii
         B64 is about 10% faster.
      */

     String input = _T("A B C");
     std::vector<String> result = StringParser::SplitString(input, _T(" "));
     if (result.size() != 3)
        throw;

     input = "A B";
     result = StringParser::SplitString(input, " ");
     if (result.size() != 2)
        throw;

     // Test Contains and ContainsNoCase
     String s1 = "Test";
     String s2 = "Test";
     Assert(s1.Contains(s2));

     s1 = "Test";
     s2 = "Tes";
     Assert(s1.Contains(s2));

     s1 = "Test";
     s2 = "est";
     Assert(s1.Contains(s2));

     s1 = "Test";
     s2 = "est";
     Assert(s1.Contains(s2));

     s1 = "Te";
     s2 = "Tes";
     Assert(!s1.Contains(s2));

     s1 = "Test";
     s2 = "TEST";
     Assert(!s1.Contains(s2));

     s1 = "Test";
     s2 = "TEST";
     Assert(s1.ContainsNoCase(s2));

     s1 = "ABCDEFGHIJKLMNOPQ";
     s2 = "hijkl";
     Assert(s1.ContainsNoCase(s2));

     s1 = "ABCDEFGHIJKLMNOPQ";
     s2 = "hijkl";
     Assert(!s1.Contains(s2));

   }
 
   


}