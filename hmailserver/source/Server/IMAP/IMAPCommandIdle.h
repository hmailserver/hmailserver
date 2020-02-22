// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"

namespace HM
{
   class IMAPCommandIdle : public IMAPCommand
   {
   public:
	   IMAPCommandIdle(std::shared_ptr<IMAPConnection> pConnection);
	   virtual ~IMAPCommandIdle();

      IMAPResult ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);
      void Finish(bool sendNotificationToClient);

   private:

      String tag_;

      std::weak_ptr<IMAPConnection> connection_;
   };

}
