// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ACL;
   class IMAPFolders;
   class IMAPFolder;

   class IMAPFolderContainer : public Singleton<IMAPFolderContainer>
   {
   public:
	   IMAPFolderContainer();
	   virtual ~IMAPFolderContainer();

      std::shared_ptr<HM::IMAPFolders> GetFoldersForAccount(__int64 AccountID);
      // Returns a list of all folders for the specified account.

      std::shared_ptr<HM::IMAPFolders> GetPublicFolders();

      void UncacheAccount(__int64 iAccountID);

      void UpdateCurrentUID(__int64 accountID, __int64 folderID, unsigned int currentUID);

      bool Clear();

      static bool IsPublicFolder(const std::vector<String> &vecFolderPath);

      static std::shared_ptr<IMAPFolder> GetTopMostExistingFolder(std::shared_ptr<IMAPFolders> pContainer, const std::vector<String> &vecFolderPath);

   private:

      std::map<__int64, std::shared_ptr<IMAPFolders> > folders_;
      
      static boost::recursive_mutex fetch_list_mutex_;
   };
}
