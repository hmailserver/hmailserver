// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPFolderContainer.h"

#include "../Common/Application/ACLManager.h"
#include "../IMAP/IMAPConfiguration.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/IMAPFolders.h"
#include "../Common/BO/ACLPermission.h"
#include "../Common/BO/ACLPermissions.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   CriticalSection IMAPFolderContainer::m_hFetchListCriticalSection;

   IMAPFolderContainer::IMAPFolderContainer()
   {
      
   }

   IMAPFolderContainer::~IMAPFolderContainer()
   {

   }

   shared_ptr<IMAPFolders> 
   IMAPFolderContainer::GetFoldersForAccount(__int64 AccountID)
   {
      CriticalSectionScope scope(m_hFetchListCriticalSection);

      std::map<__int64, shared_ptr<HM::IMAPFolders> >::iterator iterFolders = m_mapFolders.find(AccountID);
      
      shared_ptr<IMAPFolders> pFolders;

      if (iterFolders == m_mapFolders.end())
      {
         pFolders = shared_ptr<IMAPFolders>(new IMAPFolders(AccountID, -1));
         pFolders->Refresh();
         m_mapFolders[AccountID] = pFolders;
      }
      else
      {
         pFolders = (*iterFolders).second;
      }

      return pFolders;
   }

   shared_ptr<IMAPFolders>
   IMAPFolderContainer::GetPublicFolders()
   {
      shared_ptr<IMAPFolders> pFolders = Configuration::Instance()->GetIMAPConfiguration()->GetPublicFolders();

      return pFolders;
   }

   void
   IMAPFolderContainer::SetFolderNeedRefresh(__int64 AccountID, __int64 lMailBox)
   {
      CriticalSectionScope scope(m_hFetchListCriticalSection);

      shared_ptr<IMAPFolder> pFolder;
      if (AccountID == 0)
      {
         // Get the public folder.
         pFolder = GetPublicFolders()->GetItemByDBIDRecursive(lMailBox);
      }
      else
      {
         std::map<__int64, shared_ptr<HM::IMAPFolders> >::iterator iterFolder = m_mapFolders.find(AccountID); 

         if (iterFolder == m_mapFolders.end())
            return;

         pFolder = (*iterFolder).second->GetItemByDBIDRecursive(lMailBox);
      }

      if (!pFolder)
      {
         String sErrorMessage;
         sErrorMessage.Format(_T("Folder could not be fetched. Account: %d, Folder: %d"), AccountID, lMailBox);
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4214, "IMAPFolderContainer::SetFolderNeedRefresh", sErrorMessage);

         return;
      }

      pFolder->SetFolderNeedsRefresh();
   }

   void 
   IMAPFolderContainer::UncacheAccount(__int64 iAccountID)
   {
      CriticalSectionScope scope(m_hFetchListCriticalSection);

      std::map<__int64, shared_ptr<HM::IMAPFolders> >::iterator iterFolder = m_mapFolders.find(iAccountID); 

      if (iterFolder != m_mapFolders.end())
      {
         // The account exists. uncache it.
         m_mapFolders.erase(iterFolder);
      }

   }

   bool 
   IMAPFolderContainer::Clear()
   {
      CriticalSectionScope scope(m_hFetchListCriticalSection);

      bool bCleared = !m_mapFolders.empty();

      m_mapFolders.clear();

      return bCleared;
   }

   bool 
   IMAPFolderContainer::IsPublicFolder(const std::vector<String> &vecFolderPath)
   {
      if (vecFolderPath.size() == 0)
         return false;

      String sPublicFolderName = Configuration::Instance()->GetIMAPConfiguration()->GetIMAPPublicFolderName();
      if (sPublicFolderName.CompareNoCase(vecFolderPath[0]) == 0)
         return true;
      else
         return false;
   }
   
   shared_ptr<IMAPFolder> 
   IMAPFolderContainer::GetTopMostExistingFolder(shared_ptr<IMAPFolders> pContainer, const std::vector<String> &vecFolderPath)
   {
      vector<String> tempFolderPath = vecFolderPath;
      shared_ptr<IMAPFolder> pTempFolder = pContainer->GetFolderByFullPath(tempFolderPath);

      while (!pTempFolder && tempFolderPath.size() > 0)
      {
         tempFolderPath.erase(tempFolderPath.end() - 1);
         
         pTempFolder = pContainer->GetFolderByFullPath(tempFolderPath);   
      }

      return pTempFolder;
   }

   void
   IMAPFolderContainer::UpdateCurrentUID(__int64 accountID, __int64 folderID, unsigned int currentUID)
   {
      if (accountID == 0)
      {
         shared_ptr<IMAPFolder> folder = GetPublicFolders()->GetItemByDBIDRecursive(folderID);
         if (!folder)
         {
            assert(0);
            return;
         }

         folder->SetCurrentUID(currentUID);

      }
      else
      {
         CriticalSectionScope scope(m_hFetchListCriticalSection);

         std::map<__int64, shared_ptr<HM::IMAPFolders> >::iterator iter = m_mapFolders.find(accountID);

         if (iter == m_mapFolders.end())
            return;

         shared_ptr<IMAPFolder> folder = (*iter).second->GetItemByDBIDRecursive(folderID);
         if (!folder)
         {
            assert(0);
            return;
         }

         folder->SetCurrentUID(currentUID);;
      }

   }



}
