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
      m_drResult (ResultUndefined),
      m_iLocalAccountID(0),
      m_iMessageID(0),
      m_bRequiresAuthentication(false),
      _isLocalName(false)
   {

   }

   void
   MessageRecipient::CopyFrom(shared_ptr<MessageRecipient> pRecip)
   {
      m_sAddress = pRecip->GetAddress();
      m_iLocalAccountID = pRecip->GetLocalAccountID();
      m_iMessageID = pRecip->GetMessageID();
      m_sOriginalAddress = pRecip->GetOriginalAddress();

      m_bRequiresAuthentication = pRecip->GetRequireAuth();
      m_sRequiredSender = pRecip->GetRequiredSender();

      _isLocalName = pRecip->GetIsLocalName();
   }

   MessageRecipient::~MessageRecipient(void)
   {
   }



}
