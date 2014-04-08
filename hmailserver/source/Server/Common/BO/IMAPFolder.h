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

      __int64 GetID() const { return m_iDBID; }
      void SetID(__int64 lNewVal) { m_iDBID = lNewVal;}

      __int64 GetParentFolderID() const;
      void SetParentFolderID(__int64 value) {m_iParentFolderID = value;}

      __int64 GetAccountID() const { return m_iAccountID;} 
      void SetAccountID(__int64 newVal) {m_iAccountID = newVal;}

      unsigned int GetCurrentUID() const { return _currentUID;} 
      void SetCurrentUID(unsigned int currentUID) {_currentUID = currentUID;}

      const DateTime &GetCreationTime() const { return _createTime;} 
      void SetCreationTime(const DateTime &currentUID) {_createTime = currentUID;}


      bool GetIsSubscribed() const { return m_bFolderIsSubscribed;} 
      void SetIsSubscribed(bool bNewVal) { m_bFolderIsSubscribed = bNewVal;}

      String GetFolderName() const { return m_sFolderName;}
      void SetFolderName(const String & sNewVal) { m_sFolderName =sNewVal; }

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

      __int64 m_iDBID;
      __int64 m_iAccountID;
      __int64 m_iParentFolderID;
      unsigned int _currentUID;

      bool m_bFolderIsSubscribed;
      AnsiString m_sFolderName;

      shared_ptr<Messages> m_oMessages;
      shared_ptr<IMAPFolders> m_oSubFolders;   
 
      DateTime _createTime;

      bool m_bFolderNeedsRefresh;
   };

}

