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
      parent_folder_id_(iParentFolderID),
      special_use_flags_(SpecialUseNone)
   {

   }

   IMAPFolder::IMAPFolder() :
      account_id_(0),
      dbid_(0),
      current_uid_(0),
      folder_is_subscribed_(false),
      parent_folder_id_(-1),
      special_use_flags_(SpecialUseNone)
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
      // Connections share IMAPFolder instances, so the lazy creation must be synchronized.
      boost::lock_guard<boost::recursive_mutex> guard(sub_folders_mutex_);

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
      pNode->AppendAttr(_T("SpecialUse"), GetSpecialUse());
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
      SetSpecialUse(pFolderNode->GetAttrValue(_T("SpecialUse")));
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

      // Restore ACL permissions.
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
      std::vector<std::shared_ptr<IMAPFolder> > vecSubFolders = pSubFolders->GetSnapshot();
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

   bool
   IMAPFolder::IsValidSpecialUseAttribute(const String &sAttribute)
   {
      // RFC 6154 section 2. \Important (Gmail extension) is intentionally not supported.
      // \All and \Flagged are also intentionally not supported - see the comment on
      // SpecialUseFlags in IMAPFolder.h.
      return sAttribute.CompareNoCase(_T("\\Archive")) == 0 ||
             sAttribute.CompareNoCase(_T("\\Drafts")) == 0 ||
             sAttribute.CompareNoCase(_T("\\Junk")) == 0 ||
             sAttribute.CompareNoCase(_T("\\Sent")) == 0 ||
             sAttribute.CompareNoCase(_T("\\Trash")) == 0;
   }

   unsigned int
   IMAPFolder::SpecialUseStringToFlags(const String &sAttributes)
   {
      unsigned int flags = SpecialUseNone;

      std::vector<String> vecAttributes = StringParser::SplitString(sAttributes, _T(" "));
      for (const String &sAttribute : vecAttributes)
      {
         if (sAttribute.CompareNoCase(_T("\\Archive")) == 0)
            flags |= SpecialUseArchive;
         else if (sAttribute.CompareNoCase(_T("\\Drafts")) == 0)
            flags |= SpecialUseDrafts;
         else if (sAttribute.CompareNoCase(_T("\\Junk")) == 0)
            flags |= SpecialUseJunk;
         else if (sAttribute.CompareNoCase(_T("\\Sent")) == 0)
            flags |= SpecialUseSent;
         else if (sAttribute.CompareNoCase(_T("\\Trash")) == 0)
            flags |= SpecialUseTrash;
         // Unrecognized attributes (e.g. garbage from a hand-edited database) are silently dropped.
      }

      return flags;
   }

   String
   IMAPFolder::SpecialUseFlagsToString(unsigned int flags)
   {
      std::vector<String> vecAttributes;

      if (flags & SpecialUseArchive)
         vecAttributes.push_back(_T("\\Archive"));
      if (flags & SpecialUseDrafts)
         vecAttributes.push_back(_T("\\Drafts"));
      if (flags & SpecialUseJunk)
         vecAttributes.push_back(_T("\\Junk"));
      if (flags & SpecialUseSent)
         vecAttributes.push_back(_T("\\Sent"));
      if (flags & SpecialUseTrash)
         vecAttributes.push_back(_T("\\Trash"));

      return StringParser::JoinVector(vecAttributes, _T(" "));
   }

}