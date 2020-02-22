// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "PersistentMessageRecipient.h"


#include "../BO/MessageRecipient.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentMessageRecipient::PersistentMessageRecipient(void)
   {
   }

   PersistentMessageRecipient::~PersistentMessageRecipient(void)
   {
   }


   bool 
   PersistentMessageRecipient::DeleteObject(std::shared_ptr<MessageRecipient> pRecipient)
   {
      SQLCommand command("delete from hm_messagerecipients where recipientaddress = @ADDRESS AND recipientmessageid = @MESSAGEID");
      command.AddParameter("@ADDRESS", pRecipient->GetAddress());
      command.AddParameter("@MESSAGEID", pRecipient->GetMessageID());

      return Application::Instance()->GetDBManager()->Execute(command);

   }
}