// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"

namespace HM
{
   class IMAPCommandIdle : public IMAPCommand
   {
   public:
	   IMAPCommandIdle(shared_ptr<IMAPConnection> pConnection);
	   virtual ~IMAPCommandIdle();

      IMAPResult ExecuteCommand(shared_ptr<IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument);
      void Finish(bool sendNotificationToClient);

   private:

      String m_sTag;

      weak_ptr<IMAPConnection> _connection;
   };

}
