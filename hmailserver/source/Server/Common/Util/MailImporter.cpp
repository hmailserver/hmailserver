// Copyright (c) 2006 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "MailImporter.h"
#include "..\BO\Message.h"
#include "..\BO\MessageData.h"
#include "..\BO\MessageRecipients.h"
#include "..\Cache/CacheContainer.h"
#include "..\Util\Time.h"
#include "..\Util\Utilities.h"

#include "..\Persistence\PersistentMessage.h"

#include "Parsing\AddresslistParser.h"
#include "..\..\SMTP\RecipientParser.h"
#include "..\..\IMAP\IMAPConfiguration.h"
#include "..\..\IMAP\IMAPFolderContainer.h"
#include "../../IMAP/MessagesContainer.h"

#include "MessageUtilities.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MailImporter::MailImporter(void)
   {
   }

   MailImporter::~MailImporter(void)
   {
   }

   bool
   MailImporter::Import (const String &originalFullPath, int iAccountID, const String &sIMAPFolder)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Imports a message located in the data directory into the queue or into an account.
   // If the message is located into an account, the ID of that account must be supplied.
   // hMailServer could determine this automatically, but we're lazy.
   //---------------------------------------------------------------------------()
   {
      // Check so that the file really exists.
      if (!FileUtilities::Exists(originalFullPath))
         return false;

      // The message must be located in the data directory.
      String dataDirectory = IniFileSettings::Instance()->GetDataDirectory();

      // Retrieve the ID of the message.
      __int64 messageID;
      bool isPartialFileName = false;
      if (!PersistentMessage::GetMessageID(originalFullPath, messageID, isPartialFileName))
         return false;

      // Check so that this message isn't already referenced in the database.
      if (messageID > 0)
      {
         // The file apparently already exists. But if the full path is stored in the database,
         // we should replace that with the partial file name.
         if (isPartialFileName)
            return true;
         else
         {
            bool result = ReplaceMessagePath_(messageID, originalFullPath);
            
            if (iAccountID > 0 && result)
            {
               MessagesContainer::Instance()->UncacheAccount(iAccountID);
            }

            return result;

         }
      }

      String newFullPath = originalFullPath;

      // Construct a partial file name.
      String partialFileName;
      if (!PersistentMessage::GetPartialFilename(newFullPath, partialFileName))
      {
         // It was not possible to construct a partial file name from the full path.
         String movedFile;
         if (!MoveToNamedSubdirectory_(originalFullPath, partialFileName, newFullPath))
            return false;
      }

      if (iAccountID > 0)
      {
         // Make sure we're really placing the file into an account.
         int domainSlashPos = originalFullPath.Find(_T("\\"), dataDirectory.GetLength()+1);

         if (domainSlashPos < 0)
            return false;

         int accountSlashPos = originalFullPath.Find(_T("\\"), domainSlashPos+1);

         if (accountSlashPos < 0)
            return false;
      }
      else
      {
         // Make sure the file is placed into the data directory root.
         int invalidSlash = originalFullPath.Find(_T("\\"), dataDirectory.GetLength()+1);
         
         if (invalidSlash > 0)
         {
            // The message doesn't appear to be placed into the data directory.
            return false;
         }
      }

      std::shared_ptr<Message> pMessage = std::shared_ptr<Message> (new Message);
      pMessage->SetPartialFileName(partialFileName);

      // Set message size
      pMessage->SetSize(FileUtilities::FileSize(newFullPath));

      std::shared_ptr<MessageData> pMsgData = std::shared_ptr<MessageData>(new MessageData());
      if (!pMsgData->LoadFromMessage(newFullPath, pMessage))
         return false;

      // Check what we should set the internal date to.
      String sReceivedHeader = GetInternalDate_(pMsgData);
      pMessage->SetCreateTime(sReceivedHeader);
   
      // Parse out the From-address
      std::shared_ptr<AddresslistParser> pFromParser = std::shared_ptr<AddresslistParser>(new AddresslistParser());
      std::vector<std::shared_ptr<Address> > vecAddresses = pFromParser->ParseList(pMsgData->GetFrom());
      auto iterElement = vecAddresses.begin();

      if (iterElement != vecAddresses.end())
      {
         String sAddress = (*iterElement)->sMailboxName + "@" + (*iterElement)->sDomainName;
         pMessage->SetFromAddress(sAddress);
      }

      if (iAccountID == 0)
      {
         // Since this message is supposed to be delivered, we need
         // to parse the sender and recipients
         std::shared_ptr<AddresslistParser> pToParser = std::shared_ptr<AddresslistParser>(new AddresslistParser());
         std::vector<std::shared_ptr<Address> > vecAddresses = pToParser->ParseList(pMsgData->GetTo());
         auto iterElement = vecAddresses.begin();

         RecipientParser recipientParser;
         while (iterElement != vecAddresses.end())
         {
            String sAddress = (*iterElement)->sMailboxName + "@" + (*iterElement)->sDomainName;
            bool recipientOK = false;
            recipientParser.CreateMessageRecipientList(sAddress, pMessage->GetRecipients(), recipientOK);
            iterElement++;
         }

         // Since this message is supposed to be delivered, we need
         // to parse the sender and recipients
         pToParser = std::shared_ptr<AddresslistParser>(new AddresslistParser());
         vecAddresses = pToParser->ParseList(pMsgData->GetCC());
         iterElement = vecAddresses.begin();

         while (iterElement != vecAddresses.end())
         {
            String sAddress = (*iterElement)->sMailboxName + "@" + (*iterElement)->sDomainName;
            bool recipientOK = false;
            recipientParser.CreateMessageRecipientList(sAddress, pMessage->GetRecipients(), recipientOK);
            iterElement++;
         }

         pMessage->SetState(Message::Delivering);

         // Remove all non-local recipients to prevent duplicate email from being sent.
         pMessage->GetRecipients()->RemoveNonLocalAccounts();

         if (pMessage->GetRecipients()->GetCount() == 0)
            return false;
      }
      else
      {
         pMessage->SetAccountID(iAccountID);
         pMessage->SetState(Message::Delivered);

         // Check if we should move it to an IMAP folder
         if (!sIMAPFolder.IsEmpty())
         {
            __int64 iResultAccount = 0;
            __int64 iResultFolder = 0;
            MessageUtilities::MoveToIMAPFolder(pMessage, iAccountID, sIMAPFolder, true, true, iResultAccount, iResultFolder);
         }
         else
         {
            __int64 inboxID = CacheContainer::Instance()->GetInboxIDCache().GetUserInboxFolder(iAccountID);
            if (inboxID == 0)
               return false;

            pMessage->SetFolderID(inboxID);
         }
      }

      // Add the object to the database.
      if (!PersistentMessage::SaveObject(pMessage))
         return false;

      return true;
   }

   bool 
   MailImporter::ReplaceMessagePath_(__int64 messageID, const String &messagePath)
   {
      String newFullPath = messagePath;

      // Construct a partial file name.
      String partialFileName;
      if (!PersistentMessage::GetPartialFilename(newFullPath, partialFileName))
      {
         // It was not possible to construct a partial file name from the full path.
         String movedFile;
         if (!MoveToNamedSubdirectory_(messagePath, partialFileName, newFullPath))
            return false;
      }
      
      // We have a partial file name. Update the database with it.
      std::shared_ptr<Message> message = std::shared_ptr<Message>(new Message());
      if (!PersistentMessage::ReadObject(message, messageID))
         return false;

      message->SetPartialFileName(partialFileName);

      bool result = PersistentMessage::SaveObject(message);

      return result;
   }

   bool 
   MailImporter::MoveToNamedSubdirectory_(const String &sourceFile, String &resultFile, String &newFullPath)
   {
      // The file must be located in the data directory. Make sure this is the case.
      const String dataDirectory = IniFileSettings::Instance()->GetDataDirectory();

      if (!sourceFile.StartsWith(dataDirectory))
         return false;

      // Create a new file name.
      resultFile = Message::GenerateFileName();

      String guidFolder = resultFile.Mid(1,2);
      
      // The file could be located in an incorrect folder, not following the name standard
      // for sub folders in hMailServer. Retrieve the correct folder name so that we can
      // move the message into the correct location.
      
      String currentCorrectDirectory; 
      if (!GetRootLevelDirectory_(sourceFile, currentCorrectDirectory))
         return false;

      String destinationDirectory = FileUtilities::Combine(currentCorrectDirectory, guidFolder);
      if (!FileUtilities::Exists(destinationDirectory))
         FileUtilities::CreateDirectory(destinationDirectory);

      String destinationFile = FileUtilities::Combine(destinationDirectory, resultFile);

      if (FileUtilities::Move(sourceFile, destinationFile))
      {
         newFullPath = destinationFile;
         return true;
      }
      
      return false;
   }


   bool
   MailImporter::GetRootLevelDirectory_(const String &fullPath, String &rootLevel)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Takes an input parameter such as C:\DataDir\Account\Sub1\Sub2\Test.eml and
   // returns the root of that hierarcy, such as C:\Datadir\Account in this case.
   //---------------------------------------------------------------------------()
   {
      // The file must be located in the data directory. Make sure this is the case.
      const String dataDirectory = IniFileSettings::Instance()->GetDataDirectory();

      if (!fullPath.StartsWith(dataDirectory))
         return false;

      int currentTrimPosition = dataDirectory.GetLength() + 1;

      const String publicFolderName = IMAPConfiguration::GetPublicFolderDiskName();

      // Is the file located in the public folder?
      if (fullPath.FindNoCase(publicFolderName, currentTrimPosition) == currentTrimPosition)
      {
         // The file is located in the public folder.
         currentTrimPosition += publicFolderName.GetLength() + 1;
      }
      else
      {
         // The file is either located in the queue folder or in an account folder.
         int slashPosition = fullPath.Find(_T("\\"), currentTrimPosition);
         if (slashPosition < 0)
            return false;

         int accountSlashPosition = fullPath.Find(_T("\\"), slashPosition+1);

         if (accountSlashPosition > 0)
         {
            // The file is locate din the queue folder.
            currentTrimPosition = accountSlashPosition+1;
         }
      }

      rootLevel = fullPath.Mid(0, currentTrimPosition);
      return true;
   }

   String 
   MailImporter::GetInternalDate_(std::shared_ptr<MessageData> pMsgData)
   {

      String sReceivedHeader = pMsgData->GetFieldValue("Received");
      if (!sReceivedHeader.IsEmpty())
      {
         DateTime dtTime = Utilities::GetDateTimeFromReceivedHeader(sReceivedHeader);
         
         if (dtTime.GetYear() > 1980 && dtTime.GetYear() < 2040)
         {
            return GetInternalDate_DatePlusTimeZone(dtTime);
         }
      }        

      // We could not determine the date by looking at the Received header, check
      // the Date header insetad.
      String sDateHeader = pMsgData->GetFieldValue("Date");
      if (!sDateHeader.IsEmpty())
      {
         DateTime dtTime = Time::GetDateTimeFromMimeHeader(sDateHeader);
         
         if (dtTime.GetYear() > 1980 && dtTime.GetYear() < 2040)
         {
            return GetInternalDate_DatePlusTimeZone(dtTime);
         }
      }

      // We didn't find it in the Date header either. Use the current date.
      String sCurrentTime = Time::GetCurrentDateTime();
      return sCurrentTime;
      
   }

   String 
   MailImporter::GetInternalDate_DatePlusTimeZone(DateTime dtTime)
   {
      long minutes = Time::GetUTCRelationMinutes();

      DateTimeSpan dtSpan;
      dtSpan.SetDateTimeSpan(0, 0, minutes, 0);
      dtTime = dtTime + dtSpan;     

      String sDate = Time::GetTimeStampFromDateTime(dtTime);

      return sDate;
   }

#ifdef _DEBUG
   void
   MailImporterTester::Test()
   {
      MailImporter::Import("C:\\Program Files\\hMailServer\\Data\\{625F8A16-39BC-4282-B820-F1C270A94805}.eml", 128, "");
   }

#endif
}