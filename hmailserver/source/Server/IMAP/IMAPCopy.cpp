// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCopy.h"
#include "IMAPConnection.h"
#include "IMAPCopyResult.h"
#include "IMAPRange.h"
#include "IMAPRangeParser.h"
#include "IMAPRangeMessageLocator.h"
#include "IMAPMessageInRange.h"
#include "IMAPCommand.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/Account.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/Persistence/PersistentMessage.h"
#include "IMAPSimpleCommandParser.h"
#include "../Common/BO/ACLPermission.h"
#include "../Common/Tracking/ChangeNotification.h"


#include "MessagesContainer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCopy::IMAPCopy()
   {
      
   }

   IMAPResult
   IMAPCopy::DoForMails(bool isUid, std::shared_ptr<IMAPConnection> pConnection, const String& sTag, const String& sMailNos, const String& sTargetFolder)
   {
      // We should check if the folder exists.
      std::shared_ptr<IMAPFolder> pFolder = pConnection->GetFolderByFullPath(sTargetFolder);

      if (!pFolder)
         return IMAPResult(IMAPResult::ResultBad, "The folder could not be found.");
      
      // Locate messages to copy
      IMAPRangeParser rangeParser;
      auto ranges = rangeParser.Parse(sMailNos);
      auto messages = pConnection->GetCurrentFolder()->GetMessages();

      IMAPRangeMessageLocator rangeMessageLocator;
      auto messagesInRange = rangeMessageLocator.GetMessageInRanges(messages, isUid, ranges);

      std::vector<unsigned int> sourceUids;
      std::vector<unsigned int> targetUids;

      for (auto messageInRange : messagesInRange)
      {
         auto oldMessage = messageInRange.GetMessage();

         unsigned int targetUid;
         IMAPResult result = CopyEmail_(pConnection, oldMessage, pFolder, targetUid);
         if (result.GetResult() != IMAPResult::ResultOK)
            return result;

         sourceUids.push_back(oldMessage->GetUID());
         targetUids.push_back(targetUid);
      }

      String sourceUidsStr = StringParser::JoinVector(sourceUids, ",");
      String targetUidsStr = StringParser::JoinVector(targetUids, ",");

      // Append COPYUID response, but only if the user has vaccess to read the target folder.
      // From RFC 4315:
      //   The COPYUID and APPENDUID response codes return information about the
      //   mailbox, which may be considered sensitive if the mailbox has
      //   permissions set that permit the client to COPY or APPEND to the
      //   mailbox, but not SELECT or EXAMINE it.
      auto hasReadPermissions = pConnection->CheckPermission(pFolder, ACLPermission::PermissionRead);

      String sCommandOkString;

      if (!sourceUids.empty() && hasReadPermissions)
      {
         sCommandOkString.Format(_T("%s OK [COPYUID %d %s %s] COPY completed\r\n"), sTag.c_str(), pFolder->GetCreationTime().ToInt(), sourceUidsStr.c_str(), targetUidsStr.c_str());
      }
      else
      {
         sCommandOkString.Format(_T("%s OK COPY completed\r\n"), sTag.c_str());
      }

      pConnection->SendAsciiData(sCommandOkString);

      return IMAPResult();
   }

   IMAPResult
   IMAPCopy::CopyEmail_(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<Message> pOldMessage, const std::shared_ptr<IMAPFolder> pFolder, unsigned int &targetUid)
   {
      targetUid = 0;

      if (!pOldMessage)
         return IMAPResult(IMAPResult::ResultBad, "Invalid parameters");
      

      std::shared_ptr<const Account> pAccount = pConnection->GetAccount();

      if (!pFolder->IsPublicFolder())
      {
         if (!pAccount->SpaceAvailable(pOldMessage->GetSize()))
            return IMAPResult(IMAPResult::ResultNo, "Your quota has been exceeded.");
      }

      // Check if the user has permission to copy to this destination folder
      if (!pConnection->CheckPermission(pFolder, ACLPermission::PermissionInsert))
         return IMAPResult(IMAPResult::ResultBad, "ACL: Insert permission denied (Required for COPY command).");

      std::shared_ptr<Message> pNewMessage = PersistentMessage::CopyToIMAPFolder(pAccount, pOldMessage, pFolder);

      if (!pNewMessage)
         return IMAPResult(IMAPResult::ResultBad, "Failed to copy message");

      // Check if the user has access to set the Seen flag, otherwise 
      if (!pConnection->CheckPermission(pFolder, ACLPermission::PermissionWriteSeen))
         pNewMessage->SetFlagSeen(false);  

      if (!PersistentMessage::SaveObject(pNewMessage))
         return IMAPResult(IMAPResult::ResultBad, "Failed to save copy of message.");

      MessagesContainer::Instance()->SetFolderNeedsRefresh(pFolder->GetID());

      // Set a delayed notification so that the any IMAP idle client is notified when this
      // command has been finished.
      std::shared_ptr<ChangeNotification> pNotification = 
         std::shared_ptr<ChangeNotification>(new ChangeNotification(pFolder->GetAccountID(), pFolder->GetID(), ChangeNotification::NotificationMessageAdded));

      pConnection->SetDelayedChangeNotification(pNotification);

      targetUid = pNewMessage->GetUID();

      return IMAPResult();
   }

   shared_ptr<IMAPFolder>
   IMAPCopy::GetTargetFolder_(bool isUid, std::shared_ptr<IMAPSimpleCommandParser> pParser, std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      auto wordIndex = isUid ? 1 : 0;

      String sFolderName;
      if (pParser->Word(wordIndex)->Clammerized())
         sFolderName = pArgument->Literal(0);
      else
      {
         sFolderName = pParser->Word(wordIndex)->Value();
         IMAPFolder::UnescapeFolderString(sFolderName);
      }

      std::shared_ptr<IMAPFolder> pFolder = pConnection->GetFolderByFullPath(sFolderName);

      return pFolder;
   }
}
