// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "persistentdistributionlist.h"

#include "../Cache/Cache.h"

#include "persistentdistributionlistRecipient.h"
#include "PreSaveLimitationsCheck.h"

#include "../bo/DistributionListRecipients.h"
#include "../bo/DistributionList.h"



#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentDistributionList::PersistentDistributionList(void)
   {
   
   }

   PersistentDistributionList::~PersistentDistributionList(void)
   {

   }

   bool
   PersistentDistributionList::DeleteObject(std::shared_ptr<DistributionList> pDistList)
   {
      if (pDistList->GetID() == 0)
      {
         assert(0);
         return false;
      }

      DeleteMembers(pDistList);

      SQLCommand deleteCommand("delete from hm_distributionlists where distributionlistid = @LISTID");
      deleteCommand.AddParameter("@LISTID", pDistList->GetID());

      bool bResult = Application::Instance()->GetDBManager()->Execute(deleteCommand);
   
      Cache<DistributionList>::Instance()->RemoveObject(pDistList);

      return bResult;
   }

   bool
   PersistentDistributionList::ReadObject(std::shared_ptr<DistributionList> pDistList, const String &sAddress)
   {
      SQLStatement statement;

      statement.SetStatementType(SQLStatement::STSelect);
      statement.SetTable("hm_distributionlists");
      statement.AddWhereClauseColumn("distributionlistaddress", sAddress);

      return ReadObject(pDistList, statement.GetCommand());
   }  

   bool
   PersistentDistributionList::ReadObject(std::shared_ptr<DistributionList> pDistList, __int64 iID)
   {
      SQLCommand selectCommand("select * from hm_distributionlists where distributionlistid = @LISTID");
      selectCommand.AddParameter("@LISTID", iID);
  
      return ReadObject(pDistList, selectCommand);
   }

   bool
   PersistentDistributionList::ReadObject(std::shared_ptr<DistributionList> pDistList, const SQLCommand &command)
   {
      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return false;

      bool bRetVal = false;
      if (!pRS->IsEOF())
      {
         bRetVal = ReadObject(pDistList, pRS);
      }

   
      return bRetVal;
   }



   bool
   PersistentDistributionList::ReadObject(std::shared_ptr<DistributionList> pDistList, std::shared_ptr<DALRecordset> pRS)
   {
      pDistList->SetID(pRS->GetLongValue("distributionlistid"));
      pDistList->SetDomainID(pRS->GetLongValue("distributionlistdomainid"));
      pDistList->SetActive(pRS->GetLongValue("distributionlistenabled") ? true : false);
      pDistList->SetAddress(pRS->GetStringValue("distributionlistaddress"));
      pDistList->SetRequireAuth(pRS->GetLongValue("distributionlistrequireauth") ? true : false);
      pDistList->SetRequireAddress(pRS->GetStringValue("distributionlistrequireaddress"));
      pDistList->SetListMode((HM::DistributionList::ListMode) pRS->GetLongValue("distributionlistmode"));

      return true;
   }

   bool
   PersistentDistributionList::SaveObject(std::shared_ptr<DistributionList> pDistList)
   {
      String sErrorMessage;
      return SaveObject(pDistList, sErrorMessage, PersistenceModeNormal);
   }

   bool
   PersistentDistributionList::SaveObject(std::shared_ptr<DistributionList> pDistList, String &sErrorMessage, PersistenceMode mode)
   {
      if (!PreSaveLimitationsCheck::CheckLimitations(mode, pDistList, sErrorMessage))
         return false;

      SQLStatement oStatement;
      oStatement.SetTable("hm_distributionlists");

      oStatement.AddColumnInt64("distributionlistdomainid", pDistList->GetDomainID());
      oStatement.AddColumn("distributionlistenabled", pDistList->GetActive() ? 1 : 0);
      oStatement.AddColumn("distributionlistaddress", pDistList->GetAddress());
      oStatement.AddColumn("distributionlistrequireauth", pDistList->GetRequireAuth() ? 1 : 0);
      oStatement.AddColumn("distributionlistrequireaddress", pDistList->GetRequireAddress());
      oStatement.AddColumn("distributionlistmode", pDistList->GetListMode());

      if (pDistList->GetID() == 0)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("distributionlistid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);

         String sWhere;
         sWhere.Format(_T("distributionlistid = %I64d"), pDistList->GetID());
         oStatement.SetWhereClause(sWhere);
      }

      bool bNewObject = pDistList->GetID() == 0;

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pDistList->SetID((int) iDBID);

      Cache<DistributionList>::Instance()->RemoveObject(pDistList);

      return bRetVal;
   }
   
   bool
   PersistentDistributionList::DeleteMembers(std::shared_ptr<DistributionList> pDistList)
   {
      return PersistentDistributionListRecipient::DeleteByListID(pDistList->GetID());     
   }


}