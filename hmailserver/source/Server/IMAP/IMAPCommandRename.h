// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


#include "IMAPCommand.h"

namespace HM
{
   class IMAPFolder;

   class IMAPCommandRENAME : public IMAPCommand
   {
      virtual IMAPResult ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);

   private:
      
      IMAPResult ConfirmPossibleToRename(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPFolder> pFolderToRename, const std::vector<String> &vecOldPath, const std::vector<String> &vecNewPath);

      std::shared_ptr<IMAPFolder> GetParentFolder(std::shared_ptr<HM::IMAPConnection> pConnection, const std::vector<String> &vecFolderPath);
   };

}

