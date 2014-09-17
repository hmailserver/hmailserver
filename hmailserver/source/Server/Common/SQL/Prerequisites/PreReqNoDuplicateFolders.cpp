// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "PreReqNoDuplicateFolders.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PreReqNoDuplicateFolders::PreReqNoDuplicateFolders(void)
   {
   }

   PreReqNoDuplicateFolders::~PreReqNoDuplicateFolders(void)
   {
   }

   bool
   PreReqNoDuplicateFolders::Ensure(std::shared_ptr<DALConnection> connection, String &sErrorMessage)
   {
      SQLCommand command("select folderaccountid, folderparentid, foldername from hm_imapfolders "
                         "group by folderaccountid, folderparentid, foldername "
                         "having count(*) > 1 ");


      // First locate all duplicate folders.
      std::shared_ptr<DALRecordset> duplicateFolderList = connection->CreateRecordset();
      if (!duplicateFolderList->Open(connection, command))
      {
         sErrorMessage = "Failed to execute SQL statement. Please check hMailServer log file.\r\n" + command.GetQueryString();
         return false;
      }

      while (!duplicateFolderList->IsEOF())
      {
         // Locate the folders which are in conflict.
         int folderaccountID = duplicateFolderList->GetLongValue("folderaccountid");
         int parentID = duplicateFolderList->GetLongValue("folderparentid");
         String name = duplicateFolderList->GetStringValue("foldername");

         // In case a user has created a very long folder name. We do this to prevent
         // failure to rename column due to too long name.
         if (name.GetLength() > 245)
            name = name.Mid(0, 245);

         SQLCommand locateCommand ("select folderid from hm_imapfolders where folderaccountid = @ACCOUNTID and folderparentid = @PARENTID and foldername = @FOLDERNAME");
         locateCommand.AddParameter("@ACCOUNTID", folderaccountID);
         locateCommand.AddParameter("@PARENTID", parentID);
         locateCommand.AddParameter("@FOLDERNAME", name);

         std::shared_ptr<DALRecordset> conflictFoldersRecordset = connection->CreateRecordset();
         if (!conflictFoldersRecordset->Open(connection, locateCommand))
         {
            sErrorMessage = "Failed to execute SQL statement. Please check hMailServer log file.\r\n" + locateCommand.GetQueryString();
            return false;
         }

         // Assume that the first folder is the correct one. Since it's correct,
         // we should not rename it so we skip it here.
         conflictFoldersRecordset->MoveNext();

         while (!conflictFoldersRecordset->IsEOF ())
         {
            int folderID = conflictFoldersRecordset->GetLongValue("folderid");

            // Generate a name. The string "name" is already escaped (above).
            String newName;
            newName.Format(_T("%s-%d"), name.c_str(), folderID);

            SQLCommand command("update hm_imapfolders set foldername = @FOLDERNAME where folderid = @FOLDERID");
            command.AddParameter("@FOLDERNAME", newName);
            command.AddParameter("@FOLDERID", folderID);

            if (!connection->Execute(command, sErrorMessage, 0, 0))
               return false;

            conflictFoldersRecordset->MoveNext();
         }
        


         duplicateFolderList->MoveNext();
      }

      return true;

   }


   
}