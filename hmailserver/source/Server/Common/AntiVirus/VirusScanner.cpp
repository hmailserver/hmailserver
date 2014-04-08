// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "VirusScanner.h"

#include "../../SMTP/SMTPConfiguration.h"
#include "../Mime/Mime.h"
#include "../BO/Message.h"
#include "../Util/GUIDcreator.h"
#include "../Util/File.h"
#include "../BO/MessageData.h"

// The scanners:

#include "ClamAVVirusScanner.h"
#include "ClamWinVirusScanner.h"
#include "CustomVirusScanner.h"

#include "../Util/MessageUtilities.h"

// For attachment blocking

#include "../BO/BlockedAttachments.h"
#include "../BO/BlockedAttachment.h"
#include "../BO/Attachment.h"
#include "../BO/Attachments.h"
#include "../BO/MessageData.h"
#include "../BO/ServerMessages.h"

#include "../Persistence/PersistentServerMessage.h"
#include "../Persistence/PersistentMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   long VirusScanner::_runningScanners = 0;

   void 
   VirusScanner::ResetCounter()
   {
      InterlockedExchange(&_runningScanners, 0);
   }

   bool
   VirusScanner::GetVirusScanningEnabled()
   {
      AntiVirusConfiguration &antiVirusConfig = Configuration::Instance()->GetAntiVirusConfiguration();

      if (antiVirusConfig.GetClamAVEnabled() ||
          antiVirusConfig.ClamWinEnabled() ||
          antiVirusConfig.GetCustomScannerEnabled())
          return true;
      else
         return false;
   }

   /*
      This function will wait until there's a free virus scanner available.
      At most, hMailServer runs 10 virus scanners in parallel.



   */
   void
   VirusScanner::_WaitForFreeScanner()
   {
      int waitTime = 0;

      for (int failedCount = 0; failedCount < 10; failedCount++)
      {
         int currentCount = _runningScanners;

         while (currentCount >= MaxRunningScanners)
         {
            if (waitTime % 5 == 0)
            {
               LOG_DEBUG("Waiting for free virus scanner...");
            }
            
            waitTime++;
            Sleep(1000);

            if (waitTime > 60)
            {
               LOG_DEBUG("Waiting more than 60 seconds for an available virus scanner. Giving up waiting.");
               return;
            }

            currentCount = _runningScanners;
         }

         // make sure that no other thread is changing the value at the same time.
         // if that happens, we need to continue waiting for a new thread. unless
         // a compare was used here, this function could first wait and then call
         // InterlockedIncrement twice even though only one was allowed.
         int result = InterlockedCompareExchange(&_runningScanners, _runningScanners+1, currentCount);
         if (result == currentCount)
         {
            // woho.
            return;
         }
      }

      LOG_DEBUG("Waited for available virus scanner, tried to allocate, failed 10 times. Giving up waiting.");
   }

   void
   VirusScanner::DecreaseCounter()
   {
      InterlockedDecrement(&_runningScanners);
   }

   bool
   VirusScanner::Scan(shared_ptr<Message> pMessage, String &virusName)
   {
      AntiVirusConfiguration &antiVirusConfig = Configuration::Instance()->GetAntiVirusConfiguration();

      int iMaxVirusScanSizeKB = antiVirusConfig.GetVirusScanMaxSize();
      int iMessageSizeKB = pMessage->GetSize() / 1024;
      if (iMaxVirusScanSizeKB > 0 &&
          iMessageSizeKB > iMaxVirusScanSizeKB)
      {
         // Message is to big. Don't scan.
         return false;
      }

      // Prevent too many scanners from running at once.
      _WaitForFreeScanner();
      VirusScannerAutoCount autoCounter;

      // First scan the entire file.
      String sLongFilename = PersistentMessage::GetFileName(pMessage);

      VirusScanningResult result = _ScanFile(sLongFilename);
      if (result.GetVirusFound())
      {
         virusName = result.GetDetails();
         return true;
      }


      // Read message, extract attachments, 
      shared_ptr<MimeBody> pMimeBody = shared_ptr<MimeBody>(new MimeBody);
      pMimeBody->LoadFromFile(sLongFilename);

      list<shared_ptr<MimeBody> > oList;
      pMimeBody->GetAttachmentList(pMimeBody, oList);

      list<shared_ptr<MimeBody> >::iterator iter = oList.begin();

      while (iter != oList.end())
      {
         shared_ptr<MimeBody> pBody = (*iter);
         
         // Create a temporary filename.
         sLongFilename.Format(_T("%s\\%s.tmp"), IniFileSettings::Instance()->GetTempDirectory(), GUIDCreator::GetGUID() );
         pBody->WriteToFile(sLongFilename);

         VirusScanningResult result = _ScanFile(sLongFilename);
         if (result.GetVirusFound())
         {
            virusName = result.GetDetails();
            FileUtilities::DeleteFile(sLongFilename);
            return true;
         }

         FileUtilities::DeleteFile(sLongFilename);
         iter++;
      }

      return false;
   }

   void
   VirusScanner::ReportVirusFound(shared_ptr<Message> pMessage)
   {
      const String fileName = PersistentMessage::GetFileName(pMessage);

      shared_ptr<MessageData> pMsgData = shared_ptr<MessageData> (new MessageData());
      pMsgData->LoadFromMessage(fileName, pMessage);

      String sMessage;
      sMessage.Format(_T("Virus found in message from %s. Taking actions"), pMsgData->GetFrom());

      Logger::Instance()->LogApplication(sMessage);
   }

   void
   VirusScanner::BlockAttachments(shared_ptr<Message> pMessage)
   {
      shared_ptr<BlockedAttachments> pBlockedAttachments = HM::Configuration::Instance()->GetBlockedAttachments();

      vector<shared_ptr<BlockedAttachment> > vecBlockedAttachments = pBlockedAttachments->GetVector();
      vector<shared_ptr<BlockedAttachment> >::iterator iterBA;

      const String fileName = PersistentMessage::GetFileName(pMessage);

      shared_ptr<MessageData> pMsgData = shared_ptr<MessageData>(new MessageData());
      pMsgData->LoadFromMessage(fileName, pMessage);

      shared_ptr<Attachments> pAttachments = pMsgData->GetAttachments();

      bool bChangesMade = false;

      for (unsigned int i = 0; i < pAttachments->GetCount(); i++)
      {
         shared_ptr<Attachment> pAttachment = pAttachments->GetItem(i);

         // Check if attachment matches blocked file.
         for (iterBA = vecBlockedAttachments.begin(); iterBA < vecBlockedAttachments.end(); iterBA++)
         {
            String sWildcard = (*iterBA)->GetWildcard();

            if (StringParser::WildcardMatchNoCase(sWildcard, pAttachment->GetFileName()))
            {
               // Match. Remove the attachment and add a new dummy.
               bChangesMade = true;
               pAttachment->Delete();

               String sBody = Configuration::Instance()->GetServerMessages()->GetMessage("ATTACHMENT_REMOVED");

               // Replace macros.
               sBody.Replace(_T("%MACRO_FILE%"), pAttachment->GetFileName());

               // Add the new
               shared_ptr<MimeBody> pBody = pMsgData->CreatePart(_T("application/octet-stream"));
               pBody->SetRawText(sBody);

               // Create an content-disposition header.
               pBody->SetRawFieldValue(CMimeConst::ContentDisposition(), CMimeConst::Inline(), "");
               pBody->SetParameter(CMimeConst::ContentDisposition(), CMimeConst::Filename(), pAttachment->GetFileName() + ".txt");

               break;
            }

         }
      }

      if (bChangesMade)
      {
         pMsgData->Write(fileName);
         
         // Update the size of the message.
         pMessage->SetSize(FileUtilities::FileSize(fileName));
      }

   }

   VirusScanningResult
   VirusScanner::_ScanFile(const String &fileName)
   {
      AntiVirusConfiguration &antiVirusConfig = Configuration::Instance()->GetAntiVirusConfiguration();

      if (antiVirusConfig.ClamWinEnabled())
      {
         VirusScanningResult result = ClamWinVirusScanner::Scan(fileName);

         if (result.GetVirusFound())
            return result;
         else if (result.GetErrorOccured())
            _ReportScanningError(result);
      }

      if (antiVirusConfig.GetCustomScannerEnabled())
      {
         VirusScanningResult result = CustomVirusScanner::Scan(fileName);

         if (result.GetVirusFound())
            return result;
         else if (result.GetErrorOccured())
            _ReportScanningError(result);
      }

      if (antiVirusConfig.GetClamAVEnabled())
      {
         VirusScanningResult result = ClamAVVirusScanner::Scan(fileName);
         
         if (result.GetVirusFound())
            return result;
         else if (result.GetErrorOccured())
            _ReportScanningError(result);
      }

      return VirusScanningResult(VirusScanningResult::NoVirusFound, "");
   }

   void 
   VirusScanner::_ReportScanningError(const VirusScanningResult &scanningResult)
   {
      ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5406, scanningResult.GetErrorMessageSource(), scanningResult.GetDetails());
   }
}
