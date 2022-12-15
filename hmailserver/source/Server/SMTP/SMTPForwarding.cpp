// Copyright (c) 2008 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "SMTPForwarding.h"
#include "RuleApplier.h"

#include "../Common/BO/Account.h"
#include "../Common/BO/MessageData.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/MessageRecipients.h"

#include "../Common/Persistence/PersistentMessage.h"

#include "../common/Util/MailerDaemonAddressDeterminer.h"

#include "RecipientParser.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool 
   SMTPForwarding::PerformForwarding(std::shared_ptr<const Account> pRecipientAccount, std::shared_ptr<Message> pOriginalMessage)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Apply forwarding for the message.
   //---------------------------------------------------------------------------()
   {
      if (!pRecipientAccount->GetForwardEnabled())
         return true;
      
      if (pRecipientAccount->GetForwardAddress().IsEmpty())
      {
         // Configuration error. Forward was enabled, but no address specified.
         return true;
      }

      if (!pRecipientAccount->GetForwardAddress().CompareNoCase(pRecipientAccount->GetAddress()))
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4334, "SMTPDeliverer::_ApplyForwarding", "Could not forward message since target address as same as account address.");
         return true;
      }

      String sErrorMessage;
      bool bTreatSecurityAsLocal = false;
      RecipientParser recipientParser;
      if (recipientParser.CheckDeliveryPossibility(false, 
                                                    pRecipientAccount->GetAddress(), 
                                                    pRecipientAccount->GetForwardAddress(), 
                                                    sErrorMessage, 
                                                    bTreatSecurityAsLocal, 
                                                    0) != RecipientParser::DP_Possible)
      {
         String sLogMessage = Formatter::Format("Could not forward message from {0} to {1}. Reason: {2}",
                        pRecipientAccount->GetAddress(), pRecipientAccount->GetForwardAddress(), sErrorMessage);

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4382, "SMTPDeliverer::_ApplyForwarding", sLogMessage);

         return true;
      }


      String originalFileName = PersistentMessage::GetFileName(pRecipientAccount, pOriginalMessage);

      // Check that rule loop count is not yet reached.
      std::shared_ptr<MessageData> pOldMsgData  = std::shared_ptr<MessageData>(new MessageData());
      pOldMsgData->LoadFromMessage(originalFileName, pOriginalMessage);

      // false = check only loop counter not AutoSubmitted header because forward not rule
      if (!RuleApplier::IsGeneratedResponseAllowed(pOldMsgData, false))
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4333, "SMTPDeliverer::_ApplyForwarding", "Could not forward message. Maximum forward loop count reached.");

         return true;
      }

      LOG_DEBUG(_T("Forwarding message"));

      // Create a copy of the message
      std::shared_ptr<Message> pNewMessage = PersistentMessage::CopyToQueue(pRecipientAccount, pOriginalMessage);
     
      String sMailerDaemonAddress = MailerDaemonAddressDeterminer::GetMailerDaemonAddress(pNewMessage);
      if (pNewMessage->GetFromAddress().IsEmpty())
         pNewMessage->SetFromAddress(sMailerDaemonAddress);
      else if (IniFileSettings::Instance()->GetRewriteEnvelopeFromWhenForwarding())
         pNewMessage->SetFromAddress(pRecipientAccount->GetAddress());

      pNewMessage->SetState(Message::Delivering);
      
      // Increase the number of rule-deliveries made.
      std::shared_ptr<MessageData> pNewMsgData = std::shared_ptr<MessageData>(new MessageData());
      const String newFileName = PersistentMessage::GetFileName(pNewMessage);
      pNewMsgData->LoadFromMessage(newFileName, pNewMessage);
      pNewMsgData->IncreaseRuleLoopCount();
      pNewMsgData->Write(newFileName);

      // Add new recipients
      bool recipientOK = false;
      recipientParser.CreateMessageRecipientList(pRecipientAccount->GetForwardAddress(), pNewMessage->GetRecipients(), recipientOK);

      // Check that there are recipients of the letter. If not, we should skip delivery.
      if (pNewMessage->GetRecipients()->GetCount() == 0)
      {
         // Delete the file since the message cannot be delivered.
         FileUtilities::DeleteFile(newFileName);
         
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4332, "SMTPDeliverer::_ApplyForwarding", "Could not forward message; no recipients.");

         return true;
      }

      PersistentMessage::SaveObject(pNewMessage);

      bool bKeepOriginal = pRecipientAccount->GetForwardKeepOriginal();

      return bKeepOriginal;
   }

}

