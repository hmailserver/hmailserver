// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "SMTPVacationMessageCreator.h"

#include "../Common/BO/Account.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/MessageData.h"
#include "../Common/Mime/Mime.h"
#include "../Common/Util/Time.h"
#include "../Common/Persistence/PersistentMessage.h"
#include "RecipientParser.h"
#include "../Common/BO/MessageRecipients.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   SMTPVacationMessageCreator::SMTPVacationMessageCreator()
   {

   }

   SMTPVacationMessageCreator::~SMTPVacationMessageCreator()
   {

   }

   
   void 
   SMTPVacationMessageCreator::CreateVacationMessage(std::shared_ptr<const Account> recipientAccount, 
                                        const String &sToAddress, 
                                        const String &sVacationSubject, 
                                        const String &sVacationMessage,
                                        const std::shared_ptr<Message> pOriginalMessage)
   {
      
      
      
      
      if (!CanSendVacationMessage_(recipientAccount->GetAddress(), sToAddress))
         return; // We have already notified this user.
      
      LOG_DEBUG("Creating out-of-office message.");      

      String sModifiedSubject = sVacationSubject;
      String sModifiedBody = sVacationMessage;

      const String originalFileName = PersistentMessage::GetFileName(recipientAccount, pOriginalMessage);

      if (sModifiedSubject.Find(_T("%")) >= 0 || sModifiedBody.Find(_T("%")) >= 0)
      {
         // Replace macros in the string.
         MimeHeader header;
         AnsiString sHeader = PersistentMessage::LoadHeader(originalFileName);
         header.Load(sHeader, sHeader.GetLength(), true);

         String sOldSubject;
         if (header.GetField("Subject"))
            sOldSubject = header.GetField("Subject")->GetValue();

         sModifiedSubject.ReplaceNoCase(_T("%SUBJECT%"), sOldSubject);
         sModifiedBody.ReplaceNoCase(_T("%SUBJECT%"), sOldSubject);
      }

      if (sModifiedSubject.GetLength() == 0)
      {
         // Parse out the subject in the original
         // message, so that we can Re: that..
         MimeHeader header;

         AnsiString sHeader = PersistentMessage::LoadHeader(originalFileName);
         header.Load(sHeader, sHeader.GetLength(), true);

         String sOldSubject;
         if (header.GetField("Subject"))
            sOldSubject = header.GetField("Subject")->GetValue();

         sModifiedSubject = "Re: " + sOldSubject;
      }


      // Send a copy of this email.
      std::shared_ptr<Message> pMsg = std::shared_ptr<Message>(new Message());

      pMsg->SetState(Message::Delivering);
      pMsg->SetFromAddress(recipientAccount->GetAddress());

      const String newFileName = PersistentMessage::GetFileName(pMsg);

      std::shared_ptr<MessageData> pNewMsgData = std::shared_ptr<MessageData>(new MessageData());
      pNewMsgData->LoadFromMessage(newFileName, pMsg);
      
      // Required headers
      pNewMsgData->SetReturnPath(recipientAccount->GetAddress());
      pNewMsgData->GenerateMessageID();
      pNewMsgData->SetSentTime(Time::GetCurrentMimeDate());
      pNewMsgData->SetFieldValue("Content-Type", "text/plain; charset=\"utf-8\"");

      // Optional headers
      pNewMsgData->SetFrom(recipientAccount->GetAddress());
      pNewMsgData->SetTo(sToAddress);
      pNewMsgData->SetSubject(sModifiedSubject);
      pNewMsgData->SetBody(sModifiedBody);
	  pNewMsgData->SetAutoReplied();
      pNewMsgData->IncreaseRuleLoopCount();
      
      // Write message data
      pNewMsgData->Write(newFileName);

      // Add recipients.
      bool recipientOK = false;
      RecipientParser recipientParser;
      recipientParser.CreateMessageRecipientList(sToAddress, pMsg->GetRecipients(), recipientOK);

      // Save message
      if (pMsg->GetRecipients()->GetCount() > 0)
         PersistentMessage::SaveObject(pMsg);

      // Tell app to submit mail
      Application::Instance()->SubmitPendingEmail();  
   }

   bool 
   SMTPVacationMessageCreator::CanSendVacationMessage_(const String &sFrom, const String &sTo)
   {
      if (sTo.IsEmpty())
      {
         // We need a recipient address to be able to send.
         return false;
      }

      try
      {
         boost::lock_guard<boost::recursive_mutex> guard(mutex_);

         auto iter = mapVacationMessageRecipients.find(sFrom);

         if (iter == mapVacationMessageRecipients.end())
         {
            mapVacationMessageRecipients.insert(std::make_pair(sFrom, sTo));
            return true;
         }

         auto iterRange = 
            mapVacationMessageRecipients.equal_range(sFrom);

         iter = iterRange.first;
         while (iter != iterRange.second)
         {
            String sCurTo = (*iter).second;

            if (sCurTo == sTo)
            {
               // This recipient has already receivedd
               // a vacation-reply.
               return false;
            }

            iter++;
         }

         mapVacationMessageRecipients.insert(std::make_pair(sFrom, sTo));
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5022, "SMTPVacationMessageCreator::CanSendVacationMessage", "Error when doing lookup.");
      }

      return true;
   }

   void 
   SMTPVacationMessageCreator::VacationMessageTurnedOff(const String &sUserAddress)
   {
      try
      {
         boost::lock_guard<boost::recursive_mutex> guard(mutex_);

         auto iterRange = 
            mapVacationMessageRecipients.equal_range(sUserAddress);

         auto iter = iterRange.first;
         while (iter != iterRange.second)
            iter = mapVacationMessageRecipients.erase(iter);
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5023, "SMTPVacationMessageCreator::VacationMessageTurnedOff", "Error when removing user from list.");
      }
   }
}
