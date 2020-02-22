// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommandRangeAction.h"

namespace HM
{
   class IMAPConnection;

   class IMAPCommandUID : public HM::IMAPCommandRangeAction
   {
   public:
	   IMAPCommandUID();
	   virtual ~IMAPCommandUID();

      virtual IMAPResult ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);
      virtual IMAPResult DoAction(std::shared_ptr<IMAPConnection> pConnection, int messageIndex, std::shared_ptr<Message> pMessage, const std::shared_ptr<IMAPCommandArgument> pArgument) {assert(0); return IMAPResult(IMAPResult::ResultBad, "Internal parsing error.");}

   private:
      
      
      std::shared_ptr<HM::IMAPCommandRangeAction> command_;
   };

}