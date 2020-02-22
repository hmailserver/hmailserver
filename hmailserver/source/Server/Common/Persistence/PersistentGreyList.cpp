// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include ".\PersistentGreyList.h"
#include "..\BO\GreyListTriplet.h"
#include "..\SQL\DALConnection.h"
#include "..\SQL\IPAddressSQLHelper.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentGreyList::PersistentGreyList(void)
   {

   }

   PersistentGreyList::~PersistentGreyList(void)
   {

   }

   std::shared_ptr<GreyListTriplet> 
   PersistentGreyList::GetRecord(const String &sSenderAddress, const String &sRecipientAddress, const IPAddress & remoteIP)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns a grey list triple based on sender, recipient and IP address.
   //---------------------------------------------------------------------------()
   {
      std::shared_ptr<GreyListTriplet> pTriplet;

      IPAddressSQLHelper helper;

      String sSQL;
      sSQL.Format(_T("select * from hm_greylisting_triplets where glipaddress1 %s and glipaddress2 %s and glsenderaddress = @SENDERADDRESS and glrecipientaddress = @RECIPIENTADDRESS"), 
         String(helper.GetAddress1Equals(remoteIP).c_str()),
         String(helper.GetAddress2Equals(remoteIP).c_str()));

      SQLCommand command(sSQL);
      command.AddParameter("@SENDERADDRESS", sSenderAddress);
      command.AddParameter("@RECIPIENTADDRESS", sRecipientAddress);

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS || pRS->IsEOF())
      {
         // Not found
         return pTriplet;
      }

      // Read the record.
      pTriplet = std::shared_ptr<GreyListTriplet>(new GreyListTriplet);
      pTriplet->SetID(pRS->GetInt64Value("glid"));
      pTriplet->SetCreateTime(pRS->GetStringValue("glcreatetime"));
      pTriplet->SetBlockEndTime(pRS->GetStringValue("glblockendtime"));
      pTriplet->SetDeleteTime(pRS->GetStringValue("gldeletetime"));
      
      pTriplet->SetIPAddress(IPAddress(pRS->GetInt64Value("glipaddress1"), pRS->GetInt64Value("glipaddress2")));
      pTriplet->SetSenderAddress(pRS->GetStringValue("glsenderaddress"));
      pTriplet->SetRecipientAddress(pRS->GetStringValue("glrecipientaddress"));

      pTriplet->SetPassedCount(pRS->GetLongValue("glpassedcount"));
      pTriplet->SetBlockedCount(pRS->GetLongValue("glblockedcount"));

      return pTriplet;
   }

   bool 
   PersistentGreyList::AddObject(std::shared_ptr<GreyListTriplet> pTriplet)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Adds a new greylist triple into the database.
   //---------------------------------------------------------------------------()
   {
      String sSenderAddress = pTriplet->GetSenderAddress().Left(200);
      String sRecipientAddress = pTriplet->GetRecipientAddress().Left(200);
   
      // Update the triplet
      SQLStatement oStatement;

      IPAddressSQLHelper helper;

      oStatement.SetTable("hm_greylisting_triplets");
      oStatement.AddColumn("glcreatetime", pTriplet->GetCreateTime());
      oStatement.AddColumn("glblockendtime", pTriplet->GetBlockEndTime());
      oStatement.AddColumn("gldeletetime", pTriplet->GetDeleteTime());
      
      helper.AppendStatement(oStatement, pTriplet->GetIPAddress(), "glipaddress1", "glipaddress2");

      oStatement.AddColumn("glsenderaddress", sSenderAddress);
      oStatement.AddColumn("glrecipientaddress", sRecipientAddress);
      oStatement.AddColumn("glpassedcount", "0");
      oStatement.AddColumn("glblockedcount", "0");

      oStatement.SetStatementType(SQLStatement::STInsert);
      oStatement.SetIdentityColumn("glid");

      bool bResult = Application::Instance()->GetDBManager()->Execute(oStatement, 0, DALConnection::DALErrorInSQL);

      return bResult;
   }

   bool 
   PersistentGreyList::ResetDeletionTime(std::shared_ptr<GreyListTriplet> pTriplet)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Resets the deletion time for a triple.
   //---------------------------------------------------------------------------()
   {
      // Update it.
      SQLCommand command("update hm_greylisting_triplets set gldeletetime = @DELETETIME, glpassedcount = glpassedcount+1 where glid = @GLID");
      command.AddParameter("@DELETETIME", pTriplet->GetDeleteTime());
      command.AddParameter("@GLID", pTriplet->GetID());

      bool bResult = Application::Instance()->GetDBManager()->Execute(command);

      return bResult;

   }

   void 
   PersistentGreyList::ClearExpiredRecords()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Clears all expired triple records.
   //---------------------------------------------------------------------------()
   {
      SQLCommand command(Formatter::Format("delete from hm_greylisting_triplets where gldeletetime < {0}", SQLStatement::GetCurrentTimestamp()));

      Application::Instance()->GetDBManager()->Execute(command);
   }

   void 
   PersistentGreyList::ClearAllRecords()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Clears all triple records.
   //---------------------------------------------------------------------------()
   {
      SQLCommand command("delete from hm_greylisting_triplets");
      Application::Instance()->GetDBManager()->Execute(command);
   }

   void 
   PersistentGreyList::IncreaseBlocked(__int64 iTripletID)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Increases the blocked-count for a triple.
   //---------------------------------------------------------------------------()
   {
      SQLCommand command("update hm_greylisting_triplets set glblockedcount = glblockedcount + 1 where glid = @GLID");
      command.AddParameter("@GLID", iTripletID);
      
      Application::Instance()->GetDBManager()->Execute(command);
   }

}