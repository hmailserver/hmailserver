// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Attachments.h"
#include "Attachment.h"
#include "MessageData.h"

#include "../Mime/Mime.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Attachments::Attachments(std::shared_ptr<MimeBody> pMimeBody, MessageData *pMsgData) :
      mime_body_(pMimeBody),
      msg_data_(pMsgData)
   {

   }

   Attachments::~Attachments()
   {

   }

   std::shared_ptr<Attachment>
   Attachments::GetItem(unsigned int Index) const
   {
      std::shared_ptr<Attachment> pItem;

      if (Index < vecObjects.size())
         pItem = vecObjects[Index];  
       
      return pItem;
   }

   size_t
   Attachments::GetCount() const
   {
      return vecObjects.size();
   }

   void 
   Attachments::Load()
   {
      std::list<std::shared_ptr<MimeBody> > vecBodyList;
      mime_body_->GetAttachmentList(mime_body_, vecBodyList);

      for(std::shared_ptr<MimeBody> pBody : vecBodyList)
      {
         std::shared_ptr<Attachment> item = std::shared_ptr<Attachment>(new Attachment(mime_body_, pBody));
         vecObjects.push_back(item);
      }
   }

   void 
   Attachments::Clear()
   {
      vecObjects.clear();

      mime_body_->ClearAttachments();
   }

   bool
   Attachments::Add(const String &sFilename)
   {
      if (!FileUtilities::Exists(sFilename))      
         return false;

      // Load the attachment
      std::shared_ptr<MimeBody> pAttachment = msg_data_->CreatePart("application/octet-stream");
      pAttachment->SetTransferEncoding("base64");

      if (!pAttachment->ReadFromFile(sFilename))
         return false;

      // Add the attachment to the collection.
      std::shared_ptr<Attachment> pItem = std::shared_ptr<Attachment>(new Attachment(mime_body_, pAttachment));
      vecObjects.push_back(pItem);

      return true;
   }

   bool
   Attachments::Add(std::shared_ptr<Attachment> pAttachment)
   {
      vecObjects.push_back(pAttachment);
      return true;
   }
}
