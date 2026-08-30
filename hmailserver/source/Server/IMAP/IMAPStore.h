// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommandRangeAction.h"

namespace HM
{

   class IMAPStore : public IMAPCommandRangeAction
   {
   public:
	   IMAPStore();
	   virtual ~IMAPStore();

      IMAPResult DoAction(std::shared_ptr<IMAPConnection> pConnection, int messageIndex, std::shared_ptr<Message> pMessage, const std::shared_ptr<IMAPCommandArgument> pArgument);

      // STORE updates the flags on the message, so it must not work on a copy.
      virtual bool UsesLiveMessages() const { return true; }

      static String GetMessageFlags(std::shared_ptr<Message> pMessage, int messageIndex);
   };

}
