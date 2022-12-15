// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "SignatureAdder.h"

#include "../BO/Account.h"
#include "../BO/Domain.h"
#include "../BO/MessageData.h"
#include "../BO/Message.h"
#include "../BO/MessageRecipient.h"
#include "../BO/MessageRecipients.h"

#include "../Mime/Mime.h"

#include "../../SMTP/SMTPConfiguration.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   SignatureAdder::SignatureAdder()
   {
      
   }

   SignatureAdder::~SignatureAdder()
   {
      
   }

   bool 
   SignatureAdder::SetSignature(std::shared_ptr<Message> message, 
      std::shared_ptr<const Domain> sender_domain, 
      std::shared_ptr<const Account> sender_account,
      std::shared_ptr<MessageData> &message_data)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Sets the signature of the message, based on the signature in the account
   // settings and domain settings.
   //---------------------------------------------------------------------------()
   {  
      if (!message)
      {
         assert(0);
         // Input error
         return false;
      }

      if (!sender_domain)
      {
         // Not a local sender - nothing to do.
         return false;
      }

      if (!sender_domain->GetAddSignaturesToLocalMail() && GetMessageIsLocal_(message))
      {
         // The message is local, but we have configured
         // the server not to add signatures to local email.
         return false;
      }

      String signature_plain_text;
      String signature_html;

      // First check if an account signature has been specified.
      if (sender_account && sender_account->GetEnableSignature())
      {
         signature_plain_text = sender_account->GetSignaturePlainText();
         signature_html = sender_account->GetSignatureHTML();

         if (!signature_plain_text.IsEmpty() && signature_html.IsEmpty())
         {
            // Plain text specified but not HTML. Copy plain text to HTML.
            signature_html = signature_plain_text;
            signature_html.Replace(_T("\r\n"), _T("<br>\r\n"));
         }
      }

      if (sender_domain->GetEnableSignature())
      {
         String sDomainPlainText = sender_domain->GetSignaturePlainText();
         String sDomainHTML = sender_domain->GetSignatureHTML();

         if (!sDomainPlainText.IsEmpty() && sDomainHTML.IsEmpty())
         {
            // Plain text specified but not HTML. Copy plain text to HTML.
            sDomainHTML = sDomainPlainText;
            sDomainHTML.Replace(_T("\r\n"), _T("<br>\r\n"));
         }

         // Check if we should overwrite the account signature with the 
         // domain signature, if we should append it, or if we just should
         // keep it.
         if (sender_domain->GetSignatureMethod() == HM::Domain::SMSetIfNotSpecifiedInAccount)
         {
            if (signature_plain_text.IsEmpty()) 
               signature_plain_text = sDomainPlainText;
            if (signature_html.IsEmpty())
               signature_html = sDomainHTML;
         }
         else if (sender_domain->GetSignatureMethod() == HM::Domain::SMAppendToAccountSignature)
         {
            signature_plain_text += "\r\n\r\n" + sDomainPlainText;
            signature_html += "<br><br>" + sDomainHTML;
         }
         else if (sender_domain->GetSignatureMethod() == HM::Domain::SMOverwriteAccountSignature)
         {
            signature_plain_text = sDomainPlainText;
            signature_html = sDomainHTML;
         }            
      }

      if (signature_plain_text.IsEmpty() && signature_html.IsEmpty())  
      {
         // No signature should be created.
         return false;
      }

      // A signature should be created.
      if (!message_data)
      {
         message_data = std::shared_ptr<MessageData>(new MessageData());
         std::shared_ptr<Account> emptyAccount;
         if (!message_data->LoadFromMessage(emptyAccount, message))
            return false;
      }

      if (!sender_domain->GetAddSignaturesToReplies() && GetMessageIsReply_(message_data))
      {
         // The message is a reply, but we have configured the
         // server not to add signatures to replies
         return false;
      }

      if (sender_account)
      {
         signature_plain_text.ReplaceNoCase(_T("%User.FirstName%"), sender_account->GetPersonFirstName());
         signature_plain_text.ReplaceNoCase(_T("%User.LastName%"), sender_account->GetPersonLastName());
         signature_plain_text.ReplaceNoCase(_T("%User.Address%"), sender_account->GetAddress());

         signature_html.ReplaceNoCase(_T("%User.FirstName%"), sender_account->GetPersonFirstName());
         signature_html.ReplaceNoCase(_T("%User.LastName%"), sender_account->GetPersonLastName());
         signature_html.ReplaceNoCase(_T("%User.Address%"), sender_account->GetAddress());
      }

      auto text_plain_body_part = message_data->GetBodyTextPlainPart();
      if (text_plain_body_part)
      {
         String text_plan_body_content = text_plain_body_part->GetUnicodeText();
         text_plan_body_content += "\r\n" + signature_plain_text;
         text_plain_body_part->SetUnicodeText(text_plan_body_content);
      }

      auto text_html_body_part = message_data->GetBodyTextHtmlPart();
      if (text_html_body_part)
      {
         String text_html_body_content = text_html_body_part->GetUnicodeText();
         text_html_body_content += "<br/>\r\n" + signature_html;
         text_html_body_part->SetUnicodeText(text_html_body_content);
      }

      return true;      


   }

   bool 
   SignatureAdder::GetMessageIsReply_(std::shared_ptr<MessageData> &message_data)
   {
      String sHeader = message_data->GetFieldValue("References");
      if (!sHeader.IsEmpty())
         return true; // Contains Reference header - It's a reply.

      sHeader = message_data->GetFieldValue("In-Reply-To");
      if (!sHeader.IsEmpty())
         return true; // Contains In-Reply-To header - It's a reply.

      return false;
   }

   bool 
   SignatureAdder::GetMessageIsLocal_(std::shared_ptr<Message> message)
   {
      String sFromAddressDomain = StringParser::ExtractDomain(message->GetFromAddress());
      
      // Loop over the recipients and check if they are on the same domain.if
      
      std::vector<std::shared_ptr<MessageRecipient> > &vecRecipients = message->GetRecipients()->GetVector();
      auto iter = vecRecipients.begin();
      auto iterEnd = vecRecipients.end();

      for (; iter != iterEnd; iter++)
      {
         String sRecipientAddress = (*iter)->GetAddress();
         String sRecipientDomain = StringParser::ExtractDomain(sRecipientAddress);

         if (sFromAddressDomain.CompareNoCase(sRecipientDomain) != 0)
            return false;

      }
      

      return true;
   }
}
