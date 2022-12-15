// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandCopy.h"
#include "IMAPCopy.h"
#include "IMAPConnection.h"

#include "../Common/BO/IMAPFolder.h"

#include "IMAPSimpleCommandParser.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCommandCOPY::IMAPCommandCOPY()
   {

   }

   IMAPCommandCOPY::~IMAPCommandCOPY()
   {

   }


   IMAPResult
   IMAPCommandCOPY::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");
      
      if (!pConnection->GetCurrentFolder())
         return IMAPResult(IMAPResult::ResultNo, "No folder selected.");

      std::shared_ptr<IMAPCopy> pCopy = std::shared_ptr<IMAPCopy>(new IMAPCopy());
      pCopy->SetIsUID(false);

      String sResponse; 

      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());
      pParser->Parse(pArgument);
      if (pParser->ParamCount() != 2)
         return IMAPResult(IMAPResult::ResultBad, "Command requires 2 parameters.\r\n");

      String sMailNo = pParser->GetParamValue(pArgument, 0);
      String sFolderName = pParser->GetParamValue(pArgument, 1);

      pArgument->Command("\"" + sFolderName + "\"");

      // We should check if the folder exists. If not, notify user with trycreate
      std::shared_ptr<IMAPFolder> pFolder = pConnection->GetFolderByFullPath(sFolderName);

      if (!pFolder)
      {
         // Nope, doesn't exist.
         return IMAPResult(IMAPResult::ResultNo, "Can't find mailbox with that name.\r\n");
      }

      IMAPResult result = pCopy->DoForMails(pConnection, sMailNo, pArgument);

      if (result.GetResult() == IMAPResult::ResultOK)
          pConnection->SendAsciiData(pArgument->Tag() + " OK COPY completed\r\n");

      return result;
   }
}
