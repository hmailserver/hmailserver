// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;

   class IMAPMessageInRange
   {
   public:
      IMAPMessageInRange();

      void Set(std::shared_ptr<Message> message, unsigned int messageUid, unsigned int messageIndex);

      std::shared_ptr<Message> GetMessage() { return message_;  }
      unsigned int GetUid() { return messageUid_; }
      unsigned int GetIndex() { return messageIndex_; }

   private:
      std::shared_ptr<Message> message_;
      unsigned int messageUid_;
      unsigned int messageIndex_;
   };

}
