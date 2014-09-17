// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"

namespace HM
{
   
   class IMAPCommandLIST  : public IMAPCommand
   {
   public:
	   IMAPCommandLIST();
	   virtual ~IMAPCommandLIST();

      virtual IMAPResult ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);
   };

}
