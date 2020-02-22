// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"

namespace HM
{
   class IMAPConnection;

   class IMAPCommandStore : public IMAPCommand  
   {
   public:
      IMAPCommandStore();
      virtual ~IMAPCommandStore();

      virtual IMAPResult ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);

   };

}
