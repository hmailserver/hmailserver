// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class MimeBody;
   class CMimeMessage;
   
   
   class MessageAttachmentStripper  
   {
   public:
	   MessageAttachmentStripper();
	   virtual ~MessageAttachmentStripper();

      static void Strip(shared_ptr<Message> pMessage);
   private:
      
      static void _WriteToDisk(shared_ptr<Message> pMessage, MimeBody &oMainMessage, shared_ptr<MimeBody> pBody);
      static bool _IsGoodTextPart(shared_ptr<MimeBody> pBody);
   };

}