// Copyright (c) 2009 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Maintenance.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM   
{
   Maintenance::Maintenance()
   {

   }

   Maintenance::~Maintenance()
   {

   }

   bool
   Maintenance::Perform(MaintenanceOperation operation)
   {
      switch (operation)
      {
      case RecalculateFolderUID:
         return RecalculateFolderUID_();
      }

      return false;
   }

   // Goes through all mailboxes and sets the foldercurrentuid to the latest message uid.
   bool
   Maintenance::RecalculateFolderUID_()
   {
      AnsiString recordSQL = "SELECT messagefolderid, MAX(messageuid) as messageuid FROM hm_messages GROUP BY messagefolderid";

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(SQLCommand(recordSQL));

      if (!pRS)
         return false;

      while (!pRS->IsEOF())
      {
         __int64 messageFolderID = pRS->GetInt64Value("messagefolderid");
         __int64 messageUID = pRS->GetInt64Value("messageuid");

         if (messageFolderID <= 0)
            return false;

         if (messageUID <= 0)
            return false;

         AnsiString sqlUpdate = Formatter::Format("UPDATE hm_imapfolders SET foldercurrentuid = {0} WHERE folderid = {1} AND foldercurrentuid < {0}", messageUID, messageFolderID);

         bool result = Application::Instance()->GetDBManager()->Execute(SQLCommand(sqlUpdate));
         if (result == false)
            return false;
            
         pRS->MoveNext();
      }

      return true;
   }

}