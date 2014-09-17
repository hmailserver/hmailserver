// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "PersistentIncomingRelay.h"

#include "../BO/IncomingRelay.h"
#include "..\SQL\IPAddressSQLHelper.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentIncomingRelay::PersistentIncomingRelay()
   {

   }

   PersistentIncomingRelay::~PersistentIncomingRelay()
   {

   }

   bool
   PersistentIncomingRelay::DeleteObject(std::shared_ptr<IncomingRelay> pSR)
   {
      assert(pSR->GetID());

      bool bResult = false;
      if (pSR->GetID() > 0)
      {
         SQLCommand command("delete from hm_incoming_relays where relayid = @RELAYID");
         command.AddParameter("@RELAYID", pSR->GetID());

         bResult = Application::Instance()->GetDBManager()->Execute(command);
      }

      return bResult;

   }

   bool
   PersistentIncomingRelay::SaveObject(std::shared_ptr<IncomingRelay> pSR, String &errorMessage, PersistenceMode mode)
   {
      // errorMessage - not supported yet.
      return SaveObject(pSR);
   }

   bool
   PersistentIncomingRelay::SaveObject(std::shared_ptr<IncomingRelay> pSR)
   {
      SQLStatement oStatement;
      oStatement.SetTable("hm_incoming_relays");

      oStatement.AddColumn("relayname", pSR->GetName());

      IPAddressSQLHelper helper;

      helper.AppendStatement(oStatement, pSR->GetLowerIP(), "relaylowerip1", "relaylowerip2");
      helper.AppendStatement(oStatement, pSR->GetUpperIP(), "relayupperip1", "relayupperip2");

      if (pSR->GetID() == 0)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("relayid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);

         String sWhere;
         sWhere.Format(_T("relayid = %d"), (unsigned int) pSR->GetID());
         oStatement.SetWhereClause(sWhere);
      }

      bool bNewObject = pSR->GetID() == 0;

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pSR->SetID((int) iDBID);
      return bRetVal;
   }

   bool
   PersistentIncomingRelay::ReadObject(std::shared_ptr<IncomingRelay> pSR, int lDBID)
   {
      SQLCommand command("select * from hm_incoming_relays where relayid = @RELAYID");
      command.AddParameter("@RELAYID", lDBID);

      bool bResult = ReadObject(pSR, command);

      return bResult;
   }


   bool
   PersistentIncomingRelay::ReadObject(std::shared_ptr<IncomingRelay> pSR, const SQLCommand &command)
   {
      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return false;

      bool bRetVal = false;
      if (!pRS->IsEOF())
      {
         bRetVal = ReadObject(pSR, pRS);
      }
  
      return bRetVal;
   }



   bool
   PersistentIncomingRelay::ReadObject(std::shared_ptr<IncomingRelay> pSR, std::shared_ptr<DALRecordset> pRS)
   {
      pSR->SetID(pRS->GetLongValue("relayid"));
      
      IPAddressSQLHelper helper;

      pSR->SetLowerIP(helper.Construct(pRS, "relaylowerip1", "relaylowerip2"));
      pSR->SetUpperIP(helper.Construct(pRS, "relayupperip1", "relayupperip2"));

      pSR->SetName(pRS->GetStringValue("relayname"));
  
      return true;
   }
}