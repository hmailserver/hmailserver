///* Copyright (c) Martin Knafve / The hMailServer Community Developers (HCD) hMailServer.com */

#pragma once

namespace HM
{
   class MimeBody;

   class Attachment  
   {
   public:
	   Attachment(std::shared_ptr<MimeBody> pMessage, std::shared_ptr<MimeBody> pAttachment);
	   virtual ~Attachment();

      void SetFileName(const String &file_name);
      void SetContent(const String &content);
      String GetFileName();
      int GetSize();
      void Delete();
      void SaveAs(const String &sSaveTo) const;
      
   private:
      
      std::shared_ptr<MimeBody> attachment_;
      std::shared_ptr<MimeBody> message_;
   };

}
