// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPFolder.h"
#include "IMAPFolders.h"

#include "Message.h"
#include "../Persistence/PersistentMessage.h"
#include "../BO/ACLPermissions.h"
#include "../Util/Time.h"

#include "../../IMAP/MessagesContainer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{


   IMAPFolder::IMAPFolder(__int64 iAccountID, __int64 iParentFolderID) :
      account_id_(iAccountID), 
      dbid_(0),
      current_uid_(0),
      folder_is_subscribed_(false),
      parent_folder_id_(iParentFolderID)
   {
      
   }

   IMAPFolder::IMAPFolder() :
      account_id_(0), 
      dbid_(0),
      current_uid_(0),
      folder_is_subscribed_(false),
      parent_folder_id_(-1)
   {

   }

   IMAPFolder::~IMAPFolder()
   {

   }

   __int64
   IMAPFolder::GetParentFolderID() const
   {
      return parent_folder_id_;
   }

   std::shared_ptr<Messages>
   IMAPFolder::GetMessages()
   {
      return MessagesContainer::Instance()->GetMessages(account_id_, dbid_);
   }

   std::shared_ptr<IMAPFolders>
   IMAPFolder::GetSubFolders()
   {
      if (sub_folders_.get() == NULL)
         sub_folders_ = std::shared_ptr<IMAPFolders>(new IMAPFolders(account_id_, dbid_));

      return sub_folders_;
   }


   std::shared_ptr<ACLPermissions>
   IMAPFolder::GetPermissions()
   {
      // Always return a new one. Hopefully we don't have so many public folders
	  // that this will become a performance issue.
      std::shared_ptr<ACLPermissions> pPermissions = std::shared_ptr<ACLPermissions>(new ACLPermissions(dbid_));
      
	  // No point in loading list of permissions for account level folder. 
	  // (since account level folders never have permissions set)
	  if (IsPublicFolder())
	  {
         pPermissions->Refresh();
	  }

      return pPermissions;
   }


   void 
   IMAPFolder::UnescapeFolderString(String &sFolderString)
   {
      sFolderString.Replace(_T("\\\""), _T("\""));
      sFolderString.Replace(_T("\\\\"), _T("\\"));
   }
   
   void 
   IMAPFolder::EscapeFolderString(String &sFolderString)
   {
      int iCurPos = 0;
      int iInLength = sFolderString.GetLength();
      
      String sOut;

      while (iCurPos < iInLength)
      {
         wchar_t sCurChar = sFolderString.GetAt(iCurPos);
         
         if (sCurChar == '\"' ||
             sCurChar == '\\')
         {
            // We have found a special character that needs to be escaped.
            sOut += "\\";
         }

         sOut += sCurChar;
         
         iCurPos++;
      }

      sFolderString = sOut;
   }

   bool 
   IMAPFolder::XMLStore(XNode *pParentNode, int iBackupOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Folder"));
      pNode->AppendAttr(_T("Name"), String(folder_name_));
      pNode->AppendAttr(_T("Subscribed"), folder_is_subscribed_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("CreateTime"), String(Time::GetTimeStampFromDateTime(create_time_)));
      pNode->AppendAttr(_T("CurrentUID"), StringParser::IntToString(current_uid_));

      if (!GetMessages()->XMLStore(pNode, iBackupOptions))
         return false;

      if (!GetSubFolders()->XMLStore(pNode, iBackupOptions))
         return false;

      if (IsPublicFolder())
      {
         if (!GetPermissions()->XMLStore(pNode, iBackupOptions))
            return false;
      }

      return true;
   }

   

   bool 
   IMAPFolder::XMLLoad(XNode *pFolderNode, int iRestoreOptions)
   {
      folder_name_ = pFolderNode->GetAttrValue(_T("Name"));
      folder_is_subscribed_ = pFolderNode->GetAttrValue(_T("Subscribed")) == _T("1");
      create_time_ = Time::GetDateFromSystemDate(pFolderNode->GetAttrValue(_T("CreateTime")));
      current_uid_ = _ttoi(pFolderNode->GetAttrValue(_T("CurrentUID")));

      return true;
   }

   bool 
   IMAPFolder::XMLLoadSubItems(XNode *pFolderNode, int iRestoreOptions)
   {
      // Should we restore messages as well?
      if (!GetMessages()->XMLLoad(pFolderNode, iRestoreOptions))
         return false;

      // Load sub folders.
      if (!GetSubFolders()->XMLLoad(pFolderNode, iRestoreOptions))
         return false;

      // Restore ACL permisisons.
      if (IsPublicFolder())
      {
         if (!GetPermissions()->XMLLoad(pFolderNode, iRestoreOptions))
            return false;
      }

      return true;
   }

   bool 
   IMAPFolder::IsValidFolderName(const std::vector<String> &vecPath, bool bIsPublicFolder)
   {
      if (vecPath.size() > IMAPFolder::MaxFolderDepth)
         return false;

      if (vecPath.size() < 1)
         return false;

      if (!bIsPublicFolder)
      {
         String s = vecPath[0];

         if (s.Left(1) == _T("#"))
            return false;
      }

      std::vector<String>::const_iterator iter = vecPath.begin();
      std::vector<String>::const_iterator iterEnd = vecPath.end();
      for (; iter != iterEnd; iter++)
      {
         String s = (*iter);
         
         if (s.GetLength() == 0 || s.GetLength() > 255) 
            return false;
      }

      return true;
   }
   
   int
   IMAPFolder::GetFolderDepth(int &iRecursion)
   {
      iRecursion++;

      if (iRecursion > 100)
      {
         String sMessage = Formatter::Format("Excessive folder recursion. Giving up. Account: {0}, Folder: {1}", account_id_, folder_name_);
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5019, "IMAPFolder::GetFolderDepth", sMessage);

         return 0;
      }

      int iDepth = 1;
      
      std::shared_ptr<IMAPFolders> pSubFolders = GetSubFolders();
      std::vector<std::shared_ptr<IMAPFolder> > vecSubFolders = pSubFolders->GetVector();
      auto iterCurFolder = vecSubFolders.begin();

      int iSubDepth = 0;
      int iMaxSubDepth = 0;
      while (iterCurFolder != vecSubFolders.end())
      {
         std::shared_ptr<IMAPFolder> pFolder = (*iterCurFolder);

         iSubDepth = pFolder->GetFolderDepth(iRecursion);

         if (iSubDepth > iMaxSubDepth)
            iMaxSubDepth = iSubDepth;

         iterCurFolder++;
      } 

      iRecursion--;

      iDepth = iDepth + iMaxSubDepth; 

      return iDepth;
   }

   bool 
   IMAPFolder::IsPublicFolder()
   {
      return account_id_ == 0;
   }

}