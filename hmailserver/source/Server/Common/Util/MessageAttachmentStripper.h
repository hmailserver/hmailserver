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

      static void Strip(std::shared_ptr<Message> pMessage);
   private:
      
      static void WriteToDisk_(std::shared_ptr<Message> pMessage, MimeBody &oMainMessage, std::shared_ptr<MimeBody> pBody);
      static bool IsGoodTextPart_(std::shared_ptr<MimeBody> pBody);
   };

}