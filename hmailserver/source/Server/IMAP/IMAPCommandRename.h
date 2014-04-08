// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


#include "IMAPCommand.h"

namespace HM
{
   class IMAPFolder;

   class IMAPCommandRENAME : public IMAPCommand
   {
      virtual IMAPResult ExecuteCommand(shared_ptr<HM::IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument);

   private:
      
      IMAPResult ConfirmPossibleToRename(shared_ptr<HM::IMAPConnection> pConnection, shared_ptr<IMAPFolder> pFolderToRename, const std::vector<String> &vecOldPath, const std::vector<String> &vecNewPath);

      shared_ptr<IMAPFolder> GetParentFolder(shared_ptr<HM::IMAPConnection> pConnection, const std::vector<String> &vecFolderPath);
   };

}

