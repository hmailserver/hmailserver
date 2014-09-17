// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPCommandListRights.h"
#include "IMAPConnection.h"
#include "IMAPSimpleCommandParser.h"
#include "IMAPConfiguration.h"

#include "../Common/BO/IMAPFolder.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPResult
   IMAPCommandListRights::ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      if (!Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPACL())
         return IMAPResult(IMAPResult::ResultBad, "ACL is not enabled.");

      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());
      pParser->Parse(pArgument);

      if (pParser->ParamCount() != 2)
         return IMAPResult(IMAPResult::ResultBad, "ListRights command requires 2 parameter.");

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

      // Always produce the same output.
      String sIdentifier = pParser->Word(2)->Value();

      String sPermissions = "l r s w i k x t e a";

      String sResponse;
      sResponse.Format(_T("* LISTRIGHTS %s %s %s\r\n"), sOriginalFolderName.c_str(), sIdentifier.c_str(), sPermissions.c_str());
      sResponse += pArgument->Tag() + _T(" OK ListRights complete\r\n");

      pConnection->SendAsciiData(sResponse);   

      return IMAPResult();
   }
}