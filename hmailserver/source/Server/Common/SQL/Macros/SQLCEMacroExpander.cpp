// Copyright (c) 2009 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "SQLCEMacroExpander.h"
#include "Macro.h"

#include "../SQLCERecordset.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool
   SQLCEMacroExpander::ProcessMacro(std::shared_ptr<DALConnection> connection, const Macro &macro, String &sErrorMessage)
   {
      switch (macro.GetType())
      {
      case Macro::DropColumnKeys:
         {
            /// DROP KEYS

            SQLCommand command("select DISTINCT CONSTRAINT_NAME from INFORMATION_SCHEMA.KEY_COLUMN_USAGE WHERE TABLE_NAME = @TABLE_NAME AND COLUMN_NAME = @COLUMN_NAME");
            command.AddParameter("@TABLE_NAME", macro.GetTableName());
            command.AddParameter("@COLUMN_NAME", macro.GetColumnName());

            SQLCERecordset rec;
            if (!rec.Open(connection, command))
            {
               sErrorMessage = "It was not possible to execute the below SQL statement. Please see hMailServer error log for details.\r\n" + command.GetQueryString();
               return false;
            }

            while (!rec.IsEOF())
            {
               String constraintName = rec.GetStringValue("CONSTRAINT_NAME");

               String sqlUpdate;
               sqlUpdate.Format(_T("ALTER TABLE %s DROP %s"), macro.GetTableName().c_str(), constraintName.c_str());

               DALConnection::ExecutionResult execResult = connection->TryExecute(SQLCommand(sqlUpdate), sErrorMessage, 0, 0);

               if (execResult != DALConnection::DALSuccess)
                  return false;

               rec.MoveNext();
            }

            /// DROP Indexes
            SQLCommand dropCommand ("SELECT DISTINCT INDEX_NAME FROM INFORMATION_SCHEMA.INDEXES WHERE TABLE_NAME = @TABLE_NAME AND COLUMN_NAME = @COLUMN_NAME");
            dropCommand.AddParameter("@TABLE_NAME", macro.GetTableName());
            dropCommand.AddParameter("@COLUMN_NAME", macro.GetColumnName());

            SQLCERecordset indexRecordset;
            if (!indexRecordset.Open(connection, dropCommand))
            {
               sErrorMessage = "It was not possible to execute the below SQL statement. Please see hMailServer error log for details.\r\n" + dropCommand.GetQueryString();
               return false;
            }

            while (!indexRecordset.IsEOF())
            {
               String indexName = indexRecordset.GetStringValue("INDEX_NAME");

               String sqlUpdate;
               sqlUpdate.Format(_T("DROP INDEX %s.%s"), macro.GetTableName().c_str(), indexName.c_str());

               DALConnection::ExecutionResult execResult = connection->TryExecute(SQLCommand(sqlUpdate), sErrorMessage, 0, 0);

               if (execResult != DALConnection::DALSuccess)
                  return false;

               indexRecordset.MoveNext();
            }
         }
         break;
      case Macro::SQLCEUPDATE_MESSAGES_SET_FOLDER_INBOX:
         {
            /*
            Short varant. UPDATE ... FROM is not supported in SQL CE 3.5
            update hm_messages set messagefolderid = 
            (select max(folderid) from hm_imapfolders where foldername = 'Inbox' and folderaccountid = messageaccountid and folderparentid = -1)
            where messagefolderid = 0 and messagetype = 2

            Updates all messages currently pointing at folder ID 0 (old inbox) so that they point at
            the actual Inbox folder ID instead.
            */

            // Retrieve a list of inboxes for all messages.
            SQLCommand recordSQL("SELECT folderaccountid, folderid FROM hm_imapfolders WHERE foldername = 'Inbox' AND folderparentid = -1");

            SQLCERecordset rec;
            if (!rec.Open(connection, recordSQL))
            {
               sErrorMessage = "It was not possible to execute the below SQL statement. Please see hMailServer error log for details.\r\n" + recordSQL.GetQueryString();
               return false;
            }

            while (!rec.IsEOF())
            {
               unsigned int folderAccountID = (unsigned int) rec.GetInt64Value("folderaccountid");
               unsigned int folderID = (unsigned int) rec.GetInt64Value("folderid");

               SQLCommand command("UPDATE hm_messages SET messagefolderid = @FOLDERID WHERE messageaccountid = @ACCOUNTID and messagefolderid = 0");
               command.AddParameter("@FOLDERID", folderID);
               command.AddParameter("@ACCOUNTID", folderAccountID);

               DALConnection::ExecutionResult execResult = connection->TryExecute(command, sErrorMessage, 0, 0);

               if (execResult != DALConnection::DALSuccess)
                  return false;

               rec.MoveNext();
            }
            break;
         }
      case Macro::SQLCEUPDATE_FOLDERS_SET_CURRENT_UID:
         {
            /*
            Short varant. UPDATE ... FROM is not supported in SQL CE 3.5
            update hm_imapfolders set foldercurrentuid = (select max(messageid) from hm_messages where folderid = messagefolderid)

            Updates the hm_imapfolders table to contain up-to-date foldercurrentuid's.
            */
            SQLCommand recordSQL("SELECT messagefolderid, MAX(messageid) messageid FROM hm_messages GROUP BY messagefolderid");

            SQLCERecordset rec;
            if (!rec.Open(connection, recordSQL))
            {
               sErrorMessage = "It was not possible to execute the below SQL statement. Please see hMailServer error log for details.\r\n" + recordSQL.GetQueryString();
               return false;
            }

            while (!rec.IsEOF())
            {
               unsigned int messageFolderID = (unsigned int) rec.GetInt64Value("messagefolderid");
               unsigned int messageID = (unsigned int) rec.GetInt64Value("messageid");

               SQLCommand updateCommand("UPDATE hm_imapfolders SET foldercurrentuid = @FOLDERCURRENTUID WHERE folderid = @FOLDERID");
               updateCommand.AddParameter("@FOLDERCURRENTUID", messageID);
               updateCommand.AddParameter("@FOLDERID", messageFolderID);

               DALConnection::ExecutionResult execResult = connection->TryExecute(updateCommand, sErrorMessage, 0, 0);

               if (execResult != DALConnection::DALSuccess)
                  return false;

               rec.MoveNext();
            }
            break;
         }
      case Macro::SQLCEUPDATE_FOLDERS_SET_NEW_PARENTFOLDERID_WHERE_ZERO:
         {
            /*
            Short varant:

            update hm_imapfolders set folderparentid = 
            (select folderid from hm_imapfolders folderinner where foldername = 'Inbox' and folderinner.folderaccountid = hm_imapfolders.folderaccountid)
            where folderparentid = 0

            Updates the hm_imapfolders to set the folderparentid to the new Inbox ID in the cases
            where folderparentid is zero.

            */
            SQLCommand recordSQL("select folderaccountid, folderid from hm_imapfolders where foldername = 'Inbox' and folderparentid = -1");

            SQLCERecordset rec;
            if (!rec.Open(connection, recordSQL))
            {
              sErrorMessage = "It was not possible to execute the below SQL statement. Please see hMailServer error log for details.\r\n" + recordSQL.GetQueryString();
               return false;
            }

            while (!rec.IsEOF())
            {
               unsigned int accountID = (unsigned int) rec.GetInt64Value("folderaccountid");
               unsigned int inboxFolderID = (unsigned int) rec.GetInt64Value("folderid");

               SQLCommand updateCommand ("update hm_imapfolders set folderparentid = @PARENTID where folderaccountid = @ACCOUNTID and folderparentid = 0");
               updateCommand.AddParameter("@PARENTID", inboxFolderID);
               updateCommand.AddParameter("@ACCOUNTID", accountID);

               DALConnection::ExecutionResult execResult = connection->TryExecute(updateCommand, sErrorMessage, 0, 0);

               if (execResult != DALConnection::DALSuccess)
                  return false;

               rec.MoveNext();
            }

            break;
         }

      case Macro::SQLCE_UPDATE_IMAP_HIERARCHY_DELIMITER:
         {
            /*
            Short varant:

            declare @delimiter varchar(10)
            select @delimiter = settingstring from hm_settings where settingname = 'IMAPHierarchyDelimiter'
            update hm_rule_actions set actionimapfolder = REPLACE(actionimapfolder, '\', @delimiter)
            update hm_rule_actions set actionimapfolder = REPLACE(actionimapfolder, '/', @delimiter)
            update hm_rule_actions set actionimapfolder = REPLACE(actionimapfolder, '.', @delimiter)

            */
            SQLCommand recordSQL("select settingstring from hm_settings where settingname = 'IMAPHierarchyDelimiter'");

            SQLCERecordset rec;
            if (!rec.Open(connection, recordSQL))
            {
               sErrorMessage = "It was not possible to execute the below SQL statement. Please see hMailServer error log for details.\r\n" + recordSQL.GetQueryString();
               return false;
            }

            if (rec.IsEOF())
            {
               sErrorMessage = "It was not possible to retrieve the value for the setting IMAPHierarchyDelimiter";
               return false;
            }

            String delimiter = rec.GetStringValue("settingstring");

            String sqlUpdate;
            sqlUpdate.Format(_T("update hm_rule_actions set actionimapfolder = REPLACE(actionimapfolder, '\', '%s')"), delimiter.c_str());
            DALConnection::ExecutionResult execResult = connection->TryExecute(SQLCommand(sqlUpdate), sErrorMessage, 0, 0);
            if (execResult != DALConnection::DALSuccess)
               return false;

            sqlUpdate.Format(_T("update hm_rule_actions set actionimapfolder = REPLACE(actionimapfolder, '/', '%s')"), delimiter.c_str());
            execResult = connection->TryExecute(SQLCommand(sqlUpdate), sErrorMessage, 0, 0);
            if (execResult != DALConnection::DALSuccess)
               return false;

            sqlUpdate.Format(_T("update hm_rule_actions set actionimapfolder = REPLACE(actionimapfolder, '.', '%s')"), delimiter.c_str());
            execResult = connection->TryExecute(SQLCommand(sqlUpdate), sErrorMessage, 0, 0);
            if (execResult != DALConnection::DALSuccess)
               return false;
            
            break;
         }

      }

      return true;
   }
}
