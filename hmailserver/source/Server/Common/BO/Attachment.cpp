// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "Attachment.h"

#include "../Mime/Mime.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Attachment::Attachment(std::shared_ptr<MimeBody> pMessage, std::shared_ptr<MimeBody> pAttachment) :
      message_(pMessage),
      attachment_(pAttachment)
   {

   }

   Attachment::~Attachment()
   {

   }

   void
   Attachment::SaveAs(const String &sSaveTo) const
   {
      attachment_->WriteToFile(sSaveTo);
   }

   String 
   Attachment::GetFileName()
   {
      return attachment_->GetUnicodeFilename();
   }

   bool
   Attachment::GetEncapsulatedFileNames(std::vector<String> &file_names) const
   {
      return attachment_->GetEncapsulatedFileNames(file_names);
   }

   void
   Attachment::ReplaceWithText(const String &file_name, const String &content)
   {
      // Replace the type and disposition too, so clients don't read the text as the original type.
      // The type must be set before the file name, since the name may be stored in Content-Type.
      attachment_->SetContentType("text/plain", "");
      attachment_->SetDisposition("attachment", "");
      attachment_->SetFileName(file_name);
      attachment_->SetUnicodeText(content);
   }

   int
   Attachment::GetSize()
   {
      return attachment_->GetContentLength();
   }

   void
   Attachment::Delete()
   {
      // Remove this attachment from the parent message.
      message_->RemoveAttachment(attachment_);
   }
}
