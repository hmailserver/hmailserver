// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"

namespace HM
{
   class IMAPCommandRangeAction : public IMAPCommand  
   {
   public:
	   IMAPCommandRangeAction();
	   virtual ~IMAPCommandRangeAction();

      void SetIsUID(bool bIsUID);
      
      IMAPResult ExecuteCommand(shared_ptr<IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument) {return IMAPResult();}
      IMAPResult DoForMails(shared_ptr<IMAPConnection> pConnection, const String &sMailNos, const shared_ptr<IMAPCommandArgument> pArgument);

   protected:

      bool GetIsUID();
      virtual IMAPResult DoAction(shared_ptr<IMAPConnection> pConnection, int messageIndex, shared_ptr<Message> pMessage, const shared_ptr<IMAPCommandArgument> pArgument) = 0;

   private:

      bool _isUID;
     
   };

}
