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
   Attachments::Attachments(shared_ptr<MimeBody> pMimeBody, MessageData *pMsgData) :
      m_pMimeBody(pMimeBody),
      m_pMsgData(pMsgData)
   {

   }

   Attachments::~Attachments()
   {

   }

   shared_ptr<Attachment>
   Attachments::GetItem(unsigned int Index) const
   {
      shared_ptr<Attachment> pItem;

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
      list<shared_ptr<MimeBody> > vecBodyList;
      m_pMimeBody->GetAttachmentList(m_pMimeBody, vecBodyList);

      boost_foreach(shared_ptr<MimeBody> pBody, vecBodyList)
      {
         shared_ptr<Attachment> item = shared_ptr<Attachment>(new Attachment(m_pMimeBody, pBody));
         vecObjects.push_back(item);
      }
   }

   void 
   Attachments::Clear()
   {
      vecObjects.clear();

      m_pMimeBody->ClearAttachments();
   }

   bool
   Attachments::Add(const String &sFilename)
   {
      if (!FileUtilities::Exists(sFilename))      
         return false;

      // Load the attachment
      shared_ptr<MimeBody> pAttachment = m_pMsgData->CreatePart("application/octet-stream");
      pAttachment->SetTransferEncoding("base64");

      if (!pAttachment->ReadFromFile(sFilename))
         return false;

      // Add the attachment to the collection.
      shared_ptr<Attachment> pItem = shared_ptr<Attachment>(new Attachment(m_pMimeBody, pAttachment));
      vecObjects.push_back(pItem);

      return true;
   }

   bool
   Attachments::Add(shared_ptr<Attachment> pAttachment)
   {
      vecObjects.push_back(pAttachment);
      return true;
   }
}
