// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "AddresslistParser.h"
#include "../Charset.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   AddresslistParser::AddresslistParser()
   {

   }

   AddresslistParser::~AddresslistParser()
   {

   }

   bool
   AddresslistParser::IsSeperator(char c) const
   {
      if (c == ' ' || 
          c == ',' ||
          c == '\r' ||
          c == '\n' ||
          c == '\t')
         return true;
      else
         return false;
   }
   
   std::vector<std::shared_ptr<Address> > 
   AddresslistParser::ParseList(const String &sList) const
   {
      std::vector<std::shared_ptr<Address> > vecResult;

      std::vector<String> vecCompounds;
      
      int i = 0;
      int iCurWordStartPos = 0;
      bool bIsInsideQuote = false;

      int iLength = sList.GetLength();
      const wchar_t *pCharBuf = sList.data();

      int maxIterations = 1000000;

      while (i < iLength && maxIterations > 0)
      {
         maxIterations--;

         const wchar_t sCurChar = *(pCharBuf + i);

         if (sCurChar == '\\')
            i++; // Jump an extra since next character is escaped.
         else if (sCurChar == '\"')
         {
            if (bIsInsideQuote)
               bIsInsideQuote = false;
            else
               bIsInsideQuote = true;
         }
         else if (!bIsInsideQuote && sCurChar == ',')
         {
            // We have found an unquoted comma character.
            int iCurWordLen = i - iCurWordStartPos;

            String sCurWord = sList.Mid(iCurWordStartPos, iCurWordLen);
            sCurWord.Trim();
            vecCompounds.push_back(sCurWord);

            iCurWordStartPos = i + 1;
         }
         
         i++;
      }

      if (iCurWordStartPos < sList.GetLength())
      {
         String sCurWord = sList.Mid(iCurWordStartPos);
         sCurWord.Trim();
         vecCompounds.push_back(sCurWord);
      }

      // We now have a vector containing the recipients.

      for(String compound : vecCompounds)
      {
         String sFullName;
         String sMailbox;
         String sDomain;

         ExtractParts(compound, sFullName, sMailbox, sDomain);

         std::shared_ptr<Address> pAddress = std::shared_ptr<Address> (new Address);
         
         pAddress->sPersonalName = Charset::Encode(sFullName);
         
         // Remove <> from <test@test.com>
         sMailbox.Replace(_T("<"), _T(""));      
         sDomain.Replace(_T(">"), _T(""));
   
         pAddress->sMailboxName = Charset::Encode(sMailbox);
         pAddress->sDomainName = Charset::Encode(sDomain);

         vecResult.push_back(pAddress);
      }

     return vecResult;

   }
   
   void 
   AddresslistParser::ExtractParts(const String &sCompound, String &sFullName, String &sMailbox, String &sDomain) const
   {
      // Extract the name and address from a string like:
      // first last <email@domain.com>. This method is extremly
      // fuzzy and won't work unless the address is really properly
      // formatted.
      
      String sEmailAddress;

      ExtractQuotedString_(sCompound, sFullName);
      ExtractWithinGTLT_(sCompound, sEmailAddress);
      
      UnescapeString(sFullName);
      UnescapeString(sEmailAddress);

      if (sFullName.IsEmpty())
      {
         // We could not find a full name in this email address. It
         // could be that the full name is not enclosed within quotes.
         // Instead, let's assume that the string before the first <
         // is the full user name.

         int iGTPos = sCompound.Find(_T("<"));

         if (iGTPos > 0)
         {
            sFullName = sCompound.Mid(0, iGTPos);

            // If there's a space between the fullname and
            // the email address, remove it.
            sFullName.TrimRight();
         }

      }

      // If no full name has been specified, use the email address
      // as full name, so that we have something to show in the
      // recipients email client
      if (sFullName.IsEmpty())
         sFullName = sEmailAddress;

      if (sFullName.IsEmpty() && sEmailAddress.IsEmpty())
      {
         // We haven't been able to parse out any useful information
         // from this string. Let's assume that the string contains
         // just the full name of the holder of the mailbox.
         sFullName = sCompound;
      }

      if (sEmailAddress.IsEmpty())
      {
         // We haven't been able to parse out the email address. Check
         // if the full name is a valid email address and if so assume
         // it's the email address.
         if (StringParser::IsValidEmailAddress(sFullName))
         {
            sEmailAddress = sFullName;
         }
      }

      // Find the domain part and the mailbox part.
      sMailbox = StringParser::ExtractAddress(sEmailAddress);
      sDomain = StringParser::ExtractDomain(sEmailAddress);

   }

   void 
   AddresslistParser::ExtractQuotedString_(const String &sData, String &sQuotedString) const
   {
      sQuotedString = "";
      bool bInsideQuote = false;

      int iQuoteStart = -1;
      int iQuoteEnd = -1;

      int i = 0;
      // Found a space outside a quoted string.
      while (i < sData.GetLength())
      {
         wchar_t sCurChar = sData.GetAt(i);

         if (sCurChar == '\\')
            i++; // Escaped string. Skip one extra
         else if (sCurChar == '\"')
         {
            if (bInsideQuote)
            {
               iQuoteEnd = i;
               bInsideQuote = false;
               break;
            }
            else
            {
               iQuoteStart = i;
               bInsideQuote = true;
            }
         }

         i++;
      }

      if (iQuoteStart > -1 && iQuoteEnd > -1)
      {
         // Skip passed the "
         iQuoteStart ++; 

         int iQuoteLength = iQuoteEnd - iQuoteStart;
         sQuotedString = sData.Mid(iQuoteStart, iQuoteLength);

      }
   }

   void 
   AddresslistParser::ExtractWithinGTLT_(const String &sData, String &sOutputString) const
   {
      sOutputString = "";

      bool bInsideQuote = false;
      bool bInsideGTLT = false;

      int iGTPos = -1;
      int iLTPos = -1;

      int i = 0;
      // Found a space outside a quoted string.
      while (i < sData.GetLength())
      {
         wchar_t sCurChar = sData.GetAt(i);

         if (sCurChar == '\\')
            i++; // Escaped string. Skip one extra
         else if (sCurChar == '\"')
         {
            if (!bInsideGTLT)
            {
               if (bInsideQuote)
                  bInsideQuote = false;
               else
                  bInsideQuote = true;
            }
         }
         else if (!bInsideQuote)
         {
            if (sCurChar == '<')
            {
               iGTPos = i;
            }
            else if (sCurChar == '>')
            {
               iLTPos = i;
               break;
            }
         }

         i++;
      }

      if (iGTPos > -1 && iLTPos > -1)
      {
         // Skip passed the <
         int iStartPos = iGTPos + 1; 

         int iLength = iLTPos - iGTPos - 1;
         sOutputString = sData.Mid(iStartPos, iLength );

      }
   }


   void 
   AddresslistParser::UnescapeString(String &TheString) const
   {
      TheString.Replace(_T("\\\""), _T("\""));
      TheString.Replace(_T("\\\\"), _T("\\"));
   }

   AddresslistParserTester::AddresslistParserTester()
   {

   }

   bool
   AddresslistParserTester::Test()
   {
      TestAddress_("@", "@", "","");
      TestAddress_("test@example.com", "test@example.com", "test","example.com");
      TestAddress_("<martin@example.com>", "martin@example.com", "martin","example.com");
      TestAddress_("\"servizio@example.it\"<servizio@example.it>", "servizio@example.it", "servizio", "example.it");
      TestAddress_("\"Martin Knafve\" <martin@test.com>", "Martin Knafve", "martin", "test.com");
      TestAddress_("Martin Knafve <test@domain.com>", "Martin Knafve", "test", "domain.com");      
      TestAddress_("\"fdsa\\\"\" <test@test.com>", Charset::Encode("fdsa\""), "test", "test.com");      
      TestAddress_("Emanuele Example <emanuele@example.com>", "Emanuele Example", "emanuele", "example.com");      
      TestAddress_("Secunia Research <remove-vuln@example.com>", "Secunia Research", "remove-vuln", "example.com");      
      TestAddress_("bugtraq@securityfocus.com", "bugtraq@securityfocus.com", "bugtraq", "securityfocus.com");
      TestAddress_("Daniel <Daniel_SE@example.com>", "Daniel", "Daniel_SE", "example.com");
      TestAddress_("\"Fredlund Example\" <Ingrid.Example@example.com>", "Fredlund Example", "Ingrid.Example", "example.com");


      {  // MORE ADVANCED
         
         String sTestStr = "\"Martin Knafve\" <martin@test.com>,   \"Test2 Testar\" <testa@testy.com>";

         AddresslistParser* pParser= new AddresslistParser();

         std::vector<std::shared_ptr<Address> > vecResult = pParser->ParseList(sTestStr);

         if (vecResult.size() != 2)
         {
            assert(0);
            throw;
         }
         
         // Go to the second and check that it's right...
         auto iterFirst = vecResult.begin();
         iterFirst++;

         if ((*iterFirst)->sPersonalName != _T("Test2 Testar"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sMailboxName != _T("testa"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sDomainName != _T("testy.com"))
         {
            assert(0);
            throw;
         }


         delete pParser;
      }

      {  // EVEN MORE ADVANCED. Two addresses seperated by tab, newline and space.
         
         String sTestStr = "\"Martin Knafve\" <martin@test.com>, \r\n  \"Test2 Testar\" <testa@testy.com>";

         AddresslistParser* pParser= new AddresslistParser();

         std::vector<std::shared_ptr<Address> > vecResult = pParser->ParseList(sTestStr);

         if (vecResult.size() != 2)
         {
            assert(0);
            throw;
         }
         
         // Go to the second and check that it's right...
         auto iterFirst = vecResult.begin();
         iterFirst++;

         if ((*iterFirst)->sPersonalName != _T("Test2 Testar"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sMailboxName != _T("testa"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sDomainName != _T("testy.com"))
         {
            assert(0);
            throw;
         }


         delete pParser;
      }

      {  // EVEN MORE ADVANCED. Two addresses without <> around the addresses.
         
         String sTestStr = "\"Martin Knafve\" <martin@test.com>, \"Test2 Testar\" <testa@testy.com>";

         AddresslistParser* pParser= new AddresslistParser();

         std::vector<std::shared_ptr<Address> > vecResult = pParser->ParseList(sTestStr);

         if (vecResult.size() != 2)
         {
            assert(0);
            throw;
         }
         
         // Go to the second and check that it's right...
         auto iterFirst = vecResult.begin();

         if ((*iterFirst)->sPersonalName != _T("Martin Knafve"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sMailboxName != _T("martin"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sDomainName != _T("test.com"))
         {
            assert(0);
            throw;
         }

         iterFirst++;

         if ((*iterFirst)->sPersonalName != _T("Test2 Testar"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sMailboxName != _T("testa"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sDomainName != _T("testy.com"))
         {
            assert(0);
            throw;
         }


         delete pParser;
      }

      {  // EVEN EVEN MORE ADVANCED. Two addresses without "" around the names.
         
         String sTestStr = "MartinKnafve <martin@test.com>, EyyAo <testa@testy.com>";

         AddresslistParser* pParser= new AddresslistParser();

         std::vector<std::shared_ptr<Address> > vecResult = pParser->ParseList(sTestStr);

         if (vecResult.size() != 2)
         {
            assert(0);
            throw;
         }
         
         // Go to the second and check that it's right...
         auto iterFirst = vecResult.begin();

         if ((*iterFirst)->sPersonalName != _T("MartinKnafve"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sMailboxName != _T("martin"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sDomainName != _T("test.com"))
         {
            assert(0);
            throw;
         }

         iterFirst++;

         if ((*iterFirst)->sPersonalName != _T("EyyAo"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sMailboxName != _T("testa"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sDomainName != _T("testy.com"))
         {
            assert(0);
            throw;
         }


         delete pParser;
      }

      {
         String sTestStr = "The Honor Society of Phi Kappa Phi <webmaster@phiphi.org>";
         AddresslistParser* pParser= new AddresslistParser();
         
         std::vector<std::shared_ptr<Address> > vecResult = pParser->ParseList(sTestStr);
         if (vecResult.size() != 1)
         {
            assert(0);
            throw;
         }
         
         auto iterFirst = vecResult.begin();

         if ((*iterFirst)->sMailboxName != _T("webmaster"))
            throw;

         if ((*iterFirst)->sDomainName != _T("phiphi.org"))
            throw;

         delete pParser;
      }

      {  // SUPER advanced...
         
         String sTestStr = "<test@test.com>, \"Martin Knafve\" <martin@halvar.com>, \r\n"
                           "\"Martin Knafve\", test@test.com, Kalle";

         AddresslistParser* pParser= new AddresslistParser();

         std::vector<std::shared_ptr<Address> > vecResult = pParser->ParseList(sTestStr);

         if (vecResult.size() != 5)
         {
            assert(0);
            throw;
         }
         
         // Go to the second and check that it's right...
         auto iterFirst = vecResult.begin();

         iterFirst++;
         iterFirst++;
         iterFirst++;
         iterFirst++;

         if ((*iterFirst)->sPersonalName != _T("Kalle"))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sMailboxName != _T(""))
         {
            assert(0);
            throw;
         }

         if ((*iterFirst)->sDomainName != _T(""))
         {
            assert(0);
            throw;
         }

         delete pParser;
      }

      return true;
   }

   void 
   AddresslistParserTester::TestAddress_(const String &sHeader, const String &sFullname, const String &sUser, const String &sDomain)
   {
      AddresslistParser* pParser= new AddresslistParser();

      std::vector<std::shared_ptr<Address> > vecResult = pParser->ParseList(sHeader);

      if (vecResult.size() != 1)
      {
         assert(0);
         throw;
      }

      auto iterFirst = vecResult.begin();

      if ((*iterFirst)->sPersonalName != sFullname)
      {
         assert(0);
         throw;
      }

      if ((*iterFirst)->sMailboxName != sUser)
      {
         assert(0);
         throw;
      }

      if ((*iterFirst)->sDomainName != sDomain)
      {
         assert(0);
         throw;
      }

      delete pParser;
   }

}
