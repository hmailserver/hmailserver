// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "FolderListCreator.h"

#include "../Common/Application/ACLManager.h"

#include "../Common/BO/ACLPermission.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/IMAPFolders.h"

#include "../IMAP/IMAPConfiguration.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   FolderListCreator::FolderListCreator()
   {

   }

   FolderListCreator::~FolderListCreator()
   {

   }

   String 
   FolderListCreator::GetIMAPFolderList(__int64 iAccountID, shared_ptr<IMAPFolders> pStartFolders, const String &sWildcard, const String &sPrefix) 
   {
      vector<String> vecCurrentFolder;
      vector<String> vecMatchingFolders;

      _CreateIMAPFolderList(iAccountID, pStartFolders, sWildcard, false, sPrefix, vecCurrentFolder, vecMatchingFolders);

      String sRet = StringParser::JoinVector(vecMatchingFolders, "\r\n");

      if (!sRet.IsEmpty())
         sRet += "\r\n";

      return sRet;
   }

   String 
   FolderListCreator::GetIMAPLSUBFolderList(__int64 iAccountID, shared_ptr<IMAPFolders> pStartFolders, const String &sWildcard, const String &sPrefix) 
   {
      vector<String> vecCurrentFolder;
      vector<String> vecMatchingFolders;

      _CreateIMAPFolderList(iAccountID, pStartFolders, sWildcard, true, sPrefix, vecCurrentFolder, vecMatchingFolders);

      String sRet = StringParser::JoinVector(vecMatchingFolders, "\r\n");

      if (!sRet.IsEmpty())
         sRet += "\r\n";

      return sRet;
   }

   void
   FolderListCreator::_CreateIMAPFolderList(__int64 iAccountID, shared_ptr<IMAPFolders> pStartFolders, const String &sWildcard, bool bOnlySubscribed, const String &sPrefix, vector<String> &vecCurrentFolder, vector<String> &vecMatchingFolders) 
   {
      if (vecCurrentFolder.size() > IMAPFolder::MaxFolderDepth)    
         return;
         
      String hierarchyDelimiter = Configuration::Instance()->GetIMAPConfiguration()->GetHierarchyDelimiter();

      bool publicFolderAccessible = false;

	  ACLManager aclManager;
      boost_foreach(shared_ptr<IMAPFolder> currentFolder, pStartFolders->GetVector())
      {
         // Check if the user has access to this folder. Otherwise just skip it.
         shared_ptr<ACLPermission> pPermission = aclManager.GetPermissionForFolder(iAccountID, currentFolder);
         if (!pPermission || !pPermission->GetAllow(ACLPermission::PermissionLookup))
         {
            continue;
         }
         
         // at least one public folder is accessible by the user.
         publicFolderAccessible = true;

         vecCurrentFolder.push_back(currentFolder->GetFolderName());

         String sFullPath = StringParser::JoinVector(vecCurrentFolder, hierarchyDelimiter);

         if (!sPrefix.IsEmpty())
            sFullPath = sPrefix + hierarchyDelimiter + sFullPath;

         shared_ptr<IMAPFolders> subFolders = currentFolder->GetSubFolders();
         bool hasSubFolders = subFolders->GetCount() > 0;

         // Do we match?
         if (_FolderWildcardMatch(sFullPath, sWildcard, hierarchyDelimiter))
         {
            String sFolderLine = _CreateFolderLine(currentFolder, bOnlySubscribed, hasSubFolders, sFullPath, sWildcard, true, hierarchyDelimiter );

            if (!sFolderLine.IsEmpty())
               vecMatchingFolders.push_back(sFolderLine);
         }

         if (hasSubFolders)
            _CreateIMAPFolderList(iAccountID, subFolders, sWildcard, bOnlySubscribed, sPrefix, vecCurrentFolder, vecMatchingFolders);

         vecCurrentFolder.erase(vecCurrentFolder.end() - 1);
      }

      if (vecCurrentFolder.size() == 0 && publicFolderAccessible && !sPrefix.IsEmpty())
      {
         // the user has permission to see at least one public folder.
         String publicFolderName = Configuration::Instance()->GetIMAPConfiguration()->GetIMAPPublicFolderName();

         // we're listing public folders and are on the top level.
         if (_FolderWildcardMatch(publicFolderName, sWildcard, hierarchyDelimiter))
         {
            shared_ptr<IMAPFolder> pFolderDummy;
            String sFolderLine = _CreateFolderLine(pFolderDummy, bOnlySubscribed, true, publicFolderName, sWildcard, false, hierarchyDelimiter);

            if (!sFolderLine.IsEmpty())
               vecMatchingFolders.push_back(sFolderLine);
         }

      }

   }

   String 
   FolderListCreator::_CreateFolderLine(shared_ptr<IMAPFolder> currentFolder, bool bOnlySubscribed, bool hasSubFolders, String &sFullPath, const String &sWildcard, bool isSelectable, String hierarchyDelimiter)
   {
      String nameAttributes = hasSubFolders ? "\\HasChildren" : "\\HasNoChildren";

      if (!isSelectable)
         nameAttributes += " \\Noselect";

      // Workaround for Outlook "feature".
      _AdjustCaseToClientCase(sFullPath, sWildcard, hierarchyDelimiter);

      
      // We cannot send " or \ directly in the response.
      // We must escape those:
      IMAPFolder::EscapeFolderString(sFullPath);

      // 2008-11-04
      // Always quote the string. This is how GMail acts.
      sFullPath = "\"" + sFullPath + "\"";

      String sFolderLine = "";

      // \ needs to be escaped.
      hierarchyDelimiter.Replace(_T("\\"), _T("\\\\"));

      if (bOnlySubscribed && (!currentFolder || currentFolder->GetIsSubscribed()))
         sFolderLine.Format(_T("* LSUB (%s) \"%s\" %s"), nameAttributes, hierarchyDelimiter, sFullPath);
      else if (!bOnlySubscribed) 
         sFolderLine.Format(_T("* LIST (%s) \"%s\" %s"), nameAttributes, hierarchyDelimiter, sFullPath);

      return sFolderLine;
   }

   bool
   FolderListCreator::_FolderWildcardMatch(const String &sFolderName, const String &sWildcard, const String &hierarchyDelimiter)
   {
      // Convert the wildcard path to internal format.
      std::vector<String> vecWildcardPath = StringParser::SplitString(sWildcard, hierarchyDelimiter);
      std::vector<String> vecExistingPath = StringParser::SplitString(sFolderName, hierarchyDelimiter);

      String sTempWildcard = StringParser::JoinVector(vecWildcardPath, hierarchyDelimiter);


      long lRealFolderPos = 0;

      for (int i = 0; i < sTempWildcard.GetLength(); i++)
      {
         wchar_t sCurWildcardChar = sTempWildcard.GetAt(i);

         if (lRealFolderPos >= sFolderName.GetLength())
         {
            // Folder INBOX should match INBOX*.
            if ((sCurWildcardChar == '*' || sCurWildcardChar == '%') && 
               sTempWildcard.GetLength() == i + 1)
               return true;

            return false;
         }

         while (lRealFolderPos < sFolderName.GetLength())
         {

            wchar_t sCurRealCharacter = sFolderName.GetAt(lRealFolderPos);

            // Check if the current char matches the current wildcard char.
            if (sCurWildcardChar == '*')
            {
               // the current wildcard character is a *. we got a match.
               lRealFolderPos++;
               continue;
            }

            if (sCurWildcardChar == '%')
            {
               if (sCurRealCharacter == hierarchyDelimiter.GetAt(0))
               {
                  break;
               }
               else
               {
                  lRealFolderPos++;
                  continue;
               }
            }

            if (toupper(sCurRealCharacter) == toupper(sCurWildcardChar))
            {
               // the current wildcard char matches the current char exactly.
               lRealFolderPos++;
               break;
            }

            // The wildcard character doesn't match the current character in the existing folder.
            return false;



         }

      }

      if (lRealFolderPos < sFolderName.GetLength())
         return false;

      return true;
   }

   void
   FolderListCreator::_AdjustCaseToClientCase(String &sPath, const String &sWildcard, const String &hierarchyDelimiter)
   {
      // Outlook 2003 requires the correct case after creating a new folder.
      // If OE2003 executes CREATE Inbox.SubFolder it requires the response
      // to be Inbox.SubFolder and not INBOX.SubFolder... Even if the real
      // name of the inbox is INBOX and not inbox.. and even if the inbox
      // is working properly.......

      std::vector<String> vecPath = StringParser::SplitString(sPath, hierarchyDelimiter);
      std::vector<String> vecWildcard = StringParser::SplitString(sWildcard, hierarchyDelimiter);

      // Build the response string.
      std::vector<String>::iterator pathIterator = vecPath.begin();
      std::vector<String>::iterator wildIterator = vecWildcard.begin();

      std::vector<String> vecResult;
      while (pathIterator != vecPath.end())
      {
         String sFoldername = (*pathIterator);

         if (wildIterator == vecWildcard.end())
            return;

         String sWildFolder = (*wildIterator);

         if (sFoldername.CompareNoCase(sWildFolder) == 0)
            sFoldername = sWildFolder;

         vecResult.push_back(sFoldername);

         wildIterator ++;
         pathIterator++;

      }

      sPath = StringParser::JoinVector(vecResult, hierarchyDelimiter);
   }



}
