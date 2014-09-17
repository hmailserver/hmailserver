// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandUID.h"
#include "IMAPConnection.h"
#include "IMAPSimpleCommandParser.h"


#include "IMAPFetch.h"
#include "IMAPCopy.h"
#include "IMAPStore.h"
#include "IMAPCommandSearch.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCommandUID::IMAPCommandUID()
   {

   }

   IMAPCommandUID::~IMAPCommandUID()
   {

   }


   IMAPResult
   IMAPCommandUID::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      String sTag = pArgument->Tag();
      String sCommand = pArgument->Command();

      if (!pConnection->GetCurrentFolder())
         return IMAPResult(IMAPResult::ResultNo, "No folder selected.");

      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());

      pParser->Parse(pArgument);
      
      if (pParser->WordCount() < 2)
         return IMAPResult(IMAPResult::ResultBad, "Command requires at least 1 parameter.");

      String sTypeOfUID = pParser->Word(1)->Value();

      if (sTypeOfUID.CompareNoCase(_T("FETCH")) == 0)
      {
         if (pParser->WordCount() < 4)
            return IMAPResult(IMAPResult::ResultBad, "Command requires at least 3 parameters.");

         command_ = std::shared_ptr<IMAPFetch>(new IMAPFetch());
      }
      else if (sTypeOfUID.CompareNoCase(_T("COPY")) == 0)
      {

         if (pParser->WordCount() < 4)
            return IMAPResult(IMAPResult::ResultBad, "Command requires at least 3 parameters.");

         command_ = std::shared_ptr<IMAPCopy>(new IMAPCopy());
      }
      else if (sTypeOfUID.CompareNoCase(_T("STORE")) == 0)
      {
         if (pParser->WordCount() < 4)
            return IMAPResult(IMAPResult::ResultBad, "Command requires at least 3 parameters.");

         command_ = std::shared_ptr<IMAPStore>(new IMAPStore());
      }
      else if (sTypeOfUID.CompareNoCase(_T("SEARCH")) == 0)
      {
         std::shared_ptr<IMAPCommandSEARCH> pCommand = std::shared_ptr<IMAPCommandSEARCH> (new IMAPCommandSEARCH(false));
         pCommand->SetIsUID();
         IMAPResult result = pCommand->ExecuteCommand(pConnection, pArgument);

         if (result.GetResult() == IMAPResult::ResultOK)
            pConnection->SendAsciiData(sTag + " OK UID completed\r\n");

         return result;
      }
      else if (sTypeOfUID.CompareNoCase(_T("SORT")) == 0)
      {
         std::shared_ptr<IMAPCommandSEARCH> pCommand = std::shared_ptr<IMAPCommandSEARCH> (new IMAPCommandSEARCH(true));
         pCommand->SetIsUID();
         IMAPResult result = pCommand->ExecuteCommand(pConnection, pArgument);
         
         if (result.GetResult() == IMAPResult::ResultOK)
            pConnection->SendAsciiData(sTag + " OK UID completed\r\n");

         return result;
      }


      if (!command_)
         return IMAPResult(IMAPResult::ResultBad, "Bad command.");

      command_->SetIsUID(true);

      // Copy the first word containing the message sequence
      long lSecWordStartPos = sCommand.Find(_T(" "), 5) + 1;
      long lSecWordEndPos = sCommand.Find(_T(" "), lSecWordStartPos);
      long lSecWordLength = lSecWordEndPos - lSecWordStartPos;
      String sMailNo = sCommand.Mid(lSecWordStartPos, lSecWordLength);
      
      // Copy the second word containing the actual command.
      String sShowPart = sCommand.Mid(lSecWordEndPos + 1);

      if (sMailNo.IsEmpty())
         return IMAPResult(IMAPResult::ResultBad, "No mail number specified");

      if (!StringParser::ValidateString(sMailNo, "01234567890,.:*"))
         return IMAPResult(IMAPResult::ResultBad, "Incorrect mail number");

      // Set the command to execute as argument
      pArgument->Command(sShowPart);

      // Execute the command. If we have gotten this far, it means that the syntax
      // of the command is correct. If we fail now, we should return NO. 
      IMAPResult result = command_->DoForMails(pConnection, sMailNo, pArgument);

      if (result.GetResult() == IMAPResult::ResultOK)
         pConnection->SendAsciiData(pArgument->Tag() + " OK UID completed\r\n");

      return result;
   }

}