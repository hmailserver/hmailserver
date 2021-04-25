// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "SMTPVirusNotifier.h"

#include "../Common/BO/Message.h"
#include "../Common/Mime/Mime.h"
#include "../Common/Util/Time.h"
#include "../common/Util/MailerDaemonAddressDeterminer.h"
#include "../Common/BO/MessageRecipients.h"
#include "../Common/BO/MessageData.h"
#include "../Common/Persistence/PersistentMessage.h"
#include "../common/Persistence/PersistentServerMessage.h"

#include "RecipientParser.h"

#include "../common/BO/ServerMessages.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SMTPVirusNotifier::SMTPVirusNotifier()
   {

   }

   SMTPVirusNotifier::~SMTPVirusNotifier()
   {

   }

   void 
   SMTPVirusNotifier::CreateMessageDeletedNotification(const std::shared_ptr<Message> pOriginalMessage, const String &sRecipient)
   {
      LOG_DEBUG("SMTPVirusNotifier::CreateMessageDeletedNotification");

      if (pOriginalMessage->GetFromAddress().IsEmpty())
      {
         // No sender address specified, hence no place to
         // deliver the error log.
         return;
      }

       // Parse out the subject in the original
      // message, so that we can Re: that..

      MimeHeader header;
      AnsiString sHeader = PersistentMessage::LoadHeader(PersistentMessage::GetFileName(pOriginalMessage));
      header.Load(sHeader, sHeader.GetLength(), true);
       
      String sMailerDaemonAddress = MailerDaemonAddressDeterminer::GetMailerDaemonAddress(pOriginalMessage);

      String sTextVirusFound = Configuration::Instance()->GetServerMessages()->GetMessage("VIRUS_FOUND");
      String sErrMsg = Configuration::Instance()->GetServerMessages()->GetMessage("VIRUS_NOTIFICATION");

      String sOriginalSubject = header.GetRawFieldValue("Subject");

      // Replace macros.
      sErrMsg.Replace(_T("%MACRO_FROM%"), String(header.GetRawFieldValue("From")));
      sErrMsg.Replace(_T("%MACRO_TO%"), String(header.GetRawFieldValue("To")));
      sErrMsg.Replace(_T("%MACRO_SENT%"), String(header.GetRawFieldValue("Date")));
      sErrMsg.Replace(_T("%MACRO_SUBJECT%"), sOriginalSubject);

      // Send a copy of this email.
      std::shared_ptr<Message> pMsg = std::shared_ptr<Message>(new Message());

      pMsg->SetState(Message::Delivering);
      pMsg->SetFromAddress("");

      std::shared_ptr<Account> account;

      std::shared_ptr<MessageData> pNewMsgData = std::shared_ptr<MessageData>(new MessageData());
      pNewMsgData->LoadFromMessage(account, pMsg);

      // Required headers
      pNewMsgData->SetReturnPath("");
      pNewMsgData->GenerateMessageID();
      pNewMsgData->SetSentTime(Time::GetCurrentMimeDate());

      // Optional headers
      pNewMsgData->SetFrom(sMailerDaemonAddress);
      pNewMsgData->SetTo(sRecipient);
      pNewMsgData->SetSubject(sTextVirusFound + ": " + sOriginalSubject);
      pNewMsgData->SetBody(sErrMsg);
      pNewMsgData->IncreaseRuleLoopCount();

      // Write message data
      pNewMsgData->Write(PersistentMessage::GetFileName(pMsg));

      // Add recipients.
      bool recipientOK = false;
      RecipientParser recipientParser;
      recipientParser.CreateMessageRecipientList(sRecipient, pMsg->GetRecipients(), recipientOK);

      // Save message
      if (pMsg->GetRecipients()->GetCount() > 0)
         PersistentMessage::SaveObject(pMsg);

      // Tell app to submit mail
      Application::Instance()->SubmitPendingEmail();  

      LOG_DEBUG("SMTPVirusNotifier::~CreateMessageDeletedNotification");
   }
}
