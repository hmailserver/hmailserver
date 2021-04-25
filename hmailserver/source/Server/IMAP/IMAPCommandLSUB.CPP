// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandLSUB.h"
#include "IMAPConnection.h"
#include "IMAPSimpleCommandParser.h"
#include "../Common/BO/Account.h"
#include "../Common/BO/IMAPFolders.h"

#include "IMAPConfiguration.h"
#include "FolderListCreator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPResult
   IMAPCommandLSUB::ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      String sResponse;
   
      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());
      
      pParser->Parse(pArgument);

      if (pParser->WordCount() != 3)
         return IMAPResult(IMAPResult::ResultBad, "LSUB Command requires 2 parameters.");

      // We ignore the input reference (param 1)
      String sReference = pParser->Word(1)->Value();
      String sWildcards = pParser->Word(2)->Value();

      String folderSpecifier = sWildcards;
      if (sReference.GetLength() > 0)
         folderSpecifier = sReference + folderSpecifier;

      std::shared_ptr<IMAPFolders> pAccountFolders = pConnection->GetAccountFolders();
      std::shared_ptr<IMAPFolders> pPublicFolders = pConnection->GetPublicFolders();

      if (!pAccountFolders || !pPublicFolders)
         return IMAPResult(IMAPResult::ResultNo, "LIST failed - No folders.");

      String sPublicFolderName = Configuration::Instance()->GetIMAPConfiguration()->GetIMAPPublicFolderName(); 

      String sResult = FolderListCreator::GetIMAPLSUBFolderList(pConnection->GetAccount()->GetID(), pAccountFolders, folderSpecifier, "") + 
                       FolderListCreator::GetIMAPLSUBFolderList(pConnection->GetAccount()->GetID(), pPublicFolders, folderSpecifier, sPublicFolderName);

      pConnection->SendAsciiData(sResult);   

      sResponse += pArgument->Tag() + " OK LSUB completed\r\n";
      pConnection->SendAsciiData(sResponse);   

      return IMAPResult();

   }

}