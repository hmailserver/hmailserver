// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPCommandMyRights.h"
#include "IMAPConnection.h"
#include "IMAPSimpleCommandParser.h"
#include "IMAPConfiguration.h"


#include "../Common/Application/ACLManager.h"
#include "../Common/BO/Account.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/ACLPermission.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPResult
   IMAPCommandMyRights::ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      if (!Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPACL())
         return IMAPResult(IMAPResult::ResultBad, "ACL is not enabled.");

      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());
      pParser->Parse(pArgument);

      if (pParser->WordCount() < 2)
         return IMAPResult(IMAPResult::ResultBad, "MYRIGHTS command requires at least 1 parameter.");

      String sOriginalFolderName;
      String sFolderName;

      if (pParser->Word(1)->Clammerized())
      {
         sFolderName = pArgument->Literal(0);
         sOriginalFolderName = sFolderName;
      }
      else
      {
         sOriginalFolderName = pParser->Word(1)->Value();
         
         sFolderName = sOriginalFolderName;
         IMAPFolder::UnescapeFolderString(sFolderName);
      }
      
      std::shared_ptr<IMAPFolder> pSelectedFolder = pConnection->GetFolderByFullPath(sFolderName);
      if (!pSelectedFolder)
         return IMAPResult(IMAPResult::ResultBad, "Folder could not be found.");

	  ACLManager aclManager;
      std::shared_ptr<ACLPermission> pPermission = aclManager.GetPermissionForFolder(pConnection->GetAccount()->GetID(), pSelectedFolder);
      String sRightsString;
      if (pPermission)
         sRightsString = pPermission->GetRights();

      String sResponse;
      sResponse.Format(_T("* MYRIGHTS \"%s\" %s\r\n"), sOriginalFolderName.c_str(), sRightsString.c_str());
      sResponse += pArgument->Tag() + _T(" OK Myrights complete\r\n");

      pConnection->SendAsciiData(sResponse);   

      return IMAPResult();
   }
}