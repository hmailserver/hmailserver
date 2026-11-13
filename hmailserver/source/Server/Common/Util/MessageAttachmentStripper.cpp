// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "MessageAttachmentStripper.h"
#include "../Mime/Mime.h"
#include "../BO/Message.h"
#include "../BO/ServerMessages.h"
#include "../Persistence/PersistentMessage.h"
#include "../Persistence/PersistentServerMessage.h"
#include "../AntiVirus/VirusScanner.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MessageAttachmentStripper::MessageAttachmentStripper()
   {

   }

   MessageAttachmentStripper::~MessageAttachmentStripper()
   {

   }

   void
   MessageAttachmentStripper::Strip(std::shared_ptr<Message> pMessage)
   {
      const String fileName = PersistentMessage::GetFileName(pMessage);

      // Read message, extract attachments, 
      MimeBody oMimeMessage;
      oMimeMessage.LoadFromFile(fileName);

      // Keep the first text part. If there is none, keep only the notification.
      std::shared_ptr<MimeBody> pTextPart;

      std::shared_ptr<MimeBody> pBody = oMimeMessage.FindFirstPart();
      while (pBody && !pTextPart)
      {
         if (pBody->IsMultiPart())
         {
            std::shared_ptr<MimeBody> pSubBody = pBody->FindFirstPart();

            while (pSubBody && !pTextPart)
            {
               if (IsGoodTextPart_(pSubBody))
                  pTextPart = pSubBody;

               pSubBody = pBody->FindNextPart();
            }
         }

         if (!pTextPart && IsGoodTextPart_(pBody))
            pTextPart = pBody;

         pBody = oMimeMessage.FindNextPart();
      }

      // The text part may itself hold the virus. Unless it scans clean, keep only the notification.
      if (pTextPart)
      {
         VirusScanningResult result = VirusScanner::ScanPart(pTextPart);
         if (result.GetVirusFound() || result.GetErrorOccured())
            pTextPart.reset();
      }

      WriteToDisk_(pMessage, oMimeMessage, pTextPart);
   }

   void
   MessageAttachmentStripper::WriteToDisk_(std::shared_ptr<Message> pMessage, MimeBody &oMainMessage, std::shared_ptr<MimeBody> pBody)
   {
      AnsiString sHeader;

      std::vector<MimeField> oFieldList = oMainMessage.Fields();

      auto iterField = oFieldList.begin();

      while (iterField != oFieldList.end())
      {
         MimeField oField = (*iterField);

         AnsiString sName = oField.GetName();
         if (sName.CompareNoCase("subject") == 0)
         {
            String sTextVirusFound = Configuration::Instance()->GetServerMessages()->GetMessage("VIRUS_FOUND");

            sHeader += sName;
            sHeader += ": " + sTextVirusFound +": ";
            sHeader += oField.GetValue();
            sHeader += "\r\n";
         }
         else
         if (sName.CompareNoCase("content-type") != 0 &&
             sName.CompareNoCase("content-transfer-encoding") != 0)
         {
            sHeader += sName;
            sHeader += ": ";
            sHeader += oField.GetValue();
            sHeader += "\r\n";
         }

         iterField++;
      }

      String sBody = pBody ? String(pBody->GetRawText()) : String();

      String sNotification = Configuration::Instance()->GetServerMessages()->GetMessage("VIRUS_ATTACHMENT_REMOVED");

      sBody = sNotification + sBody;

      String sData = sHeader;
      sData += "\r\n";
      sData += sBody + "\r\n";

      pMessage->SetSize(sData.GetLength());

      String fileName = PersistentMessage::GetFileName(pMessage);

      FileUtilities::WriteToFile(fileName, sData, false);
   }


   bool
   MessageAttachmentStripper::IsGoodTextPart_(std::shared_ptr<MimeBody> pBody)
   {
      if (!pBody || pBody->IsAttachment())
         return false;

      String sMainType = pBody->GetMainType();
      return sMainType.CompareNoCase(_T("text")) == 0;
   }
}
