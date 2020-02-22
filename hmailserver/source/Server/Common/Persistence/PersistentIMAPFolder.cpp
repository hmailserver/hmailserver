// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "PersistentIMAPFolder.h"
#include "PersistentACLPermission.h"

#include "../BO/ACLPermissions.h"
#include "../BO/IMAPFolders.h"
#include "..\BO\IMAPFolder.h"

#include "..\..\IMAP\IMAPFolderContainer.h"
#include "..\..\IMAP\MessagesContainer.h"

#include "..\Tracking\ChangeNotification.h"
#include "..\Tracking\NotificationServer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentIMAPFolder::PersistentIMAPFolder()
   {

   }

   PersistentIMAPFolder::~PersistentIMAPFolder()
   {

   }

   bool
   PersistentIMAPFolder::DeleteByAccount(__int64 iAccountID)
   {
      if (iAccountID <= 0)
         return false;

      IMAPFolders accountFolders (iAccountID, -1);
      accountFolders.Refresh();
      return accountFolders.DeleteAll();
   }

   bool
   PersistentIMAPFolder::DeleteObject(std::shared_ptr<IMAPFolder> pFolder)
   {
      return DeleteObject  (pFolder, false);
   }

   /*
      Deletes a specific IMAP folder.

      If forceDelete is false, the user Inbox won't be deleted.

   */
   bool
   PersistentIMAPFolder::DeleteObject(std::shared_ptr<IMAPFolder> pFolder, bool forceDelete)
   {
      if (pFolder->GetID() <= 0)
         return false;
      
      // Delete sub folders first...
      if (!pFolder->GetSubFolders()->DeleteAll())
         return false;

      // We must delete all email in this folder.
      pFolder->GetMessages()->Refresh(false);

      std::function<bool(int, std::shared_ptr<Message>)> filter = [](int index, std::shared_ptr<Message> message)
         {
            return true;
         };

      auto messages = MessagesContainer::Instance()->GetMessages(pFolder->GetAccountID(), pFolder->GetID());
      messages->DeleteMessages(filter);
            
      if (!pFolder->GetPermissions()->DeleteAll())
         return false;

      bool isInbox = pFolder->GetParentFolderID() == -1 && pFolder->GetFolderName().CompareNoCase(_T("Inbox")) == 0;
      bool deleteActualFolder = forceDelete || !isInbox;

      if (deleteActualFolder)
      {
         SQLCommand command("delete from hm_imapfolders where folderid = @FOLDERID");
         command.AddParameter("@FOLDERID", pFolder->GetID());

         bool result = Application::Instance()->GetDBManager()->Execute(command);

         return result;
      }
      else
         return true;
   }

   bool
   PersistentIMAPFolder::SaveObject(std::shared_ptr<IMAPFolder> pFolder, String &errorMessage, PersistenceMode mode)
   {
      // errorMessage not supported yet.
      return SaveObject(pFolder);
   }

   bool
   PersistentIMAPFolder::SaveObject(std::shared_ptr<IMAPFolder> pFolder)
   {
      bool bNewObject = true;
      if (pFolder->GetID())
         bNewObject = false;
      
      SQLStatement oStatement;
      
      oStatement.SetTable("hm_imapfolders");
      
      if (bNewObject)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("folderid");

         DateTime creationTime = pFolder->GetCreationTime();
         if (pFolder->GetCreationTime().GetStatus() == DateTime::invalid)
            pFolder->SetCreationTime(DateTime::GetCurrentTime());

         // This column is always updated by GetUniqueMessageID below
         // but we still need to create it.
         oStatement.AddColumn("foldercurrentuid", pFolder->GetCurrentUID());
         oStatement.AddColumnDate("foldercreationtime", pFolder->GetCreationTime());
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);

         String sWhere;
         sWhere.Format(_T("folderid = %I64d"), pFolder->GetID());

         oStatement.SetWhereClause(sWhere);
      }

      oStatement.AddColumnInt64("folderaccountid", pFolder->GetAccountID());
      oStatement.AddColumnInt64("folderparentid", pFolder->GetParentFolderID());
      oStatement.AddColumn("foldername", pFolder->GetFolderName());
      oStatement.AddColumn("folderissubscribed", pFolder->GetIsSubscribed() ? 1 : 0);

      
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);      
      if (bRetVal && bNewObject)
         pFolder->SetID((int) iDBID);


      return true;
   }

   __int64 
   PersistentIMAPFolder::GetUserInboxFolder(__int64 accountID)
   {
      SQLCommand command("SELECT folderid FROM hm_imapfolders WHERE folderaccountid = @FOLDERACCOUNTID and folderparentid = -1 and foldername = 'INBOX'");
      command.AddParameter("@FOLDERACCOUNTID", accountID);

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
      {
         String message;
         message.Format(_T("The inbox for account %I64d could not be looked up"), accountID);
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5206, "PersistentIMAPFolder::GetUserInboxFolder", message);
         return 0;
      }

      __int64 folderID = pRS->GetInt64Value("folderid");

      return folderID;
   }


   bool 
   PersistentIMAPFolder::GetExistsFolderContainingCharacter(String theChar)
   {
      theChar = SQLStatement::Escape(theChar);

      SQLCommand command(_T("select count(*) as c from hm_imapfolders where foldername like '%" + theChar + "%'"));

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return false;

      long count = pRS->GetLongValue("c");

      return count > 0;
   }

   unsigned int 
   PersistentIMAPFolder::GetCurrentUID_(__int64 folderID)
   {
      if (folderID == 0)
         return 0;

      SQLCommand command("SELECT foldercurrentuid FROM hm_imapfolders WHERE folderid = @FOLDERID");
      command.AddParameter("@FOLDERID", folderID);

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
      {
         String message;
         message.Format(_T("Current UID for folder %I64d could not be looked up"), folderID);
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5207, "PersistentIMAPFolder::GetCurrentUID_", message);

         return 0;
      }

      if (pRS->IsEOF())
      {
         String message;
         message.Format(_T("Current UID for folder %I64d could not be looked up. Folder does not eixst."), folderID);
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5207, "PersistentIMAPFolder::GetCurrentUID_", message);

         return 0;
      }

      unsigned int lastUID = (unsigned int) pRS->GetInt64Value("foldercurrentuid");

      return lastUID;
   }

   bool 
   PersistentIMAPFolder::IncreaseCurrentUID_(__int64 folderID)
   {
      SQLCommand command("UPDATE hm_imapfolders SET foldercurrentuid = foldercurrentuid + 1 WHERE folderid = @FOLDERID");
      command.AddParameter("@FOLDERID", folderID);

      return Application::Instance()->GetDBManager()->Execute(command);
   }

   unsigned int 
   PersistentIMAPFolder::GetUniqueMessageID(__int64 accountID, __int64 folderID)
   {
      if (folderID == 0)
         return 0;

      IncreaseCurrentUID_(folderID);
      unsigned int newUID = GetCurrentUID_(folderID);

      IMAPFolderContainer::Instance()->UpdateCurrentUID(accountID, folderID, newUID);

      return newUID;
   }
}