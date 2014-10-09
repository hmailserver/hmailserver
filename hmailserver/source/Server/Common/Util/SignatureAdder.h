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

      bool SetSignature(std::shared_ptr<Message> message, 
                        std::shared_ptr<const Domain> sender_domain, 
                        std::shared_ptr<const Account> sender_account,
                        std::shared_ptr<MessageData> &message_data);
      // Sets the signature of the message, based on the signature in the account
      // settings and domain settings.

      
   private:

      bool GetMessageIsReply_(std::shared_ptr<MessageData> &message_data);
      bool GetMessageIsLocal_(std::shared_ptr<Message> message);
   

   };
}

