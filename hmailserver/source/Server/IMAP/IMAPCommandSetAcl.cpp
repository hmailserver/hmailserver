// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPCommandSetAcl.h"
#include "IMAPConnection.h"
#include "IMAPSimpleCommandParser.h"
#include "IMAPACLHelper.h"
#include "IMAPConfiguration.h"

#include "../Common/Application/ACLManager.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/ACLPermission.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPResult
   IMAPCommandSetAcl::ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      if (!Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPACL())
         return IMAPResult(IMAPResult::ResultBad, "ACL is not enabled.");

      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());
      pParser->Parse(pArgument);

      if (pParser->ParamCount() < 3)
         return IMAPResult(IMAPResult::ResultBad, "SETACL command requires 3 parameter.");

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
      
      std::shared_ptr<IMAPFolder> pFolder = pConnection->GetFolderByFullPath(sFolderName);
      if (!pFolder)
         return IMAPResult(IMAPResult::ResultBad, "Folder could not be found.");

      if (pFolder->GetAccountID() != 0)
         return IMAPResult(IMAPResult::ResultNo, "It is not possible to set permisssion for account folders.");

      // Check that user has permission to do this.
      if (!pConnection->CheckPermission(pFolder, ACLPermission::PermissionAdminister))
         return IMAPResult(IMAPResult::ResultNo, "Permission denied.");

      String sIdentifier = pParser->Word(2)->Value();
      String sAccessRights = pParser->Word(3)->Value();

      // Check if the given access rights are really valid.
      if (!IMAPACLHelper::IsValidPermissionString(sAccessRights))
         return IMAPResult(IMAPResult::ResultNo, "SetACL failed. Invalid access right string");

	  ACLManager aclManager;
      if (!aclManager.SetACL(pFolder, sIdentifier, sAccessRights))
         return IMAPResult(IMAPResult::ResultNo, "SetACL failed.");

      /*
         3.1 SETACL command, RFC 4314
         Note that an unrecognized right MUST cause the command to return the
         BAD response. 
      */

      String sResponse = pArgument->Tag() + _T(" OK SETACL complete\r\n");
      pConnection->SendAsciiData(sResponse);   

      return IMAPResult();
   }
}