// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class MimeBody;

   class Attachment  
   {
   public:
	   Attachment(std::shared_ptr<MimeBody> pMessage, std::shared_ptr<MimeBody> pAttachment);
	   virtual ~Attachment();

      String GetFileName();
      int GetSize();
      void Delete();
      void SaveAs(const String &sSaveTo) const;
      
   private:
      
      std::shared_ptr<MimeBody> attachment_;
      std::shared_ptr<MimeBody> message_;
   };

}
