// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "./LocalDelivery.h"

#include "../common/Application/ObjectCache.h"

#include "../common/BO/Account.h"
#include "../common/BO/Message.h"
#include "../common/BO/MessageRecipient.h"
#include "../common/BO/MessageRecipients.h"

#include "../common/Cache/CacheContainer.h"
#include "../common/Cache/AccountSizeCache.h"

#include "../common/Persistence/PersistentMessageRecipient.h"
#include "../common/Persistence/PersistentMessage.h"
#include "../common/Persistence/PersistentAccount.h"

#include "../common/Tracking/ChangeNotification.h"
#include "../common/Tracking/NotificationServer.h"

#include "../Common/Util/AWstats.h"
#include "../common/Util/TraceHeaderWriter.h"
#include "../common/Util/MessageUtilities.h"

#include "../IMAP/MessagesContainer.h"

#include "SMTPConfiguration.h"
#include "SMTPVacationMessageCreator.h"
#include "SMTPForwarding.h"
#include "RuleApplier.h"
#include "RuleResult.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{ 
   LocalDelivery::LocalDelivery(const String &sSendersIP, std::shared_ptr<Message> message, const RuleResult &globalRuleResult) :
      _sendersIP(sSendersIP),
      original_message_(message),
      _globalRuleResult(globalRuleResult)
   {
   }

   LocalDelivery::~LocalDelivery(void)
   {

   }

   /// Delivers the message to local recipients (recipients that exists in this installation)
   /// Returns true if the message has been re-used by us. If it has, the deliverer outside
   /// should not delete the message from the database when we're done.
   bool
   LocalDelivery::Perform(std::vector<String> &saErrorMessages)
   {
      LOG_DEBUG("Performing local delivery");

      bool messageReused = false;

      // NOTE: Since were manipulating the messages recipient vector below, we want to do a copy.
      //       We should iterate over the copy of recipients here. Not over the original list.
      std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients = original_message_->GetRecipients()->GetVector();
      auto iterRecipient = vecRecipients.begin();

      while (iterRecipient != vecRecipients.end())
      {
         std::shared_ptr<MessageRecipient> pRecipient = (*iterRecipient);

         if (pRecipient->GetLocalAccountID() == 0)
         {
            // this is an external recipient.
            iterRecipient++;
            continue;
         }

         // Read the recipients account from database.
         std::shared_ptr<const Account> pCheckAccount = CacheContainer::Instance()->GetAccount(pRecipient->GetLocalAccountID());

         if (pCheckAccount)
         {
            // Local recipient has been found. Deliver to it.
            DeliverToLocalAccount_(pCheckAccount, vecRecipients.size(), saErrorMessages, pRecipient->GetOriginalAddress(), messageReused);
         }
         else
         {
            // Local recipient but we could not find it. It has probably been deleted 
            // after we accepted the message.
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5165, "LocalDelivery::DeliverToLocalAccount_s", "The recipient account appears to have been deleted after the message was received. Aborting delivery.");
         }

         // Delete this recipient from the database and memory.
         PersistentMessageRecipient::DeleteObject(pRecipient);

         iterRecipient = vecRecipients.erase(iterRecipient);
      }

      LOG_DEBUG("Local delivery completed");

      return messageReused;
   }


   /// Delivers a single message to a specific account.
   /// Returns true if the delivery was made, false otherwise.
   void
   LocalDelivery::DeliverToLocalAccount_(std::shared_ptr<const Account> account, size_t iNoOfRecipients, std::vector<String> &saErrorMessages, const String &sOriginalAddress, bool &messageReused)
   {
      // First check that we're actually able to deliver a message to this account. If the account
      // has reached it's quota, we should cancel delivery immediately. If we create the account-level
      // message below, there's no turning back.
      if (!CheckAccountQuotas_(account, saErrorMessages))
      {
         return;
      }

      // We should reuse the message file if only one recipient
      // exists. Reusing message file is good for performance since
      // we don't have to create a new file on disk.
      messageReused = iNoOfRecipients == 1;

      std::shared_ptr<Message> accountLevelMessage = CreateAccountLevelMessage_(original_message_, account, messageReused, sOriginalAddress);
      if (!accountLevelMessage)
      {
         String errorMessage;
         errorMessage.Format(_T("Unable to create account-level message of %I64d for account %s."), original_message_->GetID(), String(account->GetAddress()).c_str());

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5209, "SMTPDeliverer::DeliverToLocalAccount_", errorMessage);
         return;
      }

      if (!LocalDeliveryPreProcess_(account, accountLevelMessage, sOriginalAddress, saErrorMessages))
      {
         FileUtilities::DeleteFile(PersistentMessage::GetFileName(account, accountLevelMessage));

         if (messageReused)
         {
            accountLevelMessage->SetAccountID(0);
            messageReused = false;
         }

         return;
      }

      // We don't want to set the message state to Delivered until just before we save it. If we set
      // it earlier, rules and scripts may assume that the message has been delivered and make
      // changes to it and save those to the database. For instance, if there's an account-level rule executing and this rule
      // executes a script which saves the message, only the file changes should be saved. We should
      // not add any rows to the database in this scenario. To ensure this, InterfaceMessage checks the
      // message state. If it's "Delivering", it won't save the changes to the database.
      accountLevelMessage->SetState(Message::Delivered); 

      PersistentMessage::SaveObject(accountLevelMessage);

      // Tell the folder container that the users inbox is updated this will 
      // cause a refresh in the imap server whenever a new imap command is sent.
      MessagesContainer::Instance()->SetFolderNeedsRefresh(accountLevelMessage->GetFolderID());

      // Notify the mailbox notifier that the mailbox contents have changed.
      std::shared_ptr<ChangeNotification> changeNotification = 
         std::shared_ptr<ChangeNotification>(new ChangeNotification(accountLevelMessage->GetAccountID(), accountLevelMessage->GetFolderID(), ChangeNotification::NotificationMessageAdded));
      Application::Instance()->GetNotificationServer()->SendNotification(changeNotification);

      AWStats::LogDeliverySuccess(_sendersIP, "127.0.0.1", accountLevelMessage, account->GetAddress());
   }

   /*
   Performs preprocessing for local delivery. 
   Rules, forwarding and quota is taken care of now.

   Returns true if message should be delivered, false if it should be aborted.
   */
   bool 
   LocalDelivery::LocalDeliveryPreProcess_(std::shared_ptr<const Account> account, std::shared_ptr<Message> accountLevelMessage, const String &sOriginalAddress, std::vector<String> &saErrorMessages)
   {
      SendAutoReplyMessage_(account, original_message_);

      // We must run account level rules after the message file has been
      // moved to the destination folder, so that any changes it has only
      // affects this instance.
      RuleResult accountRuleResult;
      if (!RunAccountRules_(account, accountLevelMessage, accountRuleResult))
         return false;

      SMTPForwarding forwarder;

      if (!forwarder.PerformForwarding(account, accountLevelMessage))
      {
         String sMessage = Formatter::Format("SMTPDeliverer - Message {0}: The message was not delivered to {1} because a forward was set up for the account.",
                                                original_message_->GetID(), account->GetAddress());

         return false;
      }

      // Do the final delivery of the message.
      AddTraceHeaders_(account, accountLevelMessage, sOriginalAddress);

      //
      // Move to IMAP folder. This must be done after we've executed account level rules
      // since the account level rules may override the folders. 
      //
      __int64 iDestinationFolderID = accountLevelMessage->GetFolderID();
      __int64 iDestinationAccountID = account->GetID();

      bool bSetByGlobalRule = true;
      String sIMAPFolder = _globalRuleResult.GetMoveToFolder();
      if (!accountRuleResult.GetMoveToFolder().IsEmpty())
      {
         sIMAPFolder = accountRuleResult.GetMoveToFolder();
         bSetByGlobalRule = false;
      }

      if (!sIMAPFolder.IsEmpty())
         MessageUtilities::MoveToIMAPFolder(accountLevelMessage, account->GetID(), sIMAPFolder, false, bSetByGlobalRule, iDestinationAccountID, iDestinationFolderID);

      if (iDestinationAccountID == 0)
      {
         // This message should be moved to the #public folder structure.
         const String currentPath = PersistentMessage::GetFileName(account, accountLevelMessage);
         PersistentMessage::MoveFileToPublicFolder(currentPath, accountLevelMessage);

         // Since the owner of the message has changed, we'll have to update
         // the owner on the message. Makes sense, right?
         accountLevelMessage->SetAccountID(0);
      }

      return true;
   }

   bool 
   LocalDelivery::CheckAccountQuotas_(std::shared_ptr<const Account> pCheckAccount, std::vector<String> &saErrorMessages)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Checks that the recipient account has enough space available. If not, 
   // an error message is generated.
   //---------------------------------------------------------------------------()
   {
      // Check if message is small enough to fit inside receivers mailbox.
      // All values are converted to bytes.
      if (!pCheckAccount->SpaceAvailable(original_message_->GetSize()))
      {
         String sMsg = Formatter::Format("{0}\r\n   Error Type: SMTP\r\n   Error Description: Inbox is full\r\n   Additional information: The recipients inbox is full.\r\n\r\n",
            pCheckAccount->GetAddress());
         saErrorMessages.push_back(sMsg);  

         __int64 currentSize = AccountSizeCache::Instance()->GetSize(pCheckAccount->GetID()) + original_message_->GetSize();

         String sMessage;
         sMessage.Format(_T("SMTPDeliverer - Message %I64d: The message was not delivered to %s. ")
            _T("Delivery to this account was cancelled since the account inbox is full. Max size: %d MB, Current size (including cancelled message): %d MB"), 
            original_message_->GetID(), String(pCheckAccount->GetAddress()).c_str(), pCheckAccount->GetAccountMaxSize(), (currentSize / 1024 / 1024));

         LOG_APPLICATION(sMessage);

         return false;
      }

      return true;
   }

   std::shared_ptr<Message> 
   LocalDelivery::CreateAccountLevelMessage_(std::shared_ptr<Message> pOriginalMessage, std::shared_ptr<const Account> pRecipientAccount, bool reuseMessage, const String &sOriginalAddress)
   {
      // Copy the original message to the new message. Also copy the message
      // file unless we should reuse the old one.
      std::shared_ptr<Message> pNewMessage;

      if (reuseMessage)
      {
         const String sourceLocation = PersistentMessage::GetFileName(pOriginalMessage);

         pNewMessage = pOriginalMessage;
         pNewMessage->SetAccountID(pRecipientAccount->GetID());

         // Locate the inbox for this user.
         __int64  inboxID = CacheContainer::Instance()->GetInboxIDCache().GetUserInboxFolder(pRecipientAccount->GetID());
         if (inboxID == 0)
         {
            std::shared_ptr<Message> empty;
            return empty;
         }

         pNewMessage->SetFolderID(inboxID);

         if (!PersistentMessage::MoveFileToUserFolder(sourceLocation, pNewMessage, pRecipientAccount))
         {
            std::shared_ptr<Message> empty;
            return empty;
         }
      }
      else
      {
         pNewMessage = PersistentMessage::CopyFromQueueToInbox(pOriginalMessage, pRecipientAccount);

         for (std::shared_ptr<MessageRecipient> recipient : pOriginalMessage->GetRecipients()->GetVector())
         {
            if (recipient->GetAddress().CompareNoCase(pRecipientAccount->GetAddress()) == 0)
            {
               pNewMessage->GetRecipients()->Add(recipient);
               break;
            }
         }
      }

      pNewMessage->SetNoOfRetries(pOriginalMessage->GetNoOfRetries());

      return pNewMessage;
   }

   bool 
   LocalDelivery::AddTraceHeaders_(std::shared_ptr<const Account> account, std::shared_ptr<Message> pMessage, const String &sOriginalAddress)
   {
      std::vector<std::pair<AnsiString, AnsiString> > fieldsToWrite;

      fieldsToWrite.push_back(std::make_pair("Return-Path", pMessage->GetFromAddress()));

      if (Configuration::Instance()->GetSMTPConfiguration()->GetAddDeliveredToHeader())
         fieldsToWrite.push_back(std::make_pair("Delivered-To", sOriginalAddress));

      const String fileName = PersistentMessage::GetFileName(account, pMessage);

      TraceHeaderWriter writer;
      return writer.Write(fileName, pMessage, fieldsToWrite);
   }    

   void 
   LocalDelivery::SendAutoReplyMessage_(std::shared_ptr<const Account> pAccount, std::shared_ptr<Message> pMessage)
   {
      // Do this before we move the message to the users
      // directory. If we do it afterwards, the user may have (at least theoretically)
      // have the time to delete the message before the auto-reply is sent. And we need
      // the message to be able to generate a Re:-subject line.

      // Should we send a vacation message back?
      if (!PersistentAccount::GetIsVacationMessageOn(pAccount))
         return;

      // Don't deliver vacation message to ourselvs.
      if (pAccount->GetAddress().CompareNoCase(pMessage->GetFromAddress()) == 0)
         return;

      // Save a new message with the vacation message in it.
      SMTPVacationMessageCreator::Instance()->CreateVacationMessage(pAccount, 
         pMessage->GetFromAddress(), 
         pAccount->GetVacationSubject(), 
         pAccount->GetVacationMessage(),
         pMessage
         );
   }      

   bool 
   LocalDelivery::RunAccountRules_(std::shared_ptr<const Account> pAccount, std::shared_ptr<Message> pMessage, RuleResult &accountRuleResult)
   {
      // Apply rules on this message.  
      std::shared_ptr<RuleApplier> pRuleApplier = std::shared_ptr<RuleApplier>(new RuleApplier);

      pRuleApplier->ApplyRules(ObjectCache::Instance()->GetAccountRules(pAccount->GetID()), pAccount, pMessage, accountRuleResult);

      if (accountRuleResult.GetDeleteEmail())
      {
         String sDeleteRuleName = accountRuleResult.GetDeleteRuleName();

         String sMessage = Formatter::Format("SMTPDeliverer - Message {0}: The message was not delivered to {1}. Delivery to this account was canceled by an account rule {2}.",
                                                   pMessage->GetID(), pAccount->GetAddress(), sDeleteRuleName);

         LOG_APPLICATION(sMessage);
         return false;
      }

      return true;
   }




}