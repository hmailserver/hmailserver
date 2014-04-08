// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommandRangeAction.h"

namespace HM
{
   class IMAPCopy  : public IMAPCommandRangeAction
   {
   public:
	   IMAPCopy();

      virtual IMAPResult DoAction(shared_ptr<IMAPConnection> pConnection, int messageIndex, shared_ptr<Message> pOldMessage, const shared_ptr<IMAPCommandArgument> pArgument);

      
   };
}
