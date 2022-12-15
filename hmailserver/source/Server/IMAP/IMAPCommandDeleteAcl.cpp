// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPCommandDeleteAcl.h"

#include "IMAPConnection.h"
#include "IMAPSimpleCommandParser.h"
#include "IMAPConfiguration.h"

#include "../Common/Cache/CacheContainer.h"
#include "../Common/BO/Account.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/ACLPermission.h"
#include "../Common/BO/ACLPermissions.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPResult
   IMAPCommandDeleteAcl::ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      if (!Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPACL())
         return IMAPResult(IMAPResult::ResultBad, "ACL is not enabled.");

      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());
      pParser->Parse(pArgument);

      if (pParser->ParamCount() != 2)
         return IMAPResult(IMAPResult::ResultBad, "DELETEACL command requires 2 parameter.");

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

      String sIdentifier = pParser->Word(2)->Value();
      
      std::shared_ptr<IMAPFolder> pFolder = pConnection->GetFolderByFullPath(sFolderName);
      if (!pFolder)
         return IMAPResult(IMAPResult::ResultBad, "Folder could not be found.");

      // Check if the user has access to delete ACL for this folder
      if (!pConnection->CheckPermission(pFolder, ACLPermission::PermissionAdminister))
         return IMAPResult(IMAPResult::ResultNo, "ACL: DeleteACL permission denied");

      // Assume identifier is a user and not a group.
      std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(sIdentifier);

      if (!pAccount)
      {
         // Do something. We cannot give permissions to an object which does not exist.
         return IMAPResult(IMAPResult::ResultNo, "DeleteACL failed. Identifier not found");
      }

      if (!pFolder->GetPermissions()->DeletePermissionsForAccount(pAccount->GetID()))
         return IMAPResult(IMAPResult::ResultNo, "DeleteACL failed");

      String sResponse = pArgument->Tag() + _T(" OK DeleteACL complete\r\n");
      pConnection->SendAsciiData(sResponse);   

      return IMAPResult();
   }
}