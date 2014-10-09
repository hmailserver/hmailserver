// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Attachment;
   class MimeBody;
   class MessageData;

   class Attachments 
   {
   public:
	   Attachments(std::shared_ptr<MimeBody> pMimeBody, MessageData *pMsgData);
	   virtual ~Attachments();

      bool Add(const String &sFilename);
      bool Add(std::shared_ptr<Attachment> pAttachment);
      
      std::shared_ptr<Attachment> GetItem(unsigned int index) const;

      size_t GetCount() const;
      void Clear();
      void Load();

      std::vector<std::shared_ptr<Attachment> > GetVector() { return vecObjects; }

   private:
      std::vector<std::shared_ptr<Attachment> > vecObjects;

      std::shared_ptr<MimeBody> mime_body_;
      MessageData *msg_data_;
   };
}
