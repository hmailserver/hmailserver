// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "messagerecipient.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MessageRecipient::MessageRecipient()  :
      result_ (ResultUndefined),
      local_account_id_(0),
      message_id_(0),
      requires_authentication_(false),
      is_local_name_(false)
   {

   }

   void
   MessageRecipient::CopyFrom(std::shared_ptr<MessageRecipient> pRecip)
   {
      address_ = pRecip->GetAddress();
      local_account_id_ = pRecip->GetLocalAccountID();
      message_id_ = pRecip->GetMessageID();
      original_address_ = pRecip->GetOriginalAddress();

      requires_authentication_ = pRecip->GetRequireAuth();
      required_sender_ = pRecip->GetRequiredSender();

      is_local_name_ = pRecip->GetIsLocalName();
   }

   MessageRecipient::~MessageRecipient(void)
   {
   }



}
