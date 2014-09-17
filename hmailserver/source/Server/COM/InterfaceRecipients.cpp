// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceRecipients.h"
#include "InterfaceRecipient.h"

#include "../Common/BO/Message.h"
#include "../Common/BO/MessageRecipient.h"
#include "../Common/BO/MessageRecipients.h"


void
InterfaceRecipients::Attach(std::shared_ptr<HM::Message> pMessage)
{
   message_ = pMessage;

   
}

STDMETHODIMP 
InterfaceRecipients::get_Count(long *pVal)
{
   try
   {
      if (!message_)
         return GetAccessDenied();

      std::vector<std::shared_ptr<HM::MessageRecipient> > vecRecipients = message_->GetRecipients()->GetVector();
   
      *pVal = (int) vecRecipients.size();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceRecipients::get_Item(long Index, IInterfaceRecipient **pVal)
{
   try
   {
      if (!message_)
         return GetAccessDenied();

      CComObject<InterfaceRecipient>* pInterfaceRecipient = new CComObject<InterfaceRecipient>();
      pInterfaceRecipient->SetAuthentication(authentication_);
   
      std::vector<std::shared_ptr<HM::MessageRecipient> > vecRecipients = message_->GetRecipients()->GetVector();
   
      if (Index >= (long) vecRecipients.size())
         return DISP_E_BADINDEX;
   
      std::shared_ptr<HM::MessageRecipient> pRecipient = vecRecipients[Index];
   
      pInterfaceRecipient->AttachItem(pRecipient);
      pInterfaceRecipient->AddRef();
      *pVal = pInterfaceRecipient;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


