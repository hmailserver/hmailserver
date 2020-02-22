// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "IMAPCommand.h"

namespace HM
{
   class IMAPCommandEXPUNGE : public IMAPCommand
   {
      virtual IMAPResult ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);
   };


}

