// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandAppend.h"
#include "IMAPConnection.h"
#include "../Common/BO/Account.h"
#include "../Common/BO/ACLPermission.h"
#include "../Common/BO/Domain.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/IMAPFolders.h"
#include "../Common/BO/Message.h"
#include "../Common/Cache/CacheContainer.h"
#include "../Common/Persistence/PersistentMessage.h"
#include "../Common/Tracking/ChangeNotification.h"
#include "../Common/Tracking/NotificationServer.h"
#include "../Common/Util/ByteBuffer.h"
#include "../Common/Util/File.h"
#include "../Common/Util/Time.h"
#include "../SMTP/SMTPConfiguration.h"

#include "IMAPSimpleCommandParser.h"
#include "MessagesContainer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   IMAPCommandAppend::IMAPCommandAppend() :
      bytes_left_to_receive_(0)
   {
   }

   IMAPCommandAppend::~IMAPCommandAppend()
   {
      try
      {
         KillCurrentMessage_();
      }
      catch (...)
      {

      }
   }

   void 
   IMAPCommandAppend::KillCurrentMessage_()
   {
      if (!current_message_)
         return;

      if (FileUtilities::Exists(message_file_name_))
         FileUtilities::DeleteFile(message_file_name_);
   }

   IMAPResult
   IMAPCommandAppend::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");
      
      current_tag_ = pArgument->Tag();
      
      // Reset these two so we don't re-use old values.
      flags_to_set_ = "";
      create_time_to_set_ = "";

      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());

      pParser->Parse(pArgument);

      if (pParser->WordCount() < 3)
         return IMAPResult(IMAPResult::ResultBad, "APPEND Command requires at least 2 parameter.");
      
         // Create a new mailbox
      String sFolderName = pParser->GetParamValue(pArgument, 0);
      if (!pParser->Word(1)->Clammerized())
         IMAPFolder::UnescapeFolderString(sFolderName);
     
      if (pParser->ParantheziedWord())
         flags_to_set_ = pParser->ParantheziedWord()->Value();

      // last word.
      std::shared_ptr<IMAPSimpleWord> pWord = pParser->Word(pParser->WordCount()-1);

      if (!pWord || !pWord->Clammerized())
         return IMAPResult(IMAPResult::ResultBad, "Missing literal");

      AnsiString literalSize = pWord->Value();
       
      bytes_left_to_receive_ = atoi(literalSize);
      if (bytes_left_to_receive_ == 0)
         return IMAPResult(IMAPResult::ResultBad, "Empty message not permitted.");
      
      // Add an extra two bytes since we expect a <newline> in the end.
      bytes_left_to_receive_ += 2;

      std::shared_ptr<const Domain> domain = CacheContainer::Instance()->GetDomain(pConnection->GetAccount()->GetDomainID());
      size_t maxMessageSizeKB = GetMaxMessageSize_(domain);

      if (maxMessageSizeKB > 0 && 
          bytes_left_to_receive_ / 1024 > maxMessageSizeKB)
      {
         String sMessage;
         sMessage.Format(_T("Message size exceeds fixed maximum message size. Size: %d KB, Max size: %d KB"), 
            bytes_left_to_receive_ / 1024, maxMessageSizeKB);

         return IMAPResult(IMAPResult::ResultNo, sMessage);
      }

      // Locate the parameter containing the date to set.
      // Can't use pParser->QuotedWord() since there may
      // be many quoted words in the command.
      
      for (size_t i = 2; i < pParser->WordCount(); i++)
      {
         std::shared_ptr<IMAPSimpleWord> pWord = pParser->Word(i);

         if (pWord->Quoted())
         {
            create_time_to_set_ = pWord->Value();

            // date-day-fixed  = (SP DIGIT) / 2DIGIT
            //   ; Fixed-format version of date-day
            // If the date given starts with <space>number, we need
            // to Trim. Doesn't hurt to always do this.
            create_time_to_set_.TrimLeft();
         }
      }

      destination_folder_ = pConnection->GetFolderByFullPath(sFolderName);
      if (!destination_folder_)
         return IMAPResult(IMAPResult::ResultNo, "[TRYCREATE] Folder could not be found.");

      if (!destination_folder_->IsPublicFolder())
      {
         // Make sure that this message fits in the mailbox.
         std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(pConnection->GetAccount()->GetID());
         
         if (!pAccount)
            return IMAPResult(IMAPResult::ResultNo, "Account could not be fetched.");

         if (!pAccount->SpaceAvailable(bytes_left_to_receive_))
            return IMAPResult(IMAPResult::ResultNo, "Your quota has been exceeded.");
      }

      if (!pConnection->CheckPermission(destination_folder_, ACLPermission::PermissionInsert))
         return IMAPResult(IMAPResult::ResultBad, "ACL: Insert permission denied (Required for APPEND command).");



      __int64 lFolderID = destination_folder_->GetID();

      current_message_ = std::shared_ptr<Message>(new Message);
      current_message_->SetAccountID(destination_folder_->GetAccountID());
      current_message_->SetFolderID(lFolderID);

      // Construct a file name which we'll write the message to.
      // Should we connect this message to an account? Yes, if this is not a public folder.
      std::shared_ptr<const Account> pMessageOwner;
      if (!destination_folder_->IsPublicFolder())
         pMessageOwner = pConnection->GetAccount();

      message_file_name_ = PersistentMessage::GetFileName(pMessageOwner, current_message_);

      String sResponse = "+ Ready for literal data\r\n";
      pConnection->SetReceiveBinary(true);
      pConnection->SendAsciiData(sResponse);    

      return IMAPResult();
   }

   void
   IMAPCommandAppend::ParseBinary(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<ByteBuffer> pBuf)
   {
      append_buffer_.Add(pBuf);
   
      if (append_buffer_.GetSize() >= bytes_left_to_receive_)
      {
         WriteData_(pConnection, append_buffer_.GetBuffer(), append_buffer_.GetSize());

         pConnection->SetReceiveBinary(false);
   
         append_buffer_.Empty();

         Finish_(pConnection);

         pConnection->EnqueueRead();
      }
      else
      {
         TruncateBuffer_(pConnection);

         pConnection->EnqueueRead("");
      }

   }
   
   bool
   IMAPCommandAppend::WriteData_(const std::shared_ptr<IMAPConnection>  pConn, const BYTE *pBuf, size_t WriteLen)
   {
      if (!current_message_)
         return false;

      String destinationPath = FileUtilities::GetFilePath(message_file_name_);
      if (!FileUtilities::Exists(destinationPath))
         FileUtilities::CreateDirectory(destinationPath);

      File oFile;
      
      try
      {
         oFile.Open(message_file_name_, File::OTAppend);


         oFile.Write(pBuf, WriteLen);
      }
      catch (...)
      {
         return false;
      }

      return true;
   }

   bool
   IMAPCommandAppend::TruncateBuffer_(const std::shared_ptr<IMAPConnection> pConn)
   {
      if (append_buffer_.GetSize() >= 20000)
      {
         WriteData_(pConn, append_buffer_.GetBuffer(), append_buffer_.GetSize());
         bytes_left_to_receive_ -= append_buffer_.GetSize();
         append_buffer_.Empty();
      }

      return true;

   }

   void
   IMAPCommandAppend::Finish_(std::shared_ptr<IMAPConnection> pConnection)
   {
      if (!current_message_)
         return;

      // Add this message to the folder.
      current_message_->SetSize(FileUtilities::FileSize(message_file_name_));
      current_message_->SetState(Message::Delivered);

      // Set message flags.
      bool bSeen = (flags_to_set_.FindNoCase(_T("\\Seen")) >= 0);
      bool bDeleted = (flags_to_set_.FindNoCase(_T("\\Deleted")) >= 0);
      bool bDraft = (flags_to_set_.FindNoCase(_T("\\Draft")) >= 0);
      bool bAnswered = (flags_to_set_.FindNoCase(_T("\\Answered")) >= 0);
      bool bFlagged = (flags_to_set_.FindNoCase(_T("\\Flagged")) >= 0);
      
      if (bSeen)
      {
         // ACL: If user tries to set the Seen flag, check that he has permission to do so.
         if (!pConnection->CheckPermission(destination_folder_, ACLPermission::PermissionWriteSeen))
         {
            // User does not have permission to set the Seen flag. 
            bSeen = false;
         }
      }

      current_message_->SetFlagDeleted(bDeleted);
      current_message_->SetFlagSeen(bSeen);
      current_message_->SetFlagDraft(bDraft);
      current_message_->SetFlagAnswered(bAnswered);
      current_message_->SetFlagFlagged(bFlagged);
    
         
      // Set the create time
      if (!create_time_to_set_.IsEmpty())
      {
         // Convert to internal format
         create_time_to_set_ = Time::GetInternalDateFromIMAPInternalDate(create_time_to_set_);
         current_message_->SetCreateTime(create_time_to_set_);
      }

      PersistentMessage::SaveObject(current_message_);

      pConnection->GetRecentMessages().insert(current_message_->GetID());

      MessagesContainer::Instance()->SetFolderNeedsRefresh(destination_folder_->GetID());



      String sResponse;
      if (pConnection->GetCurrentFolder() &&
          pConnection->GetCurrentFolder()->GetID() == destination_folder_->GetID())
      {
         std::shared_ptr<Messages> messages = destination_folder_->GetMessages();
         sResponse += IMAPNotificationClient::GenerateExistsString(messages->GetCount());
         sResponse += IMAPNotificationClient::GenerateRecentString((int) pConnection->GetRecentMessages().size());
      }

      // Send the OK response to the client.
      sResponse += current_tag_ + " OK APPEND completed\r\n";
      pConnection->SendAsciiData(sResponse);

      // Notify the mailbox notifier that the mailbox contents have changed. 
      std::shared_ptr<ChangeNotification> pNotification = 
         std::shared_ptr<ChangeNotification>(new ChangeNotification(destination_folder_->GetAccountID(), destination_folder_->GetID(), ChangeNotification::NotificationMessageAdded));
      Application::Instance()->GetNotificationServer()->SendNotification(pConnection->GetNotificationClient(), pNotification);

      destination_folder_.reset();
      current_message_.reset();
   }

   int 
   IMAPCommandAppend::GetMaxMessageSize_(std::shared_ptr<const Domain> pDomain)
   {
      int iMaxMessageSizeKB = Configuration::Instance()->GetSMTPConfiguration()->GetMaxMessageSize();

      if (pDomain)
      {
         int iDomainMaxSizeKB = pDomain->GetMaxMessageSize(); 
         if (iDomainMaxSizeKB > 0)
         {
            if (iMaxMessageSizeKB == 0 || iMaxMessageSizeKB > iDomainMaxSizeKB)
               iMaxMessageSizeKB = iDomainMaxSizeKB;
         }
      }

      return iMaxMessageSizeKB;
   }

}
