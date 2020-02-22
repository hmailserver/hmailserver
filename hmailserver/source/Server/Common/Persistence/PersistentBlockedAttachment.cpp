// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include ".\PersistentBlockedAttachment.h"
#include "..\BO\BlockedAttachment.h"
#include "..\SQL\SQLStatement.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentBlockedAttachment::PersistentBlockedAttachment(void)
   {
   }

   PersistentBlockedAttachment::~PersistentBlockedAttachment(void)
   {
   }

   bool
   PersistentBlockedAttachment::DeleteObject(std::shared_ptr<BlockedAttachment> pObject)
   {
      SQLCommand command("delete from hm_blocked_attachments where baid = @BAID");
      command.AddParameter("@BAID", pObject->GetID());

      return Application::Instance()->GetDBManager()->Execute(command);
   }

   bool 
   PersistentBlockedAttachment::ReadObject(std::shared_ptr<BlockedAttachment> pObject, std::shared_ptr<DALRecordset> pRS)
   {
      pObject->SetID (pRS->GetLongValue("baid"));
      pObject->SetWildcard(pRS->GetStringValue("bawildcard"));
      pObject->SetDescription(pRS->GetStringValue("badescription"));

      return true;
   }

   bool 
   PersistentBlockedAttachment::SaveObject(std::shared_ptr<BlockedAttachment> pObject, String &errorMessage, PersistenceMode mode)
   {
      // errorMessage -- not supported yet.
      return SaveObject(pObject);
   }

   bool 
   PersistentBlockedAttachment::SaveObject(std::shared_ptr<BlockedAttachment> pObject)
   {
      SQLStatement oStatement;
      oStatement.SetTable("hm_blocked_attachments");
      
      if (pObject->GetID() == 0)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("baid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);
         String sWhere;
         sWhere.Format(_T("baid = %I64d"), pObject->GetID());
         oStatement.SetWhereClause(sWhere);
         
      }

      oStatement.AddColumn(_T("bawildcard"), pObject->GetWildcard());
      oStatement.AddColumn(_T("badescription"), pObject->GetDescription());

      bool bNewObject = pObject->GetID() == 0;

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pObject->SetID((int) iDBID);


      return true;
   }
}