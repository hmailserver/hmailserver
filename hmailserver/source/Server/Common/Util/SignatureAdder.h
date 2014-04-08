// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


namespace HM
{
   class Message;
   class Domain;
   class MessageData;
   
   class SignatureAdder
   {
   public:
	   SignatureAdder();
      virtual ~SignatureAdder();

      bool SetSignature(shared_ptr<Message> pMessage, 
                        shared_ptr<const Domain> pSenderDomain, 
                        shared_ptr<const Account> pSenderAccount,
                        shared_ptr<MessageData> &pMessageData);
      // Sets the signature of the message, based on the signature in the account
      // settings and domain settings.

      
   private:

      bool _GetMessageIsReply(shared_ptr<MessageData> &pMessageData);
      bool _GetMessageIsLocal(shared_ptr<Message> pMessage);
   

   };
}

