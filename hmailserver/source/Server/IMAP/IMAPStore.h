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

      IMAPResult DoAction(shared_ptr<IMAPConnection> pConnection, int messageIndex, shared_ptr<Message> pMessage, const shared_ptr<IMAPCommandArgument> pArgument);
      static String GetMessageFlags(shared_ptr<Message> pMessage, int messageIndex);
   };

}
