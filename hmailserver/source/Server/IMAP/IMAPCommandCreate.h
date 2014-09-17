// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"

namespace HM
{
   class IMAPFolder;

   class IMAPCommandCREATE : public IMAPCommand
   {
      virtual IMAPResult ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);

   private:

      IMAPResult ConfirmPossibleToCreate(std::shared_ptr<HM::IMAPConnection> pConnection, const std::vector<String> &vecNewPath, bool bIsPublicFolder);
   };
}