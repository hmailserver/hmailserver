// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "IMAPCommand.h"

namespace HM
{
   class IMAPCommandSUBSCRIBE : public IMAPCommand
   {
      virtual IMAPResult ExecuteCommand(shared_ptr<HM::IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument);
   };


}

