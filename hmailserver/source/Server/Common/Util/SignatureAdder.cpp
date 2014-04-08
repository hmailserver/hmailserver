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
   SignatureAdder::SetSignature(shared_ptr<Message> pMessage, 
      shared_ptr<const Domain> pSenderDomain, 
      shared_ptr<const Account> pSenderAccount,
      shared_ptr<MessageData> &pMessageData)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Sets the signature of the message, based on the signature in the account
   // settings and domain settings.
   //---------------------------------------------------------------------------()
   {  
      if (!pMessage)
      {
         // Input error
         return false;
      }

      if (!pSenderDomain)
      {
         // Not a local sender - nothing to do.
         return false;
      }

      if (!pSenderDomain->GetAddSignaturesToLocalMail() && _GetMessageIsLocal(pMessage))
      {
         // The message is local, but we have configured
         // the server not to add signatures to local email.
         return false;
      }

      String sSignaturePlainText;
      String sSignatureHTML;

      // First check if an account signature has been specified.
      if (pSenderAccount && pSenderAccount->GetEnableSignature())
      {
         sSignaturePlainText = pSenderAccount->GetSignaturePlainText();
         sSignatureHTML = pSenderAccount->GetSignatureHTML();

         if (!sSignaturePlainText.IsEmpty() && sSignatureHTML.IsEmpty())
         {
            // Plain text specified but not HTML. Copy plain text to HTML.
            sSignatureHTML = sSignaturePlainText;
            sSignatureHTML.Replace(_T("\r\n"), _T("<br>\r\n"));
         }
      }

      if (pSenderDomain->GetEnableSignature())
      {
         String sDomainPlainText = pSenderDomain->GetSignaturePlainText();
         String sDomainHTML = pSenderDomain->GetSignatureHTML();

         if (!sDomainPlainText.IsEmpty() && sDomainHTML.IsEmpty())
         {
            // Plain text specified but not HTML. Copy plain text to HTML.
            sDomainHTML = sDomainPlainText;
            sDomainHTML.Replace(_T("\r\n"), _T("<br>\r\n"));
         }

         // Check if we should overwrite the account signature with the 
         // domain signature, if we should append it, or if we just should
         // keep it.
         if (pSenderDomain->GetSignatureMethod() == HM::Domain::SMSetIfNotSpecifiedInAccount)
         {
            if (sSignaturePlainText.IsEmpty()) 
               sSignaturePlainText = sDomainPlainText;
            if (sSignatureHTML.IsEmpty())
               sSignatureHTML = sDomainHTML;
         }
         else if (pSenderDomain->GetSignatureMethod() == HM::Domain::SMAppendToAccountSignature)
         {
            sSignaturePlainText += "\r\n\r\n" + sDomainPlainText;
            sSignatureHTML += "<br><br>" + sDomainHTML;
         }
         else if (pSenderDomain->GetSignatureMethod() == HM::Domain::SMOverwriteAccountSignature)
         {
            sSignaturePlainText = sDomainPlainText;
            sSignatureHTML = sDomainHTML;
         }            
      }

      if (sSignaturePlainText.IsEmpty() && sSignatureHTML.IsEmpty())  
      {
         // No signature should be created.
         return false;
      }

      // A signature should be created.
      if (!pMessageData)
      {
         pMessageData = shared_ptr<MessageData>(new MessageData());
         shared_ptr<Account> emptyAccount;
         if (!pMessageData->LoadFromMessage(emptyAccount, pMessage))
            return false;
      }

      if (!pSenderDomain->GetAddSignaturesToReplies() && _GetMessageIsReply(pMessageData))
      {
         // The message is a reply, but we have configured the
         // server not to add signatures to replies
         return false;
      }

      String sCurrentBodyPlainText = pMessageData->GetBody();
      String sCurrentBodyHTML = pMessageData->GetHTMLBody();

      if (!sSignaturePlainText.IsEmpty())
         sCurrentBodyPlainText += "\r\n" + sSignaturePlainText;

      if (!sCurrentBodyHTML.IsEmpty() && !sSignatureHTML.IsEmpty())
         sCurrentBodyHTML += "<br>" + sSignatureHTML;

      if (pSenderAccount)
      {
         sCurrentBodyPlainText.ReplaceNoCase(_T("%User.FirstName%"), pSenderAccount->GetPersonFirstName());
         sCurrentBodyPlainText.ReplaceNoCase(_T("%User.LastName%"), pSenderAccount->GetPersonLastName());

         sCurrentBodyHTML.ReplaceNoCase(_T("%User.FirstName%"), pSenderAccount->GetPersonFirstName());
         sCurrentBodyHTML.ReplaceNoCase(_T("%User.LastName%"), pSenderAccount->GetPersonLastName());
      }

      if (!sCurrentBodyPlainText.IsEmpty() && pMessageData->GetHasBodyType("text/plain"))
         pMessageData->SetBody(sCurrentBodyPlainText);

      if (!sCurrentBodyHTML.IsEmpty() && pMessageData->GetHasBodyType("text/html"))
         pMessageData->SetHTMLBody(sCurrentBodyHTML);

      return true;      


   }

   bool 
   SignatureAdder::_GetMessageIsReply(shared_ptr<MessageData> &pMessageData)
   {
      String sHeader = pMessageData->GetFieldValue("References");
      if (!sHeader.IsEmpty())
         return true; // Contains Reference header - It's a reply.

      sHeader = pMessageData->GetFieldValue("In-Reply-To");
      if (!sHeader.IsEmpty())
         return true; // Contains In-Reply-To header - It's a reply.

      return false;
   }

   bool 
   SignatureAdder::_GetMessageIsLocal(shared_ptr<Message> pMessage)
   {
      String sFromAddressDomain = StringParser::ExtractDomain(pMessage->GetFromAddress());
      
      // Loop over the recipients and check if they are on the same domain.if
      
      std::vector<shared_ptr<MessageRecipient> > &vecRecipients = pMessage->GetRecipients()->GetVector();
      std::vector<shared_ptr<MessageRecipient> >::iterator iter = vecRecipients.begin();
      std::vector<shared_ptr<MessageRecipient> >::iterator iterEnd = vecRecipients.end();

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
