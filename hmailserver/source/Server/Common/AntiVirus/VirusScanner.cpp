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
   long VirusScanner::running_scanners_ = 0;

   void 
   VirusScanner::ResetCounter()
   {
      InterlockedExchange(&running_scanners_, 0);
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
   VirusScanner::WaitForFreeScanner_()
   {
      int waitTime = 0;

      for (int failedCount = 0; failedCount < 10; failedCount++)
      {
         int currentCount = running_scanners_;

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

            currentCount = running_scanners_;
         }

         // make sure that no other thread is changing the value at the same time.
         // if that happens, we need to continue waiting for a new thread. unless
         // a compare was used here, this function could first wait and then call
         // InterlockedIncrement twice even though only one was allowed.
         int result = InterlockedCompareExchange(&running_scanners_, running_scanners_+1, currentCount);
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
      InterlockedDecrement(&running_scanners_);
   }

   bool
   VirusScanner::Scan(std::shared_ptr<Message> pMessage, String &virusName)
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
      WaitForFreeScanner_();
      VirusScannerAutoCount autoCounter;

      // First scan the entire file.
      String sLongFilename = PersistentMessage::GetFileName(pMessage);

      VirusScanningResult result = ScanFile_(sLongFilename);
      if (result.GetVirusFound())
      {
         virusName = result.GetDetails();
         return true;
      }


      // Read message, extract attachments, 
      std::shared_ptr<MimeBody> pMimeBody = std::shared_ptr<MimeBody>(new MimeBody);
      pMimeBody->LoadFromFile(sLongFilename);

      std::list<std::shared_ptr<MimeBody> > oList;
      pMimeBody->GetAttachmentList(pMimeBody, oList);

      auto iter = oList.begin();

      while (iter != oList.end())
      {
         VirusScanningResult partResult = ScanPart_(*iter);
         if (partResult.GetVirusFound())
         {
            virusName = partResult.GetDetails();
            return true;
         }

         iter++;
      }

      return false;
   }

   VirusScanningResult
   VirusScanner::ScanPart(std::shared_ptr<MimeBody> pBody)
   {
      WaitForFreeScanner_();
      VirusScannerAutoCount autoCounter;

      return ScanPart_(pBody);
   }

   VirusScanningResult
   VirusScanner::ScanPart_(std::shared_ptr<MimeBody> pBody)
   {
      String sFilename;
      sFilename.Format(_T("%s\\%s.tmp"), IniFileSettings::Instance()->GetTempDirectory().c_str(), GUIDCreator::GetGUID().c_str());
      pBody->WriteToFile(sFilename);

      VirusScanningResult result = ScanFile_(sFilename);
      FileUtilities::DeleteFile(sFilename);

      return result;
   }

   void
   VirusScanner::ReportVirusFound(std::shared_ptr<Message> pMessage)
   {
      const String fileName = PersistentMessage::GetFileName(pMessage);

      std::shared_ptr<MessageData> pMsgData = std::shared_ptr<MessageData> (new MessageData());
      pMsgData->LoadFromMessage(fileName, pMessage);

      String sMessage;
      sMessage.Format(_T("Virus found in message from %s. Taking actions"), pMsgData->GetFrom().c_str());

      Logger::Instance()->LogApplication(sMessage);
   }

   void
   VirusScanner::BlockAttachments(std::shared_ptr<Message> message)
   {
      std::shared_ptr<BlockedAttachments> blocked_attachments_config = HM::Configuration::Instance()->GetBlockedAttachments();
      std::vector<std::shared_ptr<BlockedAttachment> > blocked_attachment_wildcards = blocked_attachments_config->GetSnapshot();

      const String file_name = PersistentMessage::GetFileName(message);

      std::shared_ptr<MessageData> message_data = std::shared_ptr<MessageData>(new MessageData());
      message_data->LoadFromMessage(file_name, message);

      std::shared_ptr<Attachments> pAttachments = message_data->GetAttachments();

      // Parts that could not be parsed are not checked. Rewriting the message drops them.
      bool changes_made = message_data->GetMimeMessage()->IsTruncated();

      for (auto attachment : pAttachments->GetVector())
      {
         // Check the attachment and, for a forwarded message, the attachments inside it.
         std::vector<String> file_names;
         file_names.push_back(attachment->GetFileName());
         bool fully_parsed = attachment->GetEncapsulatedFileNames(file_names);

         auto is_blocked = [&blocked_attachment_wildcards](const String &name)
            {
               return std::any_of(blocked_attachment_wildcards.begin(), blocked_attachment_wildcards.end(), [&name](std::shared_ptr<BlockedAttachment> blocked_attachment)
                  {
                     return StringParser::WildcardMatchNoCase(blocked_attachment->GetWildcard(), name);
                  });
            };

         // A forwarded message that cannot be fully inspected is blocked.
         if (!fully_parsed || std::any_of(file_names.begin(), file_names.end(), is_blocked))
         {
            // Attachment should be blocked.
            String updated_message_body = Configuration::Instance()->GetServerMessages()->GetMessage("ATTACHMENT_REMOVED");

            // Replace macros.
            updated_message_body.Replace(_T("%MACRO_FILE%"), attachment->GetFileName());

            attachment->ReplaceWithText(attachment->GetFileName() + ".txt", updated_message_body);

            changes_made = true;
         }

      }

      if (changes_made)
      {
         message_data->Write(file_name);
            
         // Update the size of the message.
         message->SetSize(FileUtilities::FileSize(file_name));
      }


   }

   VirusScanningResult
   VirusScanner::ScanFile_(const String &fileName)
   {
      AntiVirusConfiguration &antiVirusConfig = Configuration::Instance()->GetAntiVirusConfiguration();

      // Becomes the first scanner error, if no scanner finds a virus.
      VirusScanningResult outcome(VirusScanningResult::NoVirusFound, "");

      if (antiVirusConfig.ClamWinEnabled())
      {
         VirusScanningResult result = ClamWinVirusScanner::Scan(fileName);

         if (result.GetVirusFound())
            return result;
         else if (result.GetErrorOccured())
         {
            ReportScanningError_(result);
            if (!outcome.GetErrorOccured())
               outcome = result;
         }
      }

      if (antiVirusConfig.GetCustomScannerEnabled())
      {
         VirusScanningResult result = CustomVirusScanner::Scan(fileName);

         if (result.GetVirusFound())
            return result;
         else if (result.GetErrorOccured())
         {
            ReportScanningError_(result);
            if (!outcome.GetErrorOccured())
               outcome = result;
         }
      }

      if (antiVirusConfig.GetClamAVEnabled())
      {
         VirusScanningResult result = ClamAVVirusScanner::Scan(fileName);

         if (result.GetVirusFound())
            return result;
         else if (result.GetErrorOccured())
         {
            ReportScanningError_(result);
            if (!outcome.GetErrorOccured())
               outcome = result;
         }
      }

      return outcome;
   }

   void 
   VirusScanner::ReportScanningError_(const VirusScanningResult &scanningResult)
   {
      ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5406, scanningResult.GetErrorMessageSource(), scanningResult.GetDetails());
   }
}
