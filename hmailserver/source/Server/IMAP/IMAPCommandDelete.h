// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"

namespace HM
{
   class IMAPFolder;

   class IMAPCommandDELETE : public IMAPCommand
   {
      virtual IMAPResult ExecuteCommand(shared_ptr<HM::IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument);

   private:

      void _RemoveFolder( shared_ptr<IMAPFolder> pFolder, shared_ptr<HM::IMAPConnection>  pConnection );
   };
}