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

      virtual IMAPResult DoAction(std::shared_ptr<IMAPConnection> pConnection, int messageIndex, std::shared_ptr<Message> pOldMessage, const std::shared_ptr<IMAPCommandArgument> pArgument);

      // COPY is atomic (RFC 3501 6.4.7), so nothing is copied unless every message exists.
      virtual MissingMessagePolicy GetMissingMessagePolicy() const { return MissingMessagePolicy::FailBeforeActing; }

      
   };
}
