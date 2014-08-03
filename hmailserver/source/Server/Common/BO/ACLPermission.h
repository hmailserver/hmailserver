// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class ACLPermission : public BusinessObject<ACLPermission>
   {
   public:
      ACLPermission(void);
      ~ACLPermission(void);

      String GetName() const
      {
         String sName;
         sName.Format(_T("ACLPermission-%I64d"), id_);

         return sName;
      }

      enum ePermissionType
      {
         PTUser = 0,
         PTGroup = 1,
         PTAnyone = 2
      };

      enum ePermission
      {
         PermissionNone = 0,
         PermissionLookup = 1,
         PermissionRead = 2,
         PermissionWriteSeen = 4,
         PermissionWriteOthers = 8,
         PermissionInsert = 16,
         PermissionPost = 32,
         PermissionCreate = 64,
         PermissionDeleteMailbox = 128,
         PermissionWriteDeleted = 256,
         PermissionExpunge = 512,
         PermissionAdminister = 1024
      };

      /*
         l - lookup (mailbox is visible to LIST/LSUB commands, SUBSCRIBE
         mailbox)
         r - read (SELECT the mailbox, perform STATUS)
         s - keep seen/unseen information across sessions (set or clear
         \SEEN flag via STORE, also set \SEEN during APPEND/COPY/
         FETCH BODY[...])
         w - write (set or clear flags other than \SEEN and \DELETED via
         STORE, also set them during APPEND/COPY)
         i - insert (perform APPEND, COPY into mailbox)
         p - post (send mail to submission address for mailbox,
         not enforced by IMAP4 itself)
         k - create mailboxes (CREATE new sub-mailboxes in any
         implementation-defined hierarchy, parent mailbox for the new
         mailbox name in RENAME)
         x - delete mailbox (DELETE mailbox, old mailbox name in RENAME)
         t - delete messages (set or clear \DELETED flag via STORE, set
         \DELETED flag during APPEND/COPY)
         e - perform EXPUNGE and expunge as a part of CLOSE
         a - administer (perform SETACL/DELETEACL/GETACL/LISTRIGHTS)
      */


      __int64 GetShareFolderID();
      void SetShareFolderID(__int64 iNewVal);

      __int64 GetPermissionGroupID();
      void SetPermissionGroupID(__int64 iNewVal);

      __int64 GetPermissionAccountID();
      void SetPermissionAccountID(__int64 iNewVal);

      ePermissionType GetPermissionType();
      void SetPermissionType(ePermissionType iNewVal);

      __int64 GetValue();
      void SetValue(__int64 iNewVal);

      bool GetAllow(ACLPermission::ePermission p) const;
      void SetAllow(ACLPermission::ePermission p, bool bValue);

      void GrantAll();
      
      String GetRights() const;
      
      void AppendPermissions(const String &sRights);

      static ACLPermission::ePermission GetPermission(wchar_t);

      bool XMLStore(XNode *pParentNode, int iBackupOptions);
      bool XMLLoad(XNode *pAccountNode, int iRestoreOptions);
      bool XMLLoadSubItems(XNode *pAccountNode, int iRestoreOptions) {return true; }

   private:

      String GetPermissionHolderName_(ACLPermission *pPermission) const;

      __int64 share_folder_id_;

      ePermissionType permission_type_;

      __int64 permission_group_id_;
      __int64 permission_account_id_;
      
      __int64 value_;
   };
}