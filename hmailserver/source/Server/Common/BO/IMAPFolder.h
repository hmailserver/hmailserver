// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


#pragma once

#include "Messages.h"
#include "..\Util\VariantDateTime.h"

namespace HM
{
   class IMAPFolders;
   //class IMAPFolder;
   class ACLPermissions;

   class IMAPFolder 
   {
   public:

      enum
      {
         MaxFolderDepth = 25
      };
   
      IMAPFolder();
      IMAPFolder(__int64 iAccountID, __int64 iParentFolderID);

	   virtual ~IMAPFolder();

      __int64 GetID() const { return dbid_; }
      void SetID(__int64 lNewVal) { dbid_ = lNewVal;}

      __int64 GetParentFolderID() const;
      void SetParentFolderID(__int64 value) {parent_folder_id_ = value;}

      __int64 GetAccountID() const { return account_id_;} 
      void SetAccountID(__int64 newVal) {account_id_ = newVal;}

      unsigned int GetCurrentUID() const { return current_uid_;} 
      void SetCurrentUID(unsigned int currentUID) {current_uid_ = currentUID;}

      const DateTime &GetCreationTime() const { return create_time_;} 
      void SetCreationTime(const DateTime &currentUID) {create_time_ = currentUID;}


      bool GetIsSubscribed() const { return folder_is_subscribed_;} 
      void SetIsSubscribed(bool bNewVal) { folder_is_subscribed_ = bNewVal;}

      String GetFolderName() const { return folder_name_;}
      void SetFolderName(const String & sNewVal) { folder_name_ =sNewVal; }

      String GetName() const {return GetFolderName(); }
      
      std::vector<int> Expunge();
      std::vector<int> Expunge(const std::set<int> &uids, const boost::function<void()> &func);

      shared_ptr<Messages> GetMessages(bool bReloadIfNeeded = true);
      vector<shared_ptr<Message>> GetMessagesCopy(bool bReloadIfNeeded = true);
      shared_ptr<IMAPFolders> GetSubFolders();
      shared_ptr<ACLPermissions> GetPermissions();

      static void EscapeFolderString(String &sFolderString);
      static void UnescapeFolderString(String &sFolderString);
      
      int GetFolderDepth(int &iRecursion);

      void SetFolderNeedsRefresh();
      // Sets the dirty flag on this folder. Will cause the IMAP
      // folder to refresh messages in the folder the next time 
      // folder is accessed.

      bool XMLStore(XNode *pParentNode, int iBackupOptions);
      bool XMLLoad(XNode *pFolderNode, int iRestoreOptions);
      bool XMLLoadSubItems(XNode *pFolderNode, int iRestoreOptions);

      static bool IsValidFolderName(const std::vector<String> &vecPath, bool bIsPublicFolder);

      bool IsPublicFolder();

   protected:

      __int64 dbid_;
      __int64 account_id_;
      __int64 parent_folder_id_;
      unsigned int current_uid_;

      bool folder_is_subscribed_;
      AnsiString folder_name_;

      shared_ptr<Messages> messages_;
      shared_ptr<IMAPFolders> sub_folders_;   
 
      DateTime create_time_;

      bool folder_needs_refresh_;
   };

}

