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

      void SetEnabled(bool bEnabled);

      shared_ptr<HM::IMAPFolders> GetFoldersForAccount(__int64 AccountID);
      // Returns a list of all folders for the specified account.

      shared_ptr<HM::IMAPFolders> GetPublicFolders();

      void SetFolderNeedRefresh(__int64 AccountID, __int64 lMailBox = 0);
      // Sets the folders dirty flag. Will cause the IMAP server to
      // fetch new messages the next time the mailbox is accessed.

      void UncacheAccount(__int64 iAccountID);

      void UpdateCurrentUID(__int64 accountID, __int64 folderID, unsigned int currentUID);

      bool Clear();

      static bool IsPublicFolder(const std::vector<String> &vecFolderPath);

      static shared_ptr<IMAPFolder> GetTopMostExistingFolder(shared_ptr<IMAPFolders> pContainer, const std::vector<String> &vecFolderPath);
   private:

      std::map<__int64, shared_ptr<IMAPFolders> > m_mapFolders;
      
      static CriticalSection m_hFetchListCriticalSection; 
      
      bool m_bEnabled;
   
   };
}
