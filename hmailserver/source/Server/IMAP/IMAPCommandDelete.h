// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"

namespace HM
{
   class IMAPFolder;

   class IMAPCommandDELETE : public IMAPCommand
   {
      virtual IMAPResult ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);

   private:

      void RemoveFolder_( std::shared_ptr<IMAPFolder> pFolder, std::shared_ptr<HM::IMAPConnection>  pConnection );
   };
}