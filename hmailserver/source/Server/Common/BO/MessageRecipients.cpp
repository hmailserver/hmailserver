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
      _recipients.clear();
   }


   void
   MessageRecipients::Add(shared_ptr<MessageRecipient> pRecipient)
   {
      _recipients.push_back(pRecipient);
   }

   String 
   MessageRecipients::GetCommaSeperatedRecipientList()
   {
      String sRecipientList;

      boost_foreach(shared_ptr<MessageRecipient> recipient, _recipients)
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
      std::vector<shared_ptr<MessageRecipient> >::iterator iterRecipient = _recipients.begin();
      while (iterRecipient != _recipients.end())
      {
         shared_ptr<MessageRecipient> pRecipient = (*iterRecipient);

         if (pRecipient->GetLocalAccountID() == 0)
         {     
            iterRecipient = _recipients.erase(iterRecipient);
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
      std::vector<shared_ptr<MessageRecipient> >::iterator iterRecipient = _recipients.begin();
      while (iterRecipient != _recipients.end())
      {
         shared_ptr<MessageRecipient> pRecipient = (*iterRecipient);

         if (!pRecipient->GetIsLocalName())
         {     
            iterRecipient = _recipients.erase(iterRecipient);
            continue;
         }

         iterRecipient++;

      }
   }

}