// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "persistentdistributionlistrecipient.h"

#include "../Cache/Cache.h"

#include "PersistentDistributionList.h"
#include "PreSaveLimitationsCheck.h"
#include "PersistenceMode.h"

#include "../BO/DistributionList.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentDistributionListRecipient::PersistentDistributionListRecipient(void)
   {
   }

   PersistentDistributionListRecipient::~PersistentDistributionListRecipient(void)
   {
   }

   bool
   PersistentDistributionListRecipient::DeleteObject(std::shared_ptr<DistributionListRecipient> pRecipient)
   {
      if (pRecipient->GetID() == 0)
      {
         assert(0);
         return false;
      }

      SQLCommand command("delete from hm_distributionlistsrecipients where distributionlistrecipientid = @RECIPIENTID");
      command.AddParameter("@RECIPIENTID", pRecipient->GetID());

      bool bResult = Application::Instance()->GetDBManager()->Execute(command);

      Cache<DistributionList>::Instance()->RemoveObject(pRecipient->GetListID());

      return bResult;
   }

   bool 
   PersistentDistributionListRecipient::DeleteByListID(__int64 iListID)
   {
      SQLCommand command("delete from hm_distributionlistsrecipients where distributionlistrecipientlistid = @LISTID");
      command.AddParameter("@LISTID", iListID);

      return Application::Instance()->GetDBManager()->Execute(command);
   }

   bool
   PersistentDistributionListRecipient::ReadObject(std::shared_ptr<DistributionListRecipient> pRecipient, long ObjectID)
   {
      SQLCommand selectCommand(_T("select * from hm_distributionlistsrecipients where distributionlistrecipientid = @RECIPIENTID"));
      selectCommand.AddParameter("@RECIPIENTID", ObjectID);

      return ReadObject(pRecipient, selectCommand);
   }

   bool
   PersistentDistributionListRecipient::ReadObject(std::shared_ptr<DistributionListRecipient> pRecipient, const SQLCommand &command)
   {
      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);

      bool bRetVal = false;
      if (!pRS->IsEOF())
      {
         bRetVal = ReadObject(pRecipient, pRS);
      }

      return bRetVal;
   }



   bool
   PersistentDistributionListRecipient::ReadObject(std::shared_ptr<DistributionListRecipient> pRecipient, std::shared_ptr<DALRecordset> pRS)
   {
      pRecipient->SetID(pRS->GetLongValue("distributionlistrecipientid"));
      pRecipient->SetListID(pRS->GetLongValue("distributionlistrecipientlistid"));
      pRecipient->SetAddress(pRS->GetStringValue("distributionlistrecipientaddress"));

      return true;
   }

   bool
   PersistentDistributionListRecipient::SaveObject(std::shared_ptr<DistributionListRecipient> pRecipient)
   {
      String sErrorMessage;

      return SaveObject(pRecipient, sErrorMessage, PersistenceModeNormal);
   }


   bool
   PersistentDistributionListRecipient::SaveObject(std::shared_ptr<DistributionListRecipient> pRecipient, String &sErrorMessage, PersistenceMode mode)
   {
      if (!PreSaveLimitationsCheck::CheckLimitations(mode, pRecipient, sErrorMessage))
         return false;

      SQLStatement oStatement;
      oStatement.SetTable("hm_distributionlistsrecipients");

      oStatement.AddColumnInt64("distributionlistrecipientlistid", pRecipient->GetListID());
      oStatement.AddColumn("distributionlistrecipientaddress", pRecipient->GetAddress());

      if (pRecipient->GetID() == 0)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("distributionlistrecipientid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);

         String sWhere;
         sWhere.Format(_T("distributionlistrecipientid = %I64d"), pRecipient->GetID());
         oStatement.SetWhereClause(sWhere);
      }

      bool bNewObject = pRecipient->GetID() == 0;

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pRecipient->SetID((int) iDBID);

      Cache<DistributionList>::Instance()->RemoveObject(pRecipient->GetListID());

      return bRetVal;
   }

}