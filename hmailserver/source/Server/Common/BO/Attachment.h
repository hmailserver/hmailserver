// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class MimeBody;

   class Attachment  
   {
   public:
	   Attachment(shared_ptr<MimeBody> pMessage, shared_ptr<MimeBody> pAttachment);
	   virtual ~Attachment();

      String GetFileName();
      int GetSize();
      void Delete();
      void SaveAs(const String &sSaveTo) const;
      
   private:
      
      shared_ptr<MimeBody> m_pAttachment;
      shared_ptr<MimeBody> m_pMessage;
   };

}
