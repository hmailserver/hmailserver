// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "./MirrorMessage.h"

#include "../Common/BO/Account.h"
#include "../Common/BO/Domain.h"
#include "../Common/BO/Message.h"
#include "../common/BO/MessageRecipient.h"
#include "../common/BO/MessageRecipients.h"
#include "../Common/BO/Route.h"
#include "../Common/BO/Routes.h"
#include "../Common/BO/RouteAddresses.h"
#include "../common/Cache/CacheContainer.h"
#include "../common/Persistence/PersistentMessage.h"
#include "../Common/Util/MailerDaemonAddressDeterminer.h"

#include "RecipientParser.h"
#include "SMTPConfiguration.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{ 
   MirrorMessage::MirrorMessage(std::shared_ptr<Message> message) :
      message_(message)
   {
   }

   MirrorMessage::~MirrorMessage(void)
   {

   }

   void 
   MirrorMessage::Send()
   {
      String sMirrorAddress = Configuration::Instance()->GetMirrorAddress();

      if (sMirrorAddress.IsEmpty())
         return;

      // Do not mirror messages sent from the mirror address
      if (sMirrorAddress.CompareNoCase(message_->GetFromAddress()) == 0)
         return;

      // If message is sent from a mailer daemon address, don't mirror it.
      if (MailerDaemonAddressDeterminer::IsMailerDaemonAddress(message_->GetFromAddress()))
         return;  

      // Is the mirror address a local domain?
      String sDomain = StringParser::ExtractDomain(sMirrorAddress);
      std::shared_ptr<const Domain> pDomain = CacheContainer::Instance()->GetDomain(sDomain);      

      if (pDomain)
      {
         // The domain is local. See if the account exist.
         std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(sMirrorAddress);

         if (!pDomain->GetIsActive() || !pAccount || !pAccount->GetActive())
         {
            // check if a route exists with the same name and account.
            bool found = false;
            std::shared_ptr<Route> route = Configuration::Instance()->GetSMTPConfiguration()->GetRoutes()->GetItemByNameWithWildcardMatch(sDomain);
            if (route)
            {
               if (route->ToAllAddresses() || route->GetAddresses()->GetItemByName(sMirrorAddress))
               {
                  found = true;
               }
            }

            if (!found)
            {
               // The account didn't exist, or it wasn't active. Report a failure.
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4402, "SMTPDeliverer::_SendMirrorMessage", "Mirroring failed. The specified mirror address is local but either the domain is not enabled, or the account does not exist or is disabled.");
               return;
            }
         }
      }

      String deliveredToAddresses;

      // If the mirror address is on the recipient list, don't mirror it.
      std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients = message_->GetRecipients()->GetVector();
      auto iterRecipient = vecRecipients.begin();
      
      for(std::shared_ptr<MessageRecipient> recipipent : message_->GetRecipients()->GetVector())
      {
         if (recipipent->GetAddress().CompareNoCase(sMirrorAddress) == 0)
         {
            // The mirror address is in the recipient list. Do not mirror.
            return;
         }

         if (!deliveredToAddresses.IsEmpty())
            deliveredToAddresses+= ",";

         deliveredToAddresses += recipipent->GetOriginalAddress();
      }

      std::shared_ptr<Account> emptyAccount;
      std::shared_ptr<Message> pMsg = PersistentMessage::CopyToQueue(emptyAccount, message_);

      pMsg->SetState(Message::Delivering);

      // Add new recipients
      bool recipientOK = false;
      RecipientParser recipientParser;
      recipientParser.CreateMessageRecipientList(sMirrorAddress, pMsg->GetRecipients(), recipientOK);

      // The Delivered-To address is the original addresses of the original message rather
      // than the mirror address. The problem is that the Delivered-To address is limited to
      // 256 characters due to database limitations.
      deliveredToAddresses = deliveredToAddresses.Mid(0, 255);
      for(std::shared_ptr<MessageRecipient> recipipent : pMsg->GetRecipients()->GetVector())
      {
         recipipent->SetOriginalAddress(deliveredToAddresses);
      }

      // Save the message
      if (pMsg->GetRecipients()->GetCount() > 0)
      {
         // Save message
         PersistentMessage::SaveObject(pMsg);
      }

      // --- Tell the deliverer that a new message is pending.
      Application::Instance()->SubmitPendingEmail();
   }

}