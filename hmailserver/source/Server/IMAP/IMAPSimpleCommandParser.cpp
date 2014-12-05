// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPSimpleCommandParser.h"
#include "IMAPCommand.h"
#include "../Common/BO/IMAPFolder.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   IMAPSimpleWord::IMAPSimpleWord() 
   {
         is_quoted_ = false; 
         is_paranthezied_ = false; 
         is_clammerized_ = false;
   }
   
   IMAPSimpleWord::~IMAPSimpleWord() {; };

   bool
   IMAPSimpleWord::Quoted() 
   {
      return is_quoted_; 
   }

   bool 
   IMAPSimpleWord::Paranthezied() 
   {
      return is_paranthezied_; 
   }

   bool 
   IMAPSimpleWord::Clammerized() 
   {
      return is_clammerized_; 
   }

   String 
   IMAPSimpleWord::Value()
   {  
      return word_;
   }


   void
   IMAPSimpleWord::Value(const String &sNewVal) 
   {
      word_ = sNewVal; 
   }

   

   IMAPSimpleCommandParser::IMAPSimpleCommandParser()
   {

   }

   IMAPSimpleCommandParser::~IMAPSimpleCommandParser()
   {

   }

   bool 
   IMAPSimpleCommandParser::Validate_(const String &command)
   {
      int length = command.GetLength();
      bool insideString = false;
      int openParanthesis = 0;
      for (int i = 0; i < length; i++)
      {
         wchar_t curChar = command.GetAt(i);

         switch (curChar)
         {
         case '"':
            insideString = !insideString;
            break;
         case '(':
            if (!insideString)
               openParanthesis++;
            break;
         case ')':
            if (!insideString)
               openParanthesis--;
            break;
         }
      }

      if (openParanthesis != 0)
         return false;

      return true;
   }

   void
   IMAPSimpleCommandParser::Parse(std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!Validate_(pArgument->Command()))
         return;

      int iCurWordStartPos = 0;

      const String sCommand = pArgument->Command();

      int iCurrentLiteralPos = 0;

      while (1)
      {
         
         bool bCurWordIsQuoted = false;
         bool bCurWordIsParanthezed = false;
         bool bCurWordIsClammerized = false;

         if (iCurWordStartPos >= sCommand.GetLength())
            break;

         if (sCommand.GetAt(iCurWordStartPos) == '\"')
         {
            // Move to the first character in the quoted string.
            iCurWordStartPos ++;
            bCurWordIsQuoted = true;
         }
         else if (sCommand.GetAt(iCurWordStartPos) == '(')
         {
            iCurWordStartPos ++;
            bCurWordIsParanthezed = true;
         }
         else if (sCommand.GetAt(iCurWordStartPos) == '{')
         {
            iCurWordStartPos ++;
            bCurWordIsClammerized = true;
         }

         int iCurWordEndPos = 0;

         if (bCurWordIsQuoted)
         {

            iCurWordEndPos = FindEndOfQuotedString_(sCommand, iCurWordStartPos);

            if (iCurWordEndPos < 0)
               return;
         }
         else if (bCurWordIsParanthezed)
         {
            // Find the end parentheses.
            bool bInString = false;
            int nestDeep = 1;
            for (int i = iCurWordStartPos; i < sCommand.GetLength(); i++)
            {
               wchar_t sCurPos = sCommand.GetAt(i);

               if (sCurPos == '"')
               {
                  bInString = !bInString;
                  continue;
               }

               if (bInString)
                  continue;

               if (sCurPos == '(')
                  nestDeep++;

               if (sCurPos == ')')
               {
                  nestDeep--;
                  
                  if (nestDeep == 0)
                  {
                     iCurWordEndPos = i;
                     break;
                  }
               }
            }

            if (iCurWordEndPos <= 0)
               iCurWordEndPos = sCommand.GetLength();
         }
         else if (bCurWordIsClammerized)
         {
            iCurWordEndPos = sCommand.Find(_T("}"), iCurWordStartPos);

            if (iCurWordEndPos < 0)
               return;
         }
         else
         {
            iCurWordEndPos = sCommand.Find(_T(" "), iCurWordStartPos);

            if (iCurWordEndPos == -1)
            {
               // Found end of string.
               iCurWordEndPos = sCommand.GetLength();
            }
         }

         

         int iCurWordLength = iCurWordEndPos - iCurWordStartPos;
         
         String sCurWord = sCommand.Mid(iCurWordStartPos, iCurWordLength);

         if (bCurWordIsQuoted)
         {
            // The characters " and \ are escaped so we need to unescape them.
            IMAPFolder::UnescapeFolderString(sCurWord);
         }

         std::shared_ptr<IMAPSimpleWord> pWord = std::shared_ptr<IMAPSimpleWord>(new IMAPSimpleWord);
         pWord->Value(sCurWord);
         pWord->Quoted(bCurWordIsQuoted);
         pWord->Paranthezied(bCurWordIsParanthezed);
         pWord->Clammerized(bCurWordIsClammerized);

         if (bCurWordIsClammerized)
         {
            pWord->LiteralData(pArgument->Literal(iCurrentLiteralPos));
            iCurrentLiteralPos++;
         }

         parsed_words_.push_back(pWord);
         
         if (bCurWordIsQuoted || bCurWordIsParanthezed || bCurWordIsClammerized)
            iCurWordStartPos = iCurWordEndPos + 2;
         else
            iCurWordStartPos = iCurWordEndPos + 1;
      }



   }

   int
   IMAPSimpleCommandParser::FindEndOfQuotedString_(const String &sInputString, int iWordStartPos)
   {
      int i = iWordStartPos;      
      while ( i < sInputString.GetLength())
      {
         if (sInputString.GetAt(i) == '\\')
         {
            // The next character is escaped, so we should skip one exxra.
            i++;
         }
         else if (sInputString.GetAt(i) == '\"')
         {
            return i;
         }

         i++;
      }

      return i;
   }

   /*
      Convert literal data to value data.
   */
   void 
   IMAPSimpleCommandParser::UnliteralData()
   {
      for (size_t i = 0; i < WordCount(); i++)
      {
         std::shared_ptr<IMAPSimpleWord> pWord = Word(i);
         if (pWord->Clammerized())
         {
            if (!pWord->LiteralData().IsEmpty())
               pWord->Value(pWord->LiteralData());
         }
      }
   }

   std::shared_ptr<IMAPSimpleWord>
   IMAPSimpleCommandParser::QuotedWord()
   {
      auto iterWord = parsed_words_.begin();
      while (iterWord != parsed_words_.end())
      {
         if ((*iterWord)->Quoted())
            return (*iterWord);
         iterWord++;
      }

      std::shared_ptr<IMAPSimpleWord> pEmpty;
      return pEmpty;
   }

   void 
   IMAPSimpleCommandParser::RemoveWord(int iWordIdx)
   {
      int iCurIdx = 0;
      auto iterWord = parsed_words_.begin();
      while (iterWord != parsed_words_.end())
      {  
         if (iWordIdx == iCurIdx)
         {
            parsed_words_.erase(iterWord);
            return;
         }

         iterWord++;
         iCurIdx++;
      }
   }


   std::shared_ptr<IMAPSimpleWord>
   IMAPSimpleCommandParser::ParantheziedWord()
   {
      auto iterWord = parsed_words_.begin();
      while (iterWord != parsed_words_.end())
      {
         if ((*iterWord)->Paranthezied())
            return (*iterWord);
         iterWord++;
      }

      std::shared_ptr<IMAPSimpleWord> pEmpty;
      return pEmpty;
   }

   std::shared_ptr<IMAPSimpleWord>
   IMAPSimpleCommandParser::ClammerizedWord()
   {
      auto iterWord = parsed_words_.begin();
      while (iterWord != parsed_words_.end())
      {
         if ((*iterWord)->Clammerized())
            return (*iterWord);
         iterWord++;
      }

      std::shared_ptr<IMAPSimpleWord> pEmpty;
      return pEmpty;
   }

   String 
   IMAPSimpleCommandParser::GetParamValue(std::shared_ptr<IMAPCommandArgument> pArguments, int iParamIndex)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns a parameter by it's index.
   //---------------------------------------------------------------------------()
   {
      iParamIndex++;
      int iWordCount = (int) WordCount();
      int iLiteralIndex = 0;

      for (int i = 1; i < iWordCount; i++)
      {
         std::shared_ptr<IMAPSimpleWord> pWord = Word(i);

         if (!pWord)
            return "";

         if (pWord->Clammerized())
         {
            if (i == iParamIndex)
               return pArguments->Literal(iLiteralIndex);
               
            iLiteralIndex++;
         }
         else
         {
            if (i == iParamIndex)
            {
               String sValue = pWord->Value();
               return sValue;
            }
         }
      }

      return "";
   }

   IMAPSimpleCommandParserTester::IMAPSimpleCommandParserTester()
   {
            
   };

   bool 
   IMAPSimpleCommandParserTester::Test()
   {
      {  //  LIST "PARAM1" "PARAM2"

         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();

         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("LIST \"PARAM1\" \"PARAM2\"");
         pParser->Parse(pArgument);

         if (pParser->WordCount() != 3)
         {
            assert(0);
            throw;
         }

         if (pParser->Word(1)->Value() != _T("PARAM1") || pParser->Word(2)->Value() != _T("PARAM2"))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  LIST PARAM1 PARAM2

         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("LIST PARAM1 PARAM2");
         pParser->Parse(pArgument);

         if (pParser->WordCount() != 3)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(1)->Value() != _T("PARAM1") || pParser->Word(2)->Value() != _T("PARAM2"))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  LIST "PARAM1" PARAM2

         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("LIST \"PARAM1\" PARAM2");
         pParser->Parse(pArgument);


         if (pParser->WordCount() != 3)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(1)->Value() != _T("PARAM1") || pParser->Word(2)->Value() != _T("PARAM2"))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  LIST PARAM1 "PARAM2"
         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("LIST PARAM1 \"PARAM2\"");
         pParser->Parse(pArgument);


         if (pParser->WordCount() != 3)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(1)->Value() != _T("PARAM1") || pParser->Word(2)->Value() != _T("PARAM2"))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  LIST "PARAM1"
         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("LIST PARAM1");
         pParser->Parse(pArgument);


         if (pParser->WordCount() != 2)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(1)->Value() != _T("PARAM1"))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  LIST PARAMTEST
         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("LIST PARAMTEST");
         pParser->Parse(pArgument);


         if (pParser->WordCount() != 2)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(1)->Value() != _T("PARAMTEST"))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  LIST PARAMTEST
         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("LIST PARAMTEST (I AM SICK)");
         pParser->Parse(pArgument);


         if (pParser->WordCount() != 3)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(2)->Value() != _T("I AM SICK"))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  LIST PARAMTEST
         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("LIST \"INBOX*\"");
         pParser->Parse(pArgument);


         if (pParser->WordCount() != 2)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(1)->Value() != _T("INBOX*"))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  LIST PARAMTEST

         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("CREATE \"INBOX.\\\"Hej\\\"");
         pParser->Parse(pArgument);


         if (pParser->WordCount() != 2)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(1)->Value() != _T("INBOX.\"Hej\""))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  LIST PARAMTEST

         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("CREATE \"INBOX.\\\"\"");
         pParser->Parse(pArgument);


         if (pParser->WordCount() != 2)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(1)->Value() != _T("INBOX.\""))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  TEST 

         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("");
         pParser->Parse(pArgument);

         if (pParser->WordCount() != 0)
         {
            assert(0);
            throw;
         }

         
         delete pParser;
      }

      {  //  PARANTHESE

         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("CREATE (TEST)");
         pParser->Parse(pArgument);


         if (pParser->WordCount() != 2)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(1)->Value() != _T("TEST"))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  PARANTHESE2

         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("CREATE (\"TEST\")");
         pParser->Parse(pArgument);


         if (pParser->WordCount() != 2)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(1)->Value() != _T("\"TEST\""))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  PARANTHESE3

         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("CREATE (\"TE\"()\"ST\")");
         pParser->Parse(pArgument);


         if (pParser->WordCount() != 2)
         {
            assert(0);
            throw;
         }
        
         if (pParser->Word(1)->Value() != _T("\"TE\"()\"ST\""))
         {
            assert(0);
            throw;
         }
         
         delete pParser;
      }

      {  //  PARANTHESE3
         // Purpose: Detect invalid paranthesis.
         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("CREATE (TE((()ST)");
         pParser->Parse(pArgument);

         if (pParser->WordCount() != 0)
         {
            assert(0);
            throw;
         }

         
         delete pParser;
      }      

      {  //  PARANTHESE4
         // Purpose: Detect invalid paranthesis.

         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("CREATE (TEST");
         pParser->Parse(pArgument);

         if (pParser->WordCount() != 0)
         {
            assert(0);
            throw;
         }

         
         delete pParser;
      }    

      {  // PARANTHESE5
         // Correct paranthesis which may look incorrect

         IMAPSimpleCommandParser *pParser = new IMAPSimpleCommandParser();
         std::shared_ptr<IMAPCommandArgument> pArgument = std::shared_ptr<IMAPCommandArgument>(new IMAPCommandArgument);
         pArgument->Command("CREATE \"T((est\"");
         pParser->Parse(pArgument);

         if (pParser->WordCount() != 2)
         {
            assert(0);
            throw;
         }


         delete pParser;
      }  

      return true;
   }


}
