// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPSearchParser.h"
#include "IMAPSimpleCommandParser.h"
#include "IMAPCommand.h"
#include "IMAPSortParser.h"

#include "../Common/Util/RegularExpression.h"
#include "../Common/Util/Unicode.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPSearchCriteria::CriteriaType 
   IMAPSearchCriteria::GetCriteriaTypeByName(const String &sName)
   {
      String sTmp = sName;
      sTmp.MakeLower();
      
      if (sTmp.Compare(_T("")) == 0)
         return CTUnknown;
      else if (sTmp.Compare(_T("charset")) == 0)
         return CTCharset;
      else if (sTmp == _T("on"))
         return CTOn;
      else if (sTmp == _T("header"))
         return CTHeader;
      else if (sTmp == _T("text"))
         return CTText;
      else if (sTmp == _T("body"))
         return CTBody;
      else if (sTmp == _T("subject"))
         return CTSubject;
      else if (sTmp == _T("from"))
         return CTFrom;
      else if (sTmp == _T("cc"))
         return CTCC;
      else if (sTmp == _T("to"))
         return CTTo;
      else if (sTmp == _T("senton"))
         return CTSentOn;
      else if (sTmp == _T("sentbefore"))
         return CTSentBefore;
      else if (sTmp == _T("sentsince"))
         return CTSentSince;
      else if (sTmp == _T("since"))
         return CTSince;
      else if (sTmp == _T("deleted"))
         return CTDeleted;
      else if (sTmp == _T("recent"))
         return CTRecent;
      else if (sTmp == _T("seen"))
         return CTSeen;
      else if (sTmp == _T("unseen"))
         return CTUnseen;
      else if (sTmp == _T("undeleted"))
         return CTUndeleted;
      else if (sTmp == _T("uid"))
         return CTUID;
      else if (sTmp == _T("answered"))
         return CTAnswered;
      else if (sTmp == _T("before"))
         return CTBefore;
      else if (sTmp == _T("draft"))
         return CTDraft;
      else if (sTmp == _T("flagged"))
         return CTFlagged;
      else if (sTmp == _T("larger"))
         return CTLarger;
      else if (sTmp == _T("new"))
         return CTNew;
      else if (sTmp == _T("old"))
         return CTOld;
      else if (sTmp == _T("smaller"))
         return CTSmaller;
      else if (sTmp == _T("unanswered"))
         return CTUnanswered;
      else if (sTmp == _T("undraft"))
         return CTUndraft;
      else if (sTmp == _T("unflagged"))
         return CTUnflagged;
      else if (sTmp == _T("all"))
         return CTAll;
      else if (_IsSequenceSet(sTmp))
         return CTSequenceSet;

      return CTUnknown;
   }

   bool 
   IMAPSearchCriteria::_IsSequenceSet(const String &item)
   {
      const String sequenceSetRegex = "[0-9:,*]*";

      return RegularExpression::TestExactMatch(sequenceSetRegex, item);
   }



   IMAPSearchParser::IMAPSearchParser()
   {


   }

   IMAPSearchParser::~IMAPSearchParser()
   {

   }

   IMAPResult 
   IMAPSearchParser::ParseCommand(shared_ptr<IMAPCommandArgument> pArgument, bool bIsSort)
   {
      // Replace literals in the command.
      shared_ptr<IMAPSimpleCommandParser> pSimpleParser = shared_ptr<IMAPSimpleCommandParser> (new IMAPSimpleCommandParser);

      if (bIsSort)
      {
         pSimpleParser->Parse(pArgument);
         pSimpleParser->UnliteralData();

         shared_ptr<IMAPSimpleWord> pSort = pSimpleParser->Word(0);
         if (pSort->Paranthezied())
         {
            m_pSortParser = shared_ptr<IMAPSortParser>(new IMAPSortParser);
            m_pSortParser->Parse(pSort->Value());
         }

         // Trim away the SORT part of the SEARCH expresson 
         // since we only care about SEARCH below.
         String tempString = pArgument->Command();

         if (tempString.Find(_T(")")) > 0)
            tempString = tempString.Mid(tempString.Find(_T(")"))+2);

         pArgument->Command(tempString);
      }

      /* 
         Remove all parenthesis outside of strings. Some IMAP
         clients sends parenthesis, some doesn't. We remove
         them here to prevent the behaviors from differing.

         It should be safe to do this, since
            Criteria1 OR (Criteria2 Criteria 3)
         means the same as
            Criteria1 OR Criteria2 Criteria 3
      */

      String resultString;
      const String inputString = pArgument->Command();
      bool insideString = false;
      for (int i = 0; i < inputString.GetLength(); i++)
      {
         wchar_t curChar = inputString.GetAt(i);

         switch (curChar)
         {
         case '"':
            insideString = !insideString;
            break;
         case '(':
         case ')':
            if (!insideString)
               continue;
         }

         resultString += curChar;
      }

      // Replace literals in the command.
      pSimpleParser = shared_ptr<IMAPSimpleCommandParser> (new IMAPSimpleCommandParser);
      pArgument->Command(resultString);

      pSimpleParser->Parse(pArgument);
      pSimpleParser->UnliteralData();

      shared_ptr<IMAPSearchCriteria> pCriteria = shared_ptr<IMAPSearchCriteria> (new IMAPSearchCriteria);

      int currentWord = 0;
      IMAPResult result = _ParseSegment(pSimpleParser, currentWord, pCriteria, 0);
      if (result.GetResult() != IMAPResult::ResultOK)
         return result;

      m_pResultCriteria = pCriteria;

      return IMAPResult();
   }

   IMAPResult
   IMAPSearchParser::_ParseSegment(shared_ptr<IMAPSimpleCommandParser> pSimpleParser, int &currentWord, shared_ptr<IMAPSearchCriteria> pCriteria, int iRecursion)
   {
      iRecursion++;
      if (iRecursion > 50)
      {
         return IMAPResult(IMAPResult::ResultNo, "Search failed due to excessive search expression recursion.");
      }
      
      int originalCriteriaCount = pCriteria->GetSubCriterias().size();
      for (; currentWord < pSimpleParser->WordCount(); currentWord++)
      {
         shared_ptr<IMAPSimpleWord> pWord = pSimpleParser->Word(currentWord);
         String sCurCommand = pWord->Value().ToUpper();

         

         if (sCurCommand == _T("OR"))
         {
            // We have a sub argument.
            shared_ptr<IMAPSearchCriteria> pSubCriteria = shared_ptr<IMAPSearchCriteria> (new IMAPSearchCriteria());

            pSubCriteria->SetType(IMAPSearchCriteria::CTSubCriteria);
            pSubCriteria->SetIsOR(true);

            currentWord++;
            IMAPResult result = _ParseSegment(pSimpleParser, currentWord, pSubCriteria, iRecursion);
            if (result.GetResult() != IMAPResult::ResultOK)
               return result;
            
            pCriteria->GetSubCriterias().push_back(pSubCriteria);

            continue;
         }
 
         shared_ptr<IMAPSearchCriteria> pNewCriteria = shared_ptr<IMAPSearchCriteria> (new IMAPSearchCriteria);
         IMAPResult result = _ParseWord(pSimpleParser, pNewCriteria, currentWord );
         if (result.GetResult() != IMAPResult::ResultOK)
            return result;
         
         if (pNewCriteria->GetType() != IMAPSearchCriteria::CTUnknown)
            pCriteria->GetSubCriterias().push_back(pNewCriteria);

         if (iRecursion > 1)
         {
            // This is a sub criteria. We only add two words here.
            if (originalCriteriaCount + 2 == pCriteria->GetSubCriterias().size())
            {
               return IMAPResult();
            }
         }
      }

      return IMAPResult();
   }

   IMAPResult 
   IMAPSearchParser::_ParseWord(shared_ptr<IMAPSimpleCommandParser> pSimpleParser, shared_ptr<IMAPSearchCriteria> pNewCriteria, int &iCurrentWord)
   {
      String sCurCommand = pSimpleParser->Word(iCurrentWord)->Value();

      if (sCurCommand == _T("NOT"))
      {
         pNewCriteria->SetPositive(false);
         iCurrentWord++;

         if (iCurrentWord > pSimpleParser->WordCount() - 1)
            return IMAPResult(IMAPResult::ResultBad, "Syntax error. NOT used but no search criteria specified.");

         shared_ptr<IMAPSimpleWord> pWord = pSimpleParser->Word(iCurrentWord);
         sCurCommand = pWord->Value().ToUpper();
      }
      else
         pNewCriteria->SetPositive(true);

      IMAPSearchCriteria::CriteriaType ct = IMAPSearchCriteria::GetCriteriaTypeByName(sCurCommand);

      if (ct == IMAPSearchCriteria::CTHeader)
      {
         // The client want's to search for a 
         // header field.
         iCurrentWord++;

         if (iCurrentWord > pSimpleParser->WordCount() - 1)
            return IMAPResult(IMAPResult::ResultBad, "Syntax error. No header specified in header-search.");

         String sHeaderField = pSimpleParser->Word(iCurrentWord)->Value();            

         // Go to the header value.
         iCurrentWord++;

         if (iCurrentWord > pSimpleParser->WordCount() - 1)
            return IMAPResult(IMAPResult::ResultBad, "Syntax error. No value specified in header-search.");

         String sHeaderValue = pSimpleParser->Word(iCurrentWord)->Value();

         sHeaderValue = _DecodeWordAccordingToCharset(sHeaderValue);

         pNewCriteria->SetHeaderField(sHeaderField);
         pNewCriteria->SetText(sHeaderValue);
         pNewCriteria->SetType(IMAPSearchCriteria::CTHeader);         

      }
      else if (ct == IMAPSearchCriteria::CTText ||
         ct == IMAPSearchCriteria::CTSentOn ||
         ct == IMAPSearchCriteria::CTSentBefore ||
         ct == IMAPSearchCriteria::CTSentSince ||
         ct == IMAPSearchCriteria::CTBody ||
         ct == IMAPSearchCriteria::CTSubject ||
         ct == IMAPSearchCriteria::CTTo ||
         ct == IMAPSearchCriteria::CTFrom ||
         ct == IMAPSearchCriteria::CTCC ||
         ct == IMAPSearchCriteria::CTSince || 
         ct == IMAPSearchCriteria::CTBefore ||
         ct == IMAPSearchCriteria::CTLarger ||
         ct == IMAPSearchCriteria::CTSmaller ||
         ct == IMAPSearchCriteria::CTOn)
      {
         iCurrentWord++;

         if (iCurrentWord > pSimpleParser->WordCount() - 1)
            return IMAPResult(IMAPResult::ResultBad, "Syntax error. Missing value.");

         shared_ptr<IMAPSimpleWord> pWord = pSimpleParser->Word(iCurrentWord);

         if (pWord)
         {
            String searchValue = pWord->Value();

            if (_NeedsDecoding(ct))
               searchValue = _DecodeWordAccordingToCharset(searchValue);

            pNewCriteria->SetText(searchValue);
            pNewCriteria->SetType(ct);
         }

      }
      else if (ct == IMAPSearchCriteria::CTUID)
      {
         // Check which UID's we should send back.
         //

         iCurrentWord++;

         if (iCurrentWord > pSimpleParser->WordCount() - 1)
            return IMAPResult(IMAPResult::ResultBad, "Syntax error. UID parameters missing.");

         shared_ptr<IMAPSimpleWord> pWord = pSimpleParser->Word(iCurrentWord);
         if (pWord)
         {
            String sTemp = pWord->Value();
            std::vector<String> vecSplit = StringParser::SplitString(sTemp, ",");
            pNewCriteria->SetSequenceSet(vecSplit);
            pNewCriteria->SetType(ct);

         }            
      }
      else if (ct == IMAPSearchCriteria::CTDeleted ||
         ct == IMAPSearchCriteria::CTUndeleted ||
         ct == IMAPSearchCriteria::CTSeen ||
         ct == IMAPSearchCriteria::CTUnseen ||
         ct == IMAPSearchCriteria::CTRecent ||
         ct == IMAPSearchCriteria::CTAnswered ||
         ct == IMAPSearchCriteria::CTDraft ||
         ct == IMAPSearchCriteria::CTFlagged ||
         ct == IMAPSearchCriteria::CTNew ||
         ct == IMAPSearchCriteria::CTOld ||
         ct == IMAPSearchCriteria::CTUnanswered ||
         ct == IMAPSearchCriteria::CTUndraft ||
         ct == IMAPSearchCriteria::CTUnflagged ||
         ct == IMAPSearchCriteria::CTAll)
      {
         pNewCriteria->SetType(ct);
      }
      else if (ct == IMAPSearchCriteria::CTSequenceSet)
      {
         pNewCriteria->SetType(ct);
         pNewCriteria->SetSequenceSet(StringParser::SplitString(sCurCommand, ","));
      }
      else if (ct == IMAPSearchCriteria::CTCharset)
      {
         iCurrentWord++;

         if (iCurrentWord > pSimpleParser->WordCount() - 1)
            return IMAPResult(IMAPResult::ResultBad, "Syntax error. Missing charset name.");

         shared_ptr<IMAPSimpleWord> pWord = pSimpleParser->Word(iCurrentWord);

         if (pWord)
         {
            String charsetName = pWord->Value();

            if (!_IsValidCharset(charsetName))
               return IMAPResult(IMAPResult::ResultNo, "[BADCHARSET]");

            _charsetName = charsetName;
         }
      }

      return IMAPResult();
   }

   bool 
   IMAPSearchParser::_IsValidCharset(const String &charsetName)
   {
      if (charsetName.CompareNoCase(_T("UTF-8")) == 0 ||
          charsetName.CompareNoCase(_T("US-ASCII")) == 0 ||
          charsetName.CompareNoCase(_T("ISO-8859-1")) == 0)
          return true;
      else
          return false;
   }

   bool 
   IMAPSearchParser::_NeedsDecoding(IMAPSearchCriteria::CriteriaType criteriaType)
   {
      switch (criteriaType)
      {
      case IMAPSearchCriteria::CTHeader:
      case IMAPSearchCriteria::CTText:
      case IMAPSearchCriteria::CTBody:
      case IMAPSearchCriteria::CTSubject:
      case IMAPSearchCriteria::CTFrom:
      case IMAPSearchCriteria::CTTo:
      case IMAPSearchCriteria::CTCC:
         return true;
      default:
         return false;
      }
   }

   String
   IMAPSearchParser::_DecodeWordAccordingToCharset(const String &inputValue)
   {
      if (_charsetName.CompareNoCase(_T("UTF-8")) == 0)
      {
         String resultValue;
         if (Unicode::MultiByteToWide(inputValue, resultValue))
            return resultValue;
         else
            return inputValue;
      }
      else
         return inputValue;
   }

 
}