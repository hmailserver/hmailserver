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
	   Attachments(shared_ptr<MimeBody> pMimeBody, MessageData *pMsgData);
	   virtual ~Attachments();

      bool Add(const String &sFilename);
      bool Add(shared_ptr<Attachment> pAttachment);
      
      shared_ptr<Attachment> GetItem(unsigned int index) const;

      size_t GetCount() const;
      void Clear();
      
      void Load();

   private:
      std::vector<shared_ptr<Attachment> > vecObjects;

      shared_ptr<MimeBody> m_pMimeBody;
      MessageData *m_pMsgData;
   };
}
