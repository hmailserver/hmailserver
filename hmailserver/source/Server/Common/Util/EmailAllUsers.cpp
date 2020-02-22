// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "EmailAllUsers.h"

#include "Time.h"

#include "../BO/Message.h"
#include "../BO/MessageData.h"
#include "../BO/MessageRecipients.h"
#include "../BO/MessageRecipient.h"
#include "../BO/Accounts.h"
#include "../BO/Account.h"
#include "../BO/Domains.h"
#include "../BO/Domain.h"
#include "../Persistence/PersistentMessage.h"
#include "../Persistence/PersistentAccount.h"
#include "../Persistence/PersistentDomain.h"

#include "../../SMTP/SMTPDeliveryManager.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   EmailAllUsers::EmailAllUsers(void)
   {
   }

   EmailAllUsers::~EmailAllUsers(void)
   {
   }

   bool 
   EmailAllUsers::Start(const String &sRecipientWildcard, const String &sFromAddress, const String &sFromName, const String &sSubject, const String &sBody)
   {
      // Create the message
      std::shared_ptr<Message> pMessage = std::shared_ptr<Message>(new Message);

      std::shared_ptr<MessageRecipients> pRecipients = pMessage->GetRecipients();

      // First read all accounts from the database.
      Accounts oAccounts(0);
      oAccounts.Refresh();

      Domains oDomains;
      oDomains.Refresh();

      std::vector<std::shared_ptr<Account> > vecAccounts = oAccounts.GetVector();
      auto iterAccount = vecAccounts.begin();
      while (iterAccount != vecAccounts.end())
      {
         std::shared_ptr<Account> pAccount = (*iterAccount);

         if (!StringParser::WildcardMatchNoCase(sRecipientWildcard, pAccount->GetAddress()))
         {
            // The account does not match wildcard.
            iterAccount++;
            continue;
         }

         if (!pAccount->GetActive())
         {
            // The account isn't active. Don't add it.
            iterAccount++;
            continue;
         }

         String sDomainName = StringParser::ExtractDomain(pAccount->GetAddress());

         // Check that the domain is active.
         std::shared_ptr<Domain> pDomain = oDomains.GetItemByName(sDomainName);

         if (!pDomain || !pDomain->GetIsActive())
         {
            // The domain isn't active. Don't add this account.
            iterAccount++;
            continue;
         }

         // Add the recipient
         std::shared_ptr<MessageRecipient> pRecipient = std::shared_ptr<MessageRecipient>(new MessageRecipient);
         pRecipient->SetLocalAccountID(pAccount->GetID());
         pRecipient->SetAddress(pAccount->GetAddress());

         pRecipients->Add(pRecipient);

         iterAccount++;
      }

      const String fileName = PersistentMessage::GetFileName(pMessage);

      // Create the actual message
      MessageData oMessageData;
      oMessageData.LoadFromMessage(fileName, pMessage);

      String sFrom;
      sFrom.Format(_T("\"%s\" <%s>"), sFromName.c_str(), sFromAddress.c_str());

      oMessageData.SetReturnPath("");
      oMessageData.SetFrom(sFrom);
      oMessageData.SetSubject(sSubject);
      oMessageData.SetBody(sBody);
      oMessageData.SetSentTime(Time::GetCurrentMimeDate());
      oMessageData.GenerateMessageID();

      oMessageData.Write(fileName);

      pMessage->SetState(Message::Delivering);

      if (!HM::PersistentMessage::SaveObject(pMessage))
         return false;

      HM::Application::Instance()->SubmitPendingEmail();

      return true;
   }
}