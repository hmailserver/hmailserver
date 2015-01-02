// Copyright (c) 2006 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "MessageUtilities.h"

#include "../BO/Message.h"
#include "../BO/Account.h"
#include "../BO/IMAPFolders.h"

#include "../Mime/Mime.h"

#include "../Util/Utilities.h"

#include "../Cache/CacheContainer.h"
#include "../../IMAP/IMAPConfiguration.h"
#include "../../SMTP/SMTPConfiguration.h"
#include "../../IMAP/IMAPFolderContainer.h"
#include "../../IMAP/IMAPFolderUtilities.h"
#include "../Tracking/ChangeNotification.h"
#include "../Tracking/NotificationServer.h"


#include "../BO/ACLPermission.h"
#include "../Application/ACLManager.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   MessageUtilities::MessageUtilities()
   {
      
   }

   MessageUtilities::~MessageUtilities()
   {

   }

	bool
	MessageUtilities::MoveToIMAPFolder(std::shared_ptr<Message> pMessage, __int64 iAccountID, const String &sFolderName, bool bAutoSubscribe, bool bSetByGlobalRule, __int64 &iResultAccount, __int64 &iResultFolder)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Moves a message to an IMAP folder. The message should not be saved when this
   // function is called.
   // Returns the ID of the folder the message was moved to.
   //---------------------------------------------------------------------------()
	{
      LOG_DEBUG("Moving message to " + sFolderName);

      // Set default values in case we fail to move the message to another folder later on.
      iResultAccount = iAccountID;
      iResultFolder = 0;
      
      String sTempFolderName = sFolderName;

      // Start of by cleaning the string:
      CleanIMAPFolderRuleString(sTempFolderName);

      std::vector<String> vecFolderPath = StringParser::SplitString(sTempFolderName, Configuration::Instance()->GetIMAPConfiguration()->GetHierarchyDelimiter());

      std::shared_ptr<IMAPFolders> pFolders;

      bool isPublicFolder = IMAPFolderUtilities::IsPublicFolder(vecFolderPath);

      if (isPublicFolder)
      {
         pFolders = IMAPFolderContainer::Instance()->GetPublicFolders();
         vecFolderPath.erase(vecFolderPath.begin());
      }
      else
         pFolders = IMAPFolderContainer::Instance()->GetFoldersForAccount(iAccountID);

      // Check if this folder exist
      std::shared_ptr<IMAPFolder> pFolder = pFolders->GetFolderByFullPath(vecFolderPath);

      if (pFolder && pFolder->IsPublicFolder())
      {
         // Do we have permissions to append?
		   ACLManager aclManager;
         std::shared_ptr<ACLPermission> pPermission = aclManager.GetPermissionForFolder(iAccountID, pFolder);
         if (!pPermission)
            return false;

         if (!pPermission->GetAllow((ACLPermission::ePermission) ACLPermission::PermissionInsert))
            return false;
      }

      if (!pFolder)
      {
         // The folder does not exist. Are we allowed to create it?
         if (isPublicFolder)
         {
            if (!bSetByGlobalRule)
            {
               // iterate over folders until we find an already existing one.
               std::shared_ptr<IMAPFolders> pPublicFolders = Configuration::Instance()->GetIMAPConfiguration()->GetPublicFolders();
               std::shared_ptr<IMAPFolder> pTempFolder = IMAPFolderUtilities::GetTopMostExistingFolder(pPublicFolders, vecFolderPath);
               
               if (!pTempFolder)
               {
                  // User doesn't have permissions to create this folder.
                  return false;
               }

               // Do we have permissions to append?
               ACLManager aclManager;
			      std::shared_ptr<ACLPermission> pPermission = aclManager.GetPermissionForFolder(iAccountID, pTempFolder);

               if (!pPermission)
                  return false;

               if (!pPermission->GetAllow((ACLPermission::ePermission) ACLPermission::PermissionCreate))
                  return false;
            }
         }

         pFolders->CreatePath(pFolders, vecFolderPath, bAutoSubscribe );

         // Fetch the newly created folder.
         pFolder = pFolders->GetFolderByFullPath(vecFolderPath);

         if (!pFolder)
         {
            // Something is fishy. If the folder didn't exist, it
            // should be created now. 
            String sErrorMessage;
            sErrorMessage.Format(_T("Failed to locate folder. Account id: %d Path: %s"), iAccountID, sFolderName.c_str());
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5053, "MessageUtilities::MoveToIMAPFolder", sErrorMessage);

            iResultAccount = iAccountID;
            iResultFolder = 0;
            return false;
         }

      }  
      
      // Connect the message to the folder.
      pMessage->SetFolderID(pFolder->GetID());
      iResultAccount = pFolder->GetAccountID();
      iResultFolder = pFolder->GetID();

      LOG_DEBUG("Message moved to folder " + StringParser::IntToString(iResultFolder));

      return true;
	}

   void
   MessageUtilities::CleanIMAPFolderRuleString(String &sIMAPFolder)
   {
      SYSTEMTIME pTime;
      GetLocalTime(&pTime);

      String sYear;
      String sMonth;
      String sDay;

      sYear.Format(_T("%.04d"), pTime.wYear);
      sMonth.Format(_T("%.02d"), pTime.wMonth);
      sDay.Format(_T("%.02d"), pTime.wDay);

      sIMAPFolder.ReplaceNoCase(_T("%YEAR%"), sYear);
      sIMAPFolder.ReplaceNoCase(_T("%MONTH%"), sMonth);
      sIMAPFolder.ReplaceNoCase(_T("%DAY%"), sDay);
      if (sIMAPFolder.Left(1) == Configuration::Instance()->GetIMAPConfiguration()->GetHierarchyDelimiter())
         sIMAPFolder = sIMAPFolder.Mid(1);
   }

   bool 
   MessageUtilities::CopyToIMAPFolder(std::shared_ptr<Message> pMessage, int iDestinationFolderID)
   {
      // Check if the destination folder exists
      std::shared_ptr<IMAPFolders> pFolders = HM::IMAPFolderContainer::Instance()->GetFoldersForAccount(pMessage->GetAccountID());
      std::shared_ptr<IMAPFolder> pFolder = pFolders->GetItemByDBIDRecursive(iDestinationFolderID);

      if (!pFolder)
         return false;

      // Check which account this message belongs to.
      std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(pMessage->GetAccountID());
      if (!pAccount)
         return false;

      std::shared_ptr<Message> pNewMessage = PersistentMessage::CopyToIMAPFolder(pAccount, pMessage, pFolder);
      if (!pNewMessage)
         return false;

      PersistentMessage::SaveObject(pNewMessage);

      pFolder->GetMessages()->Refresh(false);

      std::shared_ptr<ChangeNotification> pNotification = 
         std::shared_ptr<ChangeNotification>(new ChangeNotification(pNewMessage->GetAccountID(), pNewMessage->GetFolderID(), ChangeNotification::NotificationMessageAdded));

      Application::Instance()->GetNotificationServer()->SendNotification(pNotification);

      return true;
   }

   bool
   MessageUtilities::RetrieveOriginatingAddress(std::shared_ptr<Message> pMessage, String &hostName, IPAddress &address)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Tries to determine the IP address / host this email originally comes from.
   //---------------------------------------------------------------------------()
   {
      hostName = "";
      address = IPAddress();

      // Extract Received headers from the message.
      std::shared_ptr<MimeHeader> pHeader = GetMessageHeader_(pMessage);

      std::list<String> receivedHeaders;

      AnsiString sHeaderName = "Received";
      std::vector<MimeField> &lstFields = pHeader->Fields();
      auto iter = lstFields.begin();
      auto iterEnd = lstFields.end();
   
      for (; iter != iterEnd; iter++)
      {
         MimeField& fd = *iter;
         
         if (sHeaderName.CompareNoCase(fd.GetName()) == 0)
         {
            receivedHeaders.push_back(fd.GetValue());
         }
      }

      return RetrieveOriginatingAddress(receivedHeaders, hostName, address);
   }

   bool
   MessageUtilities::RetrieveOriginatingAddress(const std::list<String> &receivedHeaders, String &hostName, IPAddress &address)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Tries to determine the IP address this email originally comes from.
   //---------------------------------------------------------------------------()
   {
      hostName = "";
      address = IPAddress();

      std::list<std::pair<String, IPAddress> > addresses;
      RetrieveReceivedIPList_(receivedHeaders, addresses);

      if (addresses.empty())
         return false;

      std::list<std::pair<String, IPAddress>>::const_iterator iter = addresses.begin();
      std::list<std::pair<String, IPAddress>>::const_iterator iterEnd = addresses.end();

      std::shared_ptr<IncomingRelays> incomingRelays = Configuration::Instance()->GetSMTPConfiguration()->GetIncomingRelays();

      for (; iter != iterEnd; iter++)
      {
         std::pair<String, IPAddress> item = (*iter);

         if (!incomingRelays->IsIncomingRelay(item.second))
         {
            // We've found the IP address.
            hostName = item.first;
            address = item.second;
            return true;
         }
      }

      return false;
   }

   String 
   MessageUtilities::GetSendersIP(std::shared_ptr<Message> pMessage)
   {
      const String fileName = PersistentMessage::GetFileName(pMessage);

      AnsiString sHeader = PersistentMessage::LoadHeader(fileName);

      MimeHeader oHeader;
      oHeader.Load(sHeader, sHeader.GetLength(), true);

      // Locate the first Received header
      MimeField *pReceivedHeader = oHeader.GetField("Received");
      if (pReceivedHeader == 0)
         return "127.0.0.1";

      // Now we should try to find the IP in the received header.
      String sReceivedValue = pReceivedHeader->GetValue();

      int iAddressStart = sReceivedValue.Find(_T("[")) +1;
      int iAddressEnd = sReceivedValue.Find(_T("]"), iAddressStart);
      int iAddressLen = iAddressEnd - iAddressStart;

      if (iAddressLen <= 0)
         return "127.0.0.1";

      String sIPAddress = sReceivedValue.Mid(iAddressStart, iAddressLen);

      if (!StringParser::IsValidIPAddress(sIPAddress))
         return "127.0.0.1";

      return sIPAddress;
   }

   std::shared_ptr<MimeHeader> 
   MessageUtilities::GetMessageHeader_(std::shared_ptr<Message> pMessage)
   {
      String fileName = PersistentMessage::GetFileName(pMessage);

      AnsiString sHeader = PersistentMessage::LoadHeader(fileName);
      std::shared_ptr<MimeHeader> pHeader = std::shared_ptr<MimeHeader>(new MimeHeader());
      
      std::shared_ptr<MimeHeader> pMimeHeader = std::shared_ptr<MimeHeader>(new MimeHeader);
      pHeader->Load(sHeader, sHeader.GetLength(), true);

      return pHeader;
   }

   void 
   MessageUtilities::RetrieveReceivedIPList_(const std::list<String> &headers, std::list<std::pair<String, IPAddress> > &vecAddresses)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Goes throguh all the "Received" headers of the email and returns the list
   // of IP addresses.
   //---------------------------------------------------------------------------()
   {
      IPAddress emptyAddress;

      for(String header : headers)
      {
         String hostName = Utilities::GetHostNameFromReceivedHeader(header);
         IPAddress ipaddress = Utilities::GetIPAddressFromReceivedHeader(header);

         if (hostName == _T("") || ipaddress == emptyAddress)
            continue;

         vecAddresses.push_back(std::make_pair(hostName, ipaddress));
      }
   }

   void MessageUtilitiesTester::Test()
   {
      std::list<String> headerLines;
      headerLines.push_back("from host.edu (host.edu [1.2.3.4]) by mail.host.edu (8.8.5) id 004A21; Tue, Mar 18 1997 14:36:17 -0800 (PST)\r\n");
      headerLines.push_back("from someone.google (someone.google [1.1.1.1]) by mail.host.edu (8.8.5) id 004A21; Tue, Mar 18 1997 14:36:17 -0800 (PST)\r\n");

      String hostName;
      IPAddress address;

      bool result = MessageUtilities::RetrieveOriginatingAddress(headerLines, hostName, address);
      
      if (result == false)
         throw;

      if (hostName != _T("host.edu"))
         throw;

      if (address.ToString() != "1.2.3.4")
         throw;

      if (address.IsAny())
         throw;

      // If the message
      headerLines.clear();
      headerLines.push_back("by 10.103.12.12 with SMTP id p12cs158601mui; Mon, 14 Dec 2009 08:53:48 -0800 (PST)");
      headerLines.push_back("by 10.224.63.1 with SMTP id z1mr3063080qah.314.1260809627613; Mon, 14 Dec 2009 08:53:47 -0800 (PST)");
      headerLines.push_back("from e113.en25.com (e113.en25.com [209.167.231.113]) by mx.google.com with ESMTP id 9si6782547iwn.6.2009.12.14.08.53.46; Mon, 14 Dec 2009 08:53:47 -0800 (PST)");

      result = MessageUtilities::RetrieveOriginatingAddress(headerLines, hostName, address);

      if (result == false)
         throw;

      if (hostName != _T("e113.en25.com"))
         throw;

      if (address.ToString() != "209.167.231.113")
         throw;


      if (address.IsAny())
         throw;


      // We are unable to parse this because the IP address isn't within brackets ([]).
      headerLines.clear();
      headerLines.push_back("(qmail 25533 invoked from network); 19 Jan 2010 20:06:21 +0100");
      headerLines.push_back("from ded943.maximumasp.com (HELO mail.hmailserver.com) (216.128.26.134)");

      result = MessageUtilities::RetrieveOriginatingAddress(headerLines, hostName, address);

      if (result == true)
         throw;

      if (hostName != _T(""))
         throw;

      if (address.ToString() != "0.0.0.0")
         throw;

      if (!address.IsAny())
         throw;
   }

}
