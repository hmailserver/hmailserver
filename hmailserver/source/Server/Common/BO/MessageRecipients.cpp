// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\messagerecipients.h"
#include ".\MessageRecipient.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MessageRecipients::MessageRecipients(void)
   {
   }

   MessageRecipients::~MessageRecipients(void)
   {
   }

   void
   MessageRecipients::Clear()
   {
      recipients_.clear();
   }


   void
   MessageRecipients::Add(std::shared_ptr<MessageRecipient> pRecipient)
   {
      recipients_.push_back(pRecipient);
   }

   String 
   MessageRecipients::GetCommaSeperatedRecipientList()
   {
      String sRecipientList;

      for(std::shared_ptr<MessageRecipient> recipient : recipients_)
      {
         if (!sRecipientList.IsEmpty())
            sRecipientList += ", ";

         sRecipientList += recipient->GetAddress();
      }

      return sRecipientList;

   }

   void 
   MessageRecipients::RemoveNonLocalAccounts()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Removes all non-local recipients.
   //---------------------------------------------------------------------------()
   {
      // Remove recipients that are local, but does not exist.
      auto iterRecipient = recipients_.begin();
      while (iterRecipient != recipients_.end())
      {
         std::shared_ptr<MessageRecipient> pRecipient = (*iterRecipient);

         if (pRecipient->GetLocalAccountID() == 0)
         {     
            iterRecipient = recipients_.erase(iterRecipient);
            continue;
         }

         iterRecipient++;

      }
   }

   void 
   MessageRecipients::RemoveExternal()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Removes recipients not considered local.
   //---------------------------------------------------------------------------()
   {
      // Remove recipients that are local, but does not exist.
      auto iterRecipient = recipients_.begin();
      while (iterRecipient != recipients_.end())
      {
         std::shared_ptr<MessageRecipient> pRecipient = (*iterRecipient);

         if (!pRecipient->GetIsLocalName())
         {     
            iterRecipient = recipients_.erase(iterRecipient);
            continue;
         }

         iterRecipient++;

      }
   }

}