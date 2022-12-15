// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPFolderContainer.h"

#include "../IMAP/IMAPConfiguration.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/IMAPFolders.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   boost::recursive_mutex IMAPFolderContainer::fetch_list_mutex_;

   IMAPFolderContainer::IMAPFolderContainer()
   {
      // When the server is stopped, the cache should be cleared.
      Application::Instance()->OnServerStopped.connect
         (
         [this]() { Clear(); }
      );
   }

   IMAPFolderContainer::~IMAPFolderContainer()
   {

   }

   std::shared_ptr<IMAPFolders> 
   IMAPFolderContainer::GetFoldersForAccount(__int64 AccountID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(fetch_list_mutex_);

      auto iterFolders = folders_.find(AccountID);
      
      std::shared_ptr<IMAPFolders> pFolders;

      if (iterFolders == folders_.end())
      {
         pFolders = std::shared_ptr<IMAPFolders>(new IMAPFolders(AccountID, -1));
         pFolders->Refresh();
         folders_[AccountID] = pFolders;
      }
      else
      {
         pFolders = (*iterFolders).second;
      }

      return pFolders;
   }

   std::shared_ptr<IMAPFolders>
   IMAPFolderContainer::GetPublicFolders()
   {
      std::shared_ptr<IMAPFolders> pFolders = Configuration::Instance()->GetIMAPConfiguration()->GetPublicFolders();

      return pFolders;
   }

   void 
   IMAPFolderContainer::UncacheAccount(__int64 iAccountID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(fetch_list_mutex_);

      auto iterFolder = folders_.find(iAccountID); 

      if (iterFolder != folders_.end())
      {
         // The account exists. uncache it.
         folders_.erase(iterFolder);
      }

   }

   bool 
   IMAPFolderContainer::Clear()
   {
      boost::lock_guard<boost::recursive_mutex> guard(fetch_list_mutex_);

      bool bCleared = !folders_.empty();

      folders_.clear();

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
   
   std::shared_ptr<IMAPFolder> 
   IMAPFolderContainer::GetTopMostExistingFolder(std::shared_ptr<IMAPFolders> pContainer, const std::vector<String> &vecFolderPath)
   {
      std::vector<String> tempFolderPath = vecFolderPath;
      std::shared_ptr<IMAPFolder> pTempFolder = pContainer->GetFolderByFullPath(tempFolderPath);

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
         std::shared_ptr<IMAPFolder> folder = GetPublicFolders()->GetItemByDBIDRecursive(folderID);
         if (!folder)
         {
            assert(0);
            return;
         }

         folder->SetCurrentUID(currentUID);

      }
      else
      {
         boost::lock_guard<boost::recursive_mutex> guard(fetch_list_mutex_);

         auto iter = folders_.find(accountID);

         if (iter == folders_.end())
            return;

         std::shared_ptr<IMAPFolder> folder = (*iter).second->GetItemByDBIDRecursive(folderID);
         if (!folder)
         {
            assert(0);
            return;
         }

         folder->SetCurrentUID(currentUID);;
      }

   }



}
