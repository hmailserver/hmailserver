// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPFolder.h"
#include "IMAPFolders.h"

#include "Message.h"
#include "../Persistence/PersistentMessage.h"
#include "../BO/ACLPermissions.h"
#include "../Util/Time.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{


   IMAPFolder::IMAPFolder(__int64 iAccountID, __int64 iParentFolderID) :
      m_iAccountID(iAccountID), 
      m_iDBID(0),
      _currentUID(0),
      m_bFolderIsSubscribed(false),
      m_bFolderNeedsRefresh(true),
      m_iParentFolderID(iParentFolderID)
   {
      
   }

   IMAPFolder::IMAPFolder() :
      m_iAccountID(0), 
      m_iDBID(0),
      m_bFolderIsSubscribed(false),
      m_bFolderNeedsRefresh(true),
      m_iParentFolderID(-1)
   {

   }

   IMAPFolder::~IMAPFolder()
   {

   }

   __int64
   IMAPFolder::GetParentFolderID() const
   {
      return m_iParentFolderID;
   }

   shared_ptr<Messages>
   IMAPFolder::GetMessages(bool bReloadIfNeeded)
   {
      if (m_oMessages.get() == NULL)
      {
         m_oMessages = shared_ptr<Messages>(new Messages(m_iAccountID, m_iDBID));
         m_bFolderNeedsRefresh = true;      
      }

      if (m_bFolderNeedsRefresh && bReloadIfNeeded)
      {
         m_bFolderNeedsRefresh = false;
         m_oMessages->Refresh();
      }

      return m_oMessages;
   }

   std::vector<shared_ptr<Message>>
   IMAPFolder::GetMessagesCopy(bool bReloadIfNeeded)
   {
      shared_ptr<Messages> messages = GetMessages(bReloadIfNeeded);
      
      return messages->GetCopy();
   }

   void
   IMAPFolder::SetFolderNeedsRefresh()
   {
      m_bFolderNeedsRefresh = true; 
   }

   shared_ptr<IMAPFolders>
   IMAPFolder::GetSubFolders()
   {
      if (m_oSubFolders.get() == NULL)
         m_oSubFolders = shared_ptr<IMAPFolders>(new IMAPFolders(m_iAccountID, m_iDBID));

      return m_oSubFolders;
   }


   shared_ptr<ACLPermissions>
   IMAPFolder::GetPermissions()
   {
      // Always return a new one. Hopefully we don't have so many public folders
	  // that this will become a performance issue.
      shared_ptr<ACLPermissions> pPermissions = shared_ptr<ACLPermissions>(new ACLPermissions(m_iDBID));
      
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


   std::vector<int> 
   IMAPFolder::Expunge()
   {
      return GetMessages()->Expunge();
   }

   std::vector<int> 
   IMAPFolder::Expunge(const std::set<int> &uids, const boost::function<void()> &func)
   {
      return GetMessages()->Expunge(false, uids, func);
   }


   bool 
   IMAPFolder::XMLStore(XNode *pParentNode, int iBackupOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Folder"));
      pNode->AppendAttr(_T("Name"), String(m_sFolderName));
      pNode->AppendAttr(_T("Subscribed"), m_bFolderIsSubscribed ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("CreateTime"), String(Time::GetTimeStampFromDateTime(_createTime)));
      pNode->AppendAttr(_T("CurrentUID"), StringParser::IntToString(_currentUID));

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
      m_sFolderName = pFolderNode->GetAttrValue(_T("Name"));
      m_bFolderIsSubscribed = pFolderNode->GetAttrValue(_T("Subscribed")) == _T("1");
      _createTime = Time::GetDateFromSystemDate(pFolderNode->GetAttrValue(_T("CreateTime")));
      _currentUID = _ttoi(pFolderNode->GetAttrValue(_T("CurrentUID")));

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
         String sMessage = Formatter::Format("Excessive folder recursion. Giving up. Account: {0}, Folder: {1}", m_iAccountID, m_sFolderName);
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5019, "IMAPFolder::GetFolderDepth", sMessage);

         return 0;
      }

      int iDepth = 1;
      
      shared_ptr<IMAPFolders> pSubFolders = GetSubFolders();
      vector<shared_ptr<IMAPFolder> > vecSubFolders = pSubFolders->GetVector();
      vector<shared_ptr<IMAPFolder> >::iterator iterCurFolder = vecSubFolders.begin();

      int iSubDepth = 0;
      int iMaxSubDepth = 0;
      while (iterCurFolder != vecSubFolders.end())
      {
         shared_ptr<IMAPFolder> pFolder = (*iterCurFolder);

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
      return m_iAccountID == 0;
   }

}