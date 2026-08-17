// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandList.h"
#include "IMAPConnection.h"
#include "IMAPSimpleCommandParser.h"
#include "../Common/BO/Account.h"
#include "../Common/BO/IMAPFolders.h"

#include "FolderListCreator.h"
#include "IMAPConfiguration.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCommandLIST::IMAPCommandLIST()
   {

   }

   IMAPCommandLIST::~IMAPCommandLIST()
   {

   }

   IMAPResult
   IMAPCommandLIST::ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      String sTag = pArgument->Tag();
      String sCommand = pArgument->Command();

      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());

      pParser->Parse(pArgument);

      // RFC 6154 LIST-EXTENDED: optional selection-option list before the reference
      // name, e.g. "LIST (SPECIAL-USE) "" "*"", and an optional RETURN option list
      // after the mailbox pattern, e.g. "LIST "" "*" RETURN (SPECIAL-USE)". We only
      // support the SPECIAL-USE option in either position, since that is the only
      // extension this server advertises.
      size_t iWordCount = pParser->WordCount();
      size_t iWordIdx = 1;

      bool bOnlySpecialUse = false;

      if (iWordIdx < iWordCount && pParser->Word(iWordIdx)->Paranthezied())
      {
         std::vector<String> vecSelectionOptions = StringParser::SplitString(pParser->Word(iWordIdx)->Value(), _T(" "));
         for (const String &sOption : vecSelectionOptions)
         {
            if (sOption.CompareNoCase(_T("SPECIAL-USE")) == 0)
               bOnlySpecialUse = true;
            else
               return IMAPResult(IMAPResult::ResultBad, "LIST Unsupported selection option.");
         }

         iWordIdx++;
      }

      if (iWordIdx + 2 > iWordCount)
         return IMAPResult(IMAPResult::ResultBad, "LIST Command requires a reference name and a mailbox name.");

      // Read parameters
      String sReferenceName = pParser->Word(iWordIdx)->Value();
      iWordIdx++;
      String sWildcards = pParser->Word(iWordIdx)->Value();
      iWordIdx++;

      // RFC 6154: SPECIAL-USE return option is implied by the SPECIAL-USE selection
      // option. Special-use attributes are always included in our LIST responses
      // (also for non-extended LIST), so RETURN (SPECIAL-USE) needs no extra work
      // beyond accepting the syntax instead of rejecting it as BAD.
      if (iWordIdx < iWordCount)
      {
         if (pParser->Word(iWordIdx)->Value().CompareNoCase(_T("RETURN")) != 0)
            return IMAPResult(IMAPResult::ResultBad, "LIST Invalid list-extended syntax.");

         iWordIdx++;

         if (iWordIdx >= iWordCount || !pParser->Word(iWordIdx)->Paranthezied())
            return IMAPResult(IMAPResult::ResultBad, "LIST Invalid RETURN option list.");

         std::vector<String> vecReturnOptions = StringParser::SplitString(pParser->Word(iWordIdx)->Value(), _T(" "));
         for (const String &sOption : vecReturnOptions)
         {
            if (sOption.CompareNoCase(_T("SPECIAL-USE")) != 0)
               return IMAPResult(IMAPResult::ResultBad, "LIST Unsupported RETURN option.");
         }

         iWordIdx++;
      }

      if (iWordIdx != iWordCount)
         return IMAPResult(IMAPResult::ResultBad, "LIST Invalid list-extended syntax.");

      String hierarchyDelimiter = Configuration::Instance()->GetIMAPConfiguration()->GetHierarchyDelimiter();

      String folderSpecifier = sWildcards;
      if (sReferenceName.GetLength() > 0)
         folderSpecifier = sReferenceName + folderSpecifier;

      std::shared_ptr<IMAPFolders> pFolders = pConnection->GetAccountFolders();
      std::shared_ptr<IMAPFolders> pPublicFolders = pConnection->GetPublicFolders();

      if (!pFolders || !pPublicFolders)
         return IMAPResult(IMAPResult::ResultNo, "LIST failed - No folders.");

      String sPublicFolderName = Configuration::Instance()->GetIMAPConfiguration()->GetIMAPPublicFolderName();

      String sResult =  FolderListCreator::GetIMAPFolderList(pConnection->GetAccount()->GetID(), pFolders, folderSpecifier, "", bOnlySpecialUse) +
                        FolderListCreator::GetIMAPFolderList(pConnection->GetAccount()->GetID(), pPublicFolders, folderSpecifier, sPublicFolderName, bOnlySpecialUse);

      if (sResult.IsEmpty() && sWildcards.IsEmpty())
      {
         hierarchyDelimiter.Replace(_T("\\"), _T("\\\\"));
         sResult = _T("* LIST (\\Noselect) \"") + hierarchyDelimiter + _T("\" \"\"\r\n");
      }

      sResult += sTag + " OK LIST completed\r\n";
      pConnection->SendAsciiData(sResult);   

      return IMAPResult();
   }
}
