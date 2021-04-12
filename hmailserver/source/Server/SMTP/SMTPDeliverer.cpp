// Copyright (c) 2006 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "SMTPDeliverer.h"

#include "../common/Application/ObjectCache.h"

#include "../common/AntiVirus/AntiVIrusCOnfiguration.h"
#include "../common/AntiVirus/VirusScanner.h"

#include "../common/BO/MessageRecipient.h"
#include "../common/BO/MessageRecipients.h"
#include "../common/BO/MessageData.h"
#include "../common/Cache/CacheContainer.h"
#include "../common/Cache/InboxIDCache.h"
#include "../common/Util/Time.h"
#include "../common/Util/ServerStatus.h"
#include "../common/Util/MailerDaemonAddressDeterminer.h"
#include "../common/TCPIP/TCPConnection.h"

#include "../common/Util/Utilities.h"
#include "../common/Util/MessageAttachmentStripper.h"
#include "../common/Util/MessageUtilities.h"

#include "../common/Scripting/Events.h"

#include "../common/Persistence/PersistentMessage.h"
#include "../common/Persistence/PersistentIMAPFolder.h"
#include "../common/Persistence/PersistentAccount.h"
#include "../common/Persistence/PersistentMessageRecipient.h"
#include "../common/Persistence/PersistentServerMessage.h"

#include "../common/BO/IMAPFolder.h"
#include "../common/BO/Domain.h"
#include "../common/BO/Account.h"
#include "../common/BO/ServerMessages.h"

#include "LocalDelivery.h"
#include "ExternalDelivery.h"
#include "SMTPConfiguration.h"
#include "SMTPVirusNotifier.h"
#include "RuleApplier.h"
#include "RuleResult.h"
#include "RecipientParser.h"
#include "MirrorMessage.h"

#include "../Common/Util/AWstats.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SMTPDeliverer::SMTPDeliverer()
   {
      
   }

   SMTPDeliverer::~SMTPDeliverer()
   {
   }

   void
   SMTPDeliverer::DeliverMessage(std::shared_ptr<Message> pMessage)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Submits the next message in the database. 
   //---------------------------------------------------------------------------()
   {
      LOG_DEBUG("Delivering message...");

      if (!pMessage)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4217, "SMTPDeliverer::DeliverMessage()", "The message to deliver was not given.");
         return;
      }

      // Fetch the message ID before we delete the message and it's reset.
      __int64 messageID = pMessage->GetID();

      String sSendersIP;
      RuleResult globalRuleResult;
      if (!PreprocessMessage_(pMessage, sSendersIP, globalRuleResult))
      {
         // Message delivery was aborted during preprocessing.
         return;
      }

      std::vector<String> saErrorMessages;

      // Perform deliver to local recipients.
      LocalDelivery localDeliverer(sSendersIP, pMessage, globalRuleResult);
      bool messageReused = localDeliverer.Perform(saErrorMessages);

      bool messageRescheduled = false;
      if (pMessage->GetRecipients()->GetCount() > 0)
      {
         // Perform deliveries to external recipients.
         ExternalDelivery externalDeliverer(sSendersIP, pMessage, globalRuleResult);
         messageRescheduled = externalDeliverer.Perform(saErrorMessages);
      }

      // If an error has occurred, now is the time to send an error
      // message back to the author of the email message.
      if (saErrorMessages.size() > 0)
         SubmitErrorLog_(pMessage, saErrorMessages);

      // Unless the message has been re-used, or has been rescheduled for
      // later delivery, we should delete it now.
      bool deleteMessageNow = !messageReused && !messageRescheduled;
      if (deleteMessageNow)
      {
         // Check that we haven't reused this message before we delete it.
         if (pMessage->GetAccountID() > 0)
         {
            String errorMessage;
            errorMessage.Format(_T("Attempting to delete message %I64d even though it's been delivered to user account %I64d ."), messageID, pMessage->GetAccountID());
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5208, "SMTPDeliverer::DeliverMessage", errorMessage);
            return;
         }

         PersistentMessage::DeleteObject(pMessage);   
      }

      String logText;
      logText.Format(_T("SMTPDeliverer - Message %I64d: Message delivery thread completed."), messageID);
      LOG_APPLICATION(logText);

      return;   
   }

   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Performs preprocessing of the message. Checks that the file exists, runs
   // global rules, virus scanning and mirroring.
   //
   // Returns true if delivery should continue. False if it should be aborted.
   //---------------------------------------------------------------------------()
   bool
   SMTPDeliverer::PreprocessMessage_(std::shared_ptr<Message> pMessage, String &sendersIP, RuleResult &globalRuleResult)
   {
      // Before we do anything else, check that the message file exists. If the file
      // does not exist, there is nothing to deliver. So if the file does not exist,
      // delete pMessage from the database and report in the application log.
      const String messageFileName = PersistentMessage::GetFileName(pMessage);
      if (!FileUtilities::Exists(messageFileName))
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5006, "SMTPDeliverer::DeliverMessage()", "Message " + StringParser::IntToString(pMessage->GetID()) + " could not be delivered since the data file does not exist.");
         PersistentMessage::DeleteObject(pMessage);

         return false;
      }

      if (pMessage->GetRecipients()->GetCount() == 0)
      {
         // No remaining recipients.
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5007, "SMTPDeliverer::DeliverMessage()", "Message " + StringParser::IntToString(pMessage->GetID()) + " could not be delivered. No remaining recipients. File: " + messageFileName);

         PersistentMessage::DeleteObject(pMessage);
         return false;
      }

      sendersIP = MessageUtilities::GetSendersIP(pMessage);

      // Create recipient list.
      String sRecipientList = pMessage->GetRecipients()->GetCommaSeperatedRecipientList();

      String log_from_address = pMessage->GetFromAddress().IsEmpty() ? "<Empty>" : pMessage->GetFromAddress();

      String sMessage = Formatter::Format("SMTPDeliverer - Message {0}: Delivering message from {1} to {2}. File: {3}",
                                                pMessage->GetID(), log_from_address, sRecipientList, messageFileName);

      LOG_APPLICATION(sMessage);

      // Run the first event in the delivery chain
      if (!Events::FireOnDeliveryStart(pMessage))
      {
         LogAwstatsMessageRejected_(sendersIP, pMessage, "Delivery cancelled by OnDeliveryStart-event");
         return false;
      }

      if (pMessage->GetRecipients()->GetCount() == 0)
      {
         std::vector<String> saErrorMessages;
         SubmitErrorLog_(pMessage, saErrorMessages);
         PersistentMessage::DeleteObject(pMessage);
         return false;
      }

      // Run virus protection.
      if (!RunVirusProtection_(pMessage))
      {
         //LogAwstatsMessageRejected_ moved to HandleInfectedMessage_()
         //LogAwstatsMessageRejected_(sendersIP, pMessage, "Message delivery cancelled during virus scanning");
         return false;
      }

      // Apply rules on this message.
      if (!RunGlobalRules_(pMessage, globalRuleResult))
      {
         //LogAwstatsMessageRejected_ moved to RunGlobalRules_()
         //LogAwstatsMessageRejected_(sendersIP, pMessage, "Message delivery cancelled during global rules");
         return false;
      }

      // Run the OnDeliverMessage-event
      if (!Events::FireOnDeliverMessage(pMessage))
      {
         LogAwstatsMessageRejected_(sendersIP, pMessage, "Message delivery cancelled during OnDeliverMessage-event");
         return false;
      }

      // Run mirroring
      if (pMessage->GetNoOfRetries() == 0)
      {
         MirrorMessage mirrorer(pMessage);
         mirrorer.Send();
      }

      return true;

   }


   void
   SMTPDeliverer::SubmitErrorLog_(std::shared_ptr<Message> pOrigMessage, std::vector<String> &saErrorMessages)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Some of the messages was not sent to the recipient. We have to
   // send a mail back to the sender to tell him this.
   //---------------------------------------------------------------------------()
   {
      LOG_DEBUG("SD::SubmitErrorLog_");      
      if (MailerDaemonAddressDeterminer::IsMailerDaemonAddress(pOrigMessage->GetFromAddress()))
      {
         LOG_DEBUG("SD::~SubmitErrorLog_");      
         return; // Avoid bounce-bounce
      }

      if (pOrigMessage->GetFromAddress().IsEmpty())
      {
         // No sender address specified, hence no place to
         // deliver the error log.
         return;
      }

      const String fileName = PersistentMessage::GetFileName(pOrigMessage);

      std::shared_ptr<MessageData> pMsgData = std::shared_ptr<MessageData> (new MessageData());
      pMsgData->LoadFromMessage(fileName, pOrigMessage);

      // true because we don't want to send bounces if AutoSubmitted header
      if (!RuleApplier::IsGeneratedResponseAllowed(pMsgData, true))
      {
         String sMessage;
         sMessage.Format(_T("Did not submit bounce message for message %I64d from %s since rule loop count was reached or Auto-Submitted header."), pOrigMessage->GetID(), String(pOrigMessage->GetFromAddress()).c_str());
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4404, "SMTPDeliverer::_ApplyForwarding", sMessage);

         return;
      }

      int iRuleLoopCount = pMsgData->GetRuleLoopCount();

      String sMailerDaemonAddress = MailerDaemonAddressDeterminer::GetMailerDaemonAddress(pOrigMessage);
      
      String sMessageUndeliverable;
      sMessageUndeliverable.Format(_T("%s"), Configuration::Instance()->GetServerMessages()->GetMessage("MESSAGE_UNDELIVERABLE").c_str());
      String sErrMsg = Configuration::Instance()->GetServerMessages()->GetMessage("SEND_FAILED_NOTIFICATION");

      sErrMsg.Replace(_T("%MACRO_SENT%"), pMsgData->GetSentTime());
      sErrMsg.Replace(_T("%MACRO_SUBJECT%"), pMsgData->GetSubject());

      // Ability to include original headers in undeliverable message
      // http://www.hmailserver.com/forum/viewtopic.php?f=2&t=19635
      sErrMsg.Replace(_T("%MACRO_ORIGINAL_HEADER%"), pMsgData->GetHeader());      

      String sCollectedErrors;
      for (unsigned int i=0;i<saErrorMessages.size();i++)
         sCollectedErrors += saErrorMessages[i];

      sErrMsg.Replace(_T("%MACRO_RECIPIENTS%"), sCollectedErrors);

      // Send a copy of this email.
      std::shared_ptr<Message> pMsg = std::shared_ptr<Message>(new Message());
      pMsg->SetState(Message::Delivering);
      pMsg->SetFromAddress("");

      const String newFileName = PersistentMessage::GetFileName(pMsg);

      std::shared_ptr<MessageData> pNewMsgData = std::shared_ptr<MessageData>(new MessageData());
      pNewMsgData->LoadFromMessage(newFileName, pMsg);

      // Required headers
      pNewMsgData->SetReturnPath("");
      pNewMsgData->GenerateMessageID();
      pNewMsgData->SetSentTime(Time::GetCurrentMimeDate());

      // Optional headers
      pNewMsgData->SetFrom(sMailerDaemonAddress);
      pNewMsgData->SetTo(pOrigMessage->GetFromAddress());
      pNewMsgData->SetSubject(sMessageUndeliverable + ": " + pMsgData->GetSubject());
      pNewMsgData->SetBody(sErrMsg);
      pNewMsgData->SetRuleLoopCount(iRuleLoopCount + 1);

      // Write message data
      pNewMsgData->Write(PersistentMessage::GetFileName(pMsg));

      // Add recipients
      bool recipientOK = false;
      RecipientParser recipientParser;
      recipientParser.CreateMessageRecipientList(pOrigMessage->GetFromAddress(), pMsg->GetRecipients(), recipientOK);

      if (pMsg->GetRecipients()->GetCount() > 0)
         // Save message
         PersistentMessage::SaveObject(pMsg);

      LOG_DEBUG("SD::~SubmitErrorLog_"); 

   }

   
   bool
   SMTPDeliverer::HandleInfectedMessage_(std::shared_ptr<Message> pMessage, const String &virusName)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Called if a virus is found in an email message. Checks in the configuration
   // what we should do with infected messages and does just that.
   //---------------------------------------------------------------------------()
   {
      AntiVirusConfiguration &antiVirusConfig = Configuration::Instance()->GetAntiVirusConfiguration();

      if (antiVirusConfig.AVAction() == AntiVirusConfiguration::ActionDelete)
      {
         // The message should be deleted.
         String sendersIP = MessageUtilities::GetSendersIP(pMessage);
         LogAwstatsMessageRejected_(sendersIP, pMessage, "Message delivery cancelled during virus scanning");

         // Should we notify the recipient?
         if (antiVirusConfig.AVNotifyReceiver())
         {
            // Notify every receiver of the email.
            std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients = pMessage->GetRecipients()->GetVector();
            auto iterRecipient = vecRecipients.begin();

            while (iterRecipient != vecRecipients.end())
            {
               SMTPVirusNotifier::CreateMessageDeletedNotification(pMessage, (*iterRecipient)->GetAddress());
               
               iterRecipient++;
            }
         }

         if (antiVirusConfig.AVNotifySender())
            SMTPVirusNotifier::CreateMessageDeletedNotification(pMessage, pMessage->GetFromAddress());

         String logMessage = Formatter::Format("SMTPDeliverer - Message {0}: Message deleted (contained virus {1}).", 
            pMessage->GetID(), virusName);

         LOG_APPLICATION(logMessage);

         PersistentMessage::DeleteObject(pMessage);
         
         return false; // do not continue delivery

      }
      else if (antiVirusConfig.AVAction() == AntiVirusConfiguration::ActionStripAttachments)
      {
         std::shared_ptr<MessageAttachmentStripper> pStripper = std::shared_ptr<MessageAttachmentStripper>(new MessageAttachmentStripper());

         pStripper->Strip(pMessage);

         String logMessage = Formatter::Format("SMTPDeliverer - Message {0}: Message attachments stripped (contained virus {1}).", 
            pMessage->GetID(), virusName);

         LOG_APPLICATION(logMessage);

         return true; // continue delivery
      }

      // Weird configuration?
      assert(0);
      
      return false;
   }

   bool 
   SMTPDeliverer::RunVirusProtection_(std::shared_ptr<Message> pMessage)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Runs virus scanning. If a virus is found, it's handled according to the
   // configuration. (Either the attachments are stripped, or the entire message
   // is deleted.
   //---------------------------------------------------------------------------()
   {

      // First check for blocked attachments.
      if (Configuration::Instance()->GetAntiVirusConfiguration().GetEnableAttachmentBlocking())      
      {
         VirusScanner::BlockAttachments(pMessage);
      }

      if (!pMessage->GetFlagVirusScan())
         return true;

      if (!VirusScanner::GetVirusScanningEnabled())
         return true;

      // Virus scanning
      String virusName;
      if (VirusScanner::Scan(pMessage, virusName))
      {
         // Virus found.
         ServerStatus::Instance()->OnVirusRemoved();

         if (!HandleInfectedMessage_(pMessage, virusName))
            return false;
      }      

      return true;

   }

   bool 
   SMTPDeliverer::RunGlobalRules_(std::shared_ptr<Message> pMessage, RuleResult &ruleResult)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Performs global rules. We will do this on every retry of the message.
   //---------------------------------------------------------------------------()
   {
      std::shared_ptr<RuleApplier> pRuleApplier = std::shared_ptr<RuleApplier>(new RuleApplier);
      std::shared_ptr<Account> emptyAccount;
      pRuleApplier->ApplyRules(ObjectCache::Instance()->GetGlobalRules(), emptyAccount, pMessage, ruleResult);

      if (ruleResult.GetDeleteEmail())
      {
         String sendersIP = MessageUtilities::GetSendersIP(pMessage);
         LogAwstatsMessageRejected_(sendersIP, pMessage, "Message delivery cancelled during global rules");

         String sDeleteRuleName = ruleResult.GetDeleteRuleName();

         String sMessage;
         sMessage.Format(_T("SMTPDeliverer - Message %I64d: ")
            _T("Message deleted. Action was taken by a global rule (%s). "),
            pMessage->GetID(), 
            sDeleteRuleName.c_str());

         LOG_APPLICATION(sMessage);

         PersistentMessage::DeleteObject(pMessage);
         return false;
      }

      return true;
   }

   void 
   SMTPDeliverer::LogAwstatsMessageRejected_(const String &sSendersIP, std::shared_ptr<Message> pMessage, const String &sReason)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // If awstats logging is enabled, this function goes through all the recipients
   // of the message, and logs to the awstats log that they have been rejected.
   // This is used if a message is rejected after it has been transferred from the
   // client to the server.
   //---------------------------------------------------------------------------()
   {
      // Check that message exists
      if (!pMessage)
         return;

      // Go through the recipients and log one row for each of them.
      String sFromAddress = pMessage->GetFromAddress();     

      const std::vector<std::shared_ptr<MessageRecipient> > vecRecipients = pMessage->GetRecipients()->GetVector();
      std::vector<std::shared_ptr<MessageRecipient> >::const_iterator iterRecipient = vecRecipients.begin();
      while (iterRecipient != vecRecipients.end())
      {
         String sRecipientAddress = (*iterRecipient)->GetAddress();

         // Log the error message
         AWStats::LogDeliveryFailure(sSendersIP, pMessage->GetFromAddress(), sRecipientAddress,  550);
         Events::FireOnDeliveryFailed(pMessage, sSendersIP, sRecipientAddress, sReason);

         iterRecipient++;
      }

   }


}


