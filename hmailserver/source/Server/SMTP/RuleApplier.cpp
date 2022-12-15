// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "./RuleApplier.h"

#include "RuleResult.h"

#include "../Common/BO/Rule.h"
#include "../Common/BO/Rules.h"
#include "../Common/BO/RuleCriterias.h"
#include "../Common/BO/RuleActions.h"
#include "../Common/BO/RuleAction.h"
#include "../Common/BO/MessageData.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/Account.h"
#include "../Common/BO/MessageRecipients.h"
#include "../Common/Cache/CacheContainer.h"
#include "../Common/Util/Time.h"
#include "../Common/Util/RegularExpression.h"
#include "../common/Util/MailerDaemonAddressDeterminer.h"

#include "../Common/Persistence/PersistentMessage.h"

#include "RecipientParser.h"

#include "SMTPConfiguration.h"

#include "../Common/Scripting/ScriptServer.h"
#include "../Common/Scripting/ScriptObjectContainer.h"
#include "../Common/Scripting/Result.h"

#include "../Common/BO/MessageRecipient.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   RuleApplier::RuleApplier(void) :
      rule_account_id_(0)
   {
   }

   RuleApplier::~RuleApplier(void)
   {
   }

   void
   RuleApplier::ApplyRules(std::shared_ptr<Rules> pRules, std::shared_ptr<const Account> account, std::shared_ptr<Message> pMessage, RuleResult &ruleResult)
   {
      LOG_DEBUG(_T("Applying rules"));

      if (pRules->GetCount() == 0)
      {
         return;
      }

      std::shared_ptr<MessageData> pMessageData = std::shared_ptr<MessageData>(new MessageData());
      pMessageData->LoadFromMessage(account, pMessage);
      
      rule_account_id_ = pRules->GetAccountID();

      for (int i = 0; i < pRules->GetCount(); i++)
      {
         std::shared_ptr<Rule> pRule = pRules->GetItem(i);

         if (pRule && pRule->GetActive())
         {
            bool bContinueRuleProcessing = true;
            if (ApplyRule_(pRule, account, pMessageData, bContinueRuleProcessing, ruleResult))
            {
               // The rule matched. We should return here.
               return;
            }

            if (!bContinueRuleProcessing)
            {
               break;
            }
         }
      }
   }

   bool
   RuleApplier::ApplyRule_(std::shared_ptr<Rule> pRule, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData, bool &bContinueRuleProcessing, RuleResult &ruleResult)
   {
		if (Logger::Instance()->GetLogDebug())
			LOG_DEBUG(_T("Applying rule " + pRule->GetName()));

      bool bAllRequired = pRule->GetUseAND();
      bool bDoActions = false;

      std::shared_ptr<RuleCriterias> pCriterias = pRule->GetCriterias();
      for (int i = 0; i < pCriterias->GetCount(); i++)
      {
         std::shared_ptr<RuleCriteria> pCriteria = pCriterias->GetItem(i);

         if (!pCriteria)
            continue;

         bool bMatch = MessageMatchesCriteria_(pCriteria, pMsgData);
         
         if (bAllRequired)
         {
            if (!bMatch)
            {
               return false;
            }

            bDoActions = true;
         }
         else
         {
            if (bMatch)
            {
               bDoActions = true;
               break;
            }
         }
      }

      if (bDoActions)
      {
         ApplyActions_(pRule, account, pMsgData, bContinueRuleProcessing, ruleResult);
      }

      return false;
   }

   void
   RuleApplier::ApplyActions_(std::shared_ptr<Rule> pRule, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData, bool &bContinueRuleProcessing, RuleResult &ruleResult)
   {  
      std::shared_ptr<RuleActions> pActions = pRule->GetActions();

      for (int i = 0; i < pActions->GetCount(); i++)
      {
         std::shared_ptr<RuleAction> pAction = pActions->GetItem(i);

         if (pAction)
         {
            ApplyAction_(pRule, pAction, account, pMsgData, bContinueRuleProcessing, ruleResult);
         }
      }
   }

   void
   RuleApplier::ApplyAction_(std::shared_ptr<Rule> pRule, std::shared_ptr<RuleAction> pAction, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData, bool &bContinueRuleProcessing, RuleResult &ruleResult)
   {  
      Logger::Instance()->LogDebug(_T("Performing rule action"));
      switch (pAction->GetType())
      {
      case RuleAction::Delete:  
         {
            ruleResult.SetDeleteEmail(true);

            String sDeleteRuleName;
            sDeleteRuleName.Format(_T("Rule name: %s, ID: %d"), pRule->GetName().c_str(), pRule->GetID());

            ruleResult.SetDeleteRuleName(sDeleteRuleName);
            break;
         }
      case RuleAction::MoveToIMAPFolder:
         {
            ruleResult.SetMoveToFolder(pAction->GetIMAPFolder());
            break;
         }
      case RuleAction::Forward:  
         {
            ApplyAction_Forward(pAction, account, pMsgData);
            break;
         }
      case RuleAction::Reply:
         {
            ApplyAction_Reply(pAction, account, pMsgData);
            break;
         }
      case RuleAction::ScriptFunction:
         {
            ApplyAction_ScriptFunction(pAction, account, pMsgData);
            break;
         }
      case RuleAction::SetHeaderValue:
         {
            ApplyAction_SetHeader(pAction, account, pMsgData);
            break;
         }
      case RuleAction::StopRuleProcessing:
         {
            bContinueRuleProcessing = false;
            break;
         }
      case RuleAction::SendUsingRoute:
         {
            ruleResult.SetSendUsingRoute(pAction->GetRouteID());
            break;
         }
      case RuleAction::CreateCopy:  
         {
            ApplyAction_Copy(pRule, account, pMsgData);
            break;
         }
      case RuleAction::BindToAddress:
         {
            ruleResult.SetBindToAddress(pAction->GetValue());
            break;
         }
      }
   }

   void 
   RuleApplier::ApplyAction_Forward(std::shared_ptr<RuleAction> pAction, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData) const
   {
      // false = check only loop counter not AutoSubmitted header because forward
      if (!IsGeneratedResponseAllowed(pMsgData, false))
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5064, "RuleApplier::ApplyAction_Forward", "Could not forward message. Maximum rule loop count reached.");
         return;
      }

      if (pAction->GetTo().IsEmpty())
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4225, "RuleApplier::ApplyAction_Forward", "Could not forward message; empty recipient address.");
         return;
      }

      std::shared_ptr<Message> pMsg = PersistentMessage::CopyToQueue(account, pMsgData->GetMessage());

      if (!pMsg)
         return;

      pMsg->SetState(Message::Delivering);

      std::shared_ptr<Account> emptyAccount;

      // Increase the number of rule-deliveries made.
      String newFileName = PersistentMessage::GetFileName(emptyAccount, pMsg);

      std::shared_ptr<MessageData> pNewMsgData = std::shared_ptr<MessageData>(new MessageData());
      pNewMsgData->LoadFromMessage(emptyAccount, pMsg);
      pNewMsgData->IncreaseRuleLoopCount();
      pNewMsgData->Write(newFileName);
      
      // We need to update the SMTP envelope from address, if this
      // message is forwarded by a user-level account.
      std::shared_ptr<CONST Account> pAccount = CacheContainer::Instance()->GetAccount(rule_account_id_);
      String sMailerDaemonAddress = MailerDaemonAddressDeterminer::GetMailerDaemonAddress(pMsg);
      if (pMsg->GetFromAddress().IsEmpty())
         pMsg->SetFromAddress(sMailerDaemonAddress);
      else if (pAccount && IniFileSettings::Instance()->GetRewriteEnvelopeFromWhenForwarding())
         pMsg->SetFromAddress(pAccount->GetAddress());
      
      // Add new recipients
      bool recipientOK = false;
      RecipientParser recipientParser;
      recipientParser.CreateMessageRecipientList(pAction->GetTo(), pMsg->GetRecipients(), recipientOK);

      // Check that there are recipients of the letter. If not, we should skip delivery.
      if (pMsg->GetRecipients()->GetCount() == 0)
      {
         // Delete the file since the message cannot be delivered.
         FileUtilities::DeleteFile(newFileName);

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4235, "RuleApplier::ApplyAction_Forward", "Could not forward message; no recipients.");
         return;
      }

      PersistentMessage::SaveObject(pMsg);
   }

   void 
   RuleApplier::ApplyAction_Copy(std::shared_ptr<Rule> rule, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData) const
   {
      // false = check only loop counter not AutoSubmitted header because forwarding copy
      if (!IsGeneratedResponseAllowed(pMsgData, false))
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5201, "RuleApplier::ApplyAction_Copy", "Could not copy message. Maximum rule loop count reached.");
         return;
      }

      std::shared_ptr<Message> pMsg = PersistentMessage::CopyToQueue(account, pMsgData->GetMessage());
      
      if (!pMsg)
         return;

      // We've created a copy of the message, but we need to copy the recipients as well.
      // The recipients are either the full list of SMTP envelope recipients, or, if it's
      // an account level rule it's just the address of the account. To determine this, we
      // check whether the message has been connected to any account yet.
      if (pMsgData->GetMessage()->GetAccountID() > 0)
      {
         // What account is it in?
         std::shared_ptr<const Account> recipientAccount = CacheContainer::Instance()->GetAccount(pMsgData->GetMessage()->GetAccountID());

         if (recipientAccount)
         {
            bool recipientOK = false;

            RecipientParser recipientParser;
            recipientParser.CreateMessageRecipientList(recipientAccount->GetAddress(), pMsg->GetRecipients(), recipientOK);
         }
      }
      else
      {
         // Copy the old reciopients.
         std::vector<std::shared_ptr<MessageRecipient> >  &oldRecipients = pMsgData->GetMessage()->GetRecipients()->GetVector();

         for(std::shared_ptr<MessageRecipient> recipient : oldRecipients)
         {
            std::shared_ptr<MessageRecipient> newRecipient = std::shared_ptr<MessageRecipient >(new MessageRecipient());
            newRecipient->CopyFrom(recipient);
            newRecipient->SetMessageID(0);

            pMsg->GetRecipients()->Add(newRecipient);
         }
      }

      pMsg->SetState(Message::Delivering);

      // Increase the number of rule-deliveries made.
      String newMessageFileName = PersistentMessage::GetFileName(pMsg);

      std::shared_ptr<MessageData> pNewMsgData = std::shared_ptr<MessageData>(new MessageData());
      pNewMsgData->LoadFromMessage(newMessageFileName, pMsg);
      pNewMsgData->IncreaseRuleLoopCount();
      pNewMsgData->SetFieldValue("X-CopyRule", rule->GetName());
      pNewMsgData->Write(newMessageFileName);

      // Check that there are recipients of the letter. If not, we should skip delivery.
      if (pMsg->GetRecipients()->GetCount() == 0)
      {
         // Delete the file since the message cannot be delivered.
         FileUtilities::DeleteFile(newMessageFileName);

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5201, "RuleApplier::ApplyAction_Copy", "Could not copy message; no recipients.");
         return;
      }

      PersistentMessage::SaveObject(pMsg);
   }

   void 
   RuleApplier::ApplyAction_ScriptFunction(std::shared_ptr<RuleAction> pAction, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData) const
   {
      // Run a custom function
      String sFunctionName = pAction->GetScriptFunction();

      std::shared_ptr<ScriptObjectContainer> pContainer = std::shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
      std::shared_ptr<Result> pResult = std::shared_ptr<Result>(new Result);
      pContainer->AddObject("HMAILSERVER_MESSAGE", pMsgData->GetMessage(), ScriptObject::OTMessage);
      pContainer->AddObject("Result", pResult, ScriptObject::OTResult);
      
      String sEventCaller = sFunctionName + "(HMAILSERVER_MESSAGE)";
      ScriptServer::Instance()->FireEvent(ScriptServer::EventCustom, sEventCaller, pContainer);

      // The script may have modified the message. We need to reload it. We could
      // do a CRC check or something similar to determine whether it has changed,
      // but performance-wise we can probably just as well reload it.
      pMsgData->LoadFromMessage(account, pMsgData->GetMessage());
   }

   void 
   RuleApplier::ApplyAction_SetHeader(std::shared_ptr<RuleAction> pAction, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData) const
   {
      // Run a custom function
      String sHeader = pAction->GetHeaderName();
      String sValue = pAction->GetValue();

      pMsgData->SetFieldValue(sHeader, sValue);

      String fileName = PersistentMessage::GetFileName(account, pMsgData->GetMessage());

      pMsgData->Write(fileName);
   }

   void 
   RuleApplier::ApplyAction_Reply(std::shared_ptr<RuleAction> pAction, std::shared_ptr<const Account> account, std::shared_ptr<MessageData> pMsgData) const
   {
      // true = check AutoSubmitted header and do not respond if set
      if (!IsGeneratedResponseAllowed(pMsgData, true))
      {
	      ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5065, "RuleApplier::ApplyAction_Reply", "Could not reply message. Maximum rule loop count reached or Auto-Submitted header.");
	      return;
      }

      String sReplyRecipientAddress = pMsgData->GetMessage()->GetFromAddress();

      if (sReplyRecipientAddress.IsEmpty())
      {
	      // We need a recipient address to be able to
	      // send the message..
	      return;
      }

      std::shared_ptr<Account> emptyAccount;

      // Send a copy of this email.
      std::shared_ptr<Message> pMsg = std::shared_ptr<Message>(new Message());
      pMsg->SetState(Message::Delivering);

      String newMessageFileName = PersistentMessage::GetFileName(pMsg);

      // check if this us a user-level account rule or global rule.
      std::shared_ptr<CONST Account> pAccount = CacheContainer::Instance()->GetAccount(rule_account_id_);

      std::shared_ptr<MessageData> pNewMsgData = std::shared_ptr<MessageData>(new MessageData());
      pNewMsgData->LoadFromMessage(newMessageFileName, pMsg);
      if (!pAccount)
         pNewMsgData->SetReturnPath("");
      pNewMsgData->GenerateMessageID();
      pNewMsgData->SetTo(sReplyRecipientAddress);
      pNewMsgData->SetFrom(pAction->GetFromName() + " <" + pAction->GetFromAddress() + ">");
      pNewMsgData->SetSubject(pAction->GetSubject());
      pNewMsgData->SetBody(pAction->GetBody());
      pNewMsgData->SetSentTime(Time::GetCurrentMimeDate());
      pNewMsgData->SetAutoReplied();
      pNewMsgData->IncreaseRuleLoopCount();
      pNewMsgData->Write(newMessageFileName);

      // We need to update the SMTP envelope from address, if this
      // message is replied to by a user-level account.
      if (pAccount)
	      pMsg->SetFromAddress(pAccount->GetAddress());

      // Add recipients.
      bool recipientOK = false;
      RecipientParser recipientParser;
      recipientParser.CreateMessageRecipientList(sReplyRecipientAddress, pMsg->GetRecipients(), recipientOK);

      PersistentMessage::SaveObject(pMsg);

   }

   bool
   RuleApplier::MessageMatchesCriteria_(std::shared_ptr<RuleCriteria> pCriteria, std::shared_ptr<MessageData> pMsgData) const
   {
      String sFieldValue;
      if (pCriteria->GetUsePredefined())
      {
         switch (pCriteria->GetPredefinedField())
         {
         case RuleCriteria::FTFrom:
            sFieldValue = pMsgData->GetFrom();
            break;
         case RuleCriteria::FTTo:
            sFieldValue = pMsgData->GetTo();
            break;
         case RuleCriteria::FTCC:
            sFieldValue = pMsgData->GetCC();
            break;
         case RuleCriteria::FTSubject:
            sFieldValue = pMsgData->GetSubject();
            break;
         case RuleCriteria::FTBody:
            sFieldValue = pMsgData->GetBody() + pMsgData->GetHTMLBody();
            break;
         case RuleCriteria::FTMessageSize:
            sFieldValue.Format(_T("%d"), pMsgData->GetSize());
            break;
         case RuleCriteria::FTDeliveryAttempts:
            sFieldValue.Format(_T("%u"), pMsgData->GetMessage()->GetNoOfRetries() + 1);
            break;
         case RuleCriteria::FTRecipientList:
            {
               std::shared_ptr<Message> pMessage = pMsgData->GetMessage();
               std::vector<std::shared_ptr<MessageRecipient> > vecRecipients = pMessage->GetRecipients()->GetVector();
               auto iterRecipient = vecRecipients.begin();

               while (iterRecipient != vecRecipients.end())
               {
                  if (!sFieldValue.IsEmpty())
                     sFieldValue += ";";

                  sFieldValue += (*iterRecipient)->GetAddress();
                  iterRecipient++;
               }

               break;
            }
         }
      }
      else
      {
         sFieldValue = pMsgData->GetFieldValue(pCriteria->GetHeaderField());
      }

      String matchValue = pCriteria->GetMatchValue();


      return TestMatch(matchValue, pCriteria->GetMatchType(), sFieldValue);
   }

   bool 
   RuleApplier::TestMatch(String matchValue, RuleCriteria::MatchType matchType, String testValue)
   {
      switch (matchType)
      {
      case RuleCriteria::Equals:
         {
            if (matchValue.CompareNoCase(testValue) == 0)
               return true;
            else
               return false;
         }
      case RuleCriteria::NotEquals:
         {
            if (matchValue.CompareNoCase(testValue) != 0)
               return true;
            else
               return false;
         }
      case RuleCriteria::Contains:
         {
            if (testValue.FindNoCase(matchValue) >= 0)
               return true;
            else
               return false;
         }
      case RuleCriteria::NotContains:
         {
            if (testValue.FindNoCase(matchValue) == -1)
               return true;
            else
               return false;
         }

      case RuleCriteria::LessThan:
         {
            testValue.Replace(_T(","), _T("."));
            matchValue.Replace(_T(","), _T("."));

            double iFieldValue =  _tstof(testValue);
            double iMatchValue = _tstof(matchValue);

            if (iMatchValue > iFieldValue)
               return true;
            else
               return false;
         }
      case RuleCriteria::GreaterThan:
         {
            testValue.Replace(_T(","), _T("."));
            matchValue.Replace(_T(","), _T("."));

            double iFieldValue = _tstof(testValue);
            double iMatchValue = _tstof(matchValue);

            if (iMatchValue < iFieldValue)
               return true;
            else
               return false;
         }
      case RuleCriteria::MatchesRegEx:
         {
            if (RegularExpression::TestExactMatch(matchValue, testValue))
               return true;
            else
               return false;
         }
      case RuleCriteria::Wildcard:
         {
            if (StringParser::WildcardMatchNoCase(matchValue, testValue))
               return true;
            else
               return false;

         }
      default:
         {
            assert(0); // unknown match type?
            return false;
         }


      }

      return false;
   }

   bool
   RuleApplier::IsGeneratedResponseAllowed(std::shared_ptr<MessageData> pMsgData, bool bChkAutoSubmit)
   {
      int iCurrProcessCount = pMsgData->GetRuleLoopCount();
      int iMaxAllowed = Configuration::Instance()->GetSMTPConfiguration()->GetRuleLoopLimit();

      if (iCurrProcessCount >= iMaxAllowed)
         return false;

      // bChkAutoSubmit - Fix for google calendar loop http://www.hmailserver.com/forum/viewtopic.php?f=7&t=24423
      if (bChkAutoSubmit && pMsgData->IsAutoSubmitted())
         return false;

      return true;
   }

}
