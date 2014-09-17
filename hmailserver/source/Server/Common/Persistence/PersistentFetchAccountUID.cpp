// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "../BO/FetchAccountUID.h"
#include "PersistentFetchAccountUID.h"
#include "PersistenceMode.h"
#include "../Util/Time.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentFetchAccountUID::PersistentFetchAccountUID(void)
   {
   }

   PersistentFetchAccountUID::~PersistentFetchAccountUID(void)
   {

   }

   __int64 
   PersistentFetchAccountUID::AddUID(__int64 iFAID, const String &sValue)
   {
      std::shared_ptr<FetchAccountUID> newUID = std::shared_ptr<FetchAccountUID>(new FetchAccountUID(0, iFAID, sValue, Time::GetCurrentDateTime()));

      String message;
      if (!SaveObject(newUID, message, PersistenceModeNormal))
         return 0;

      return newUID->GetID();
   }

   bool
   PersistentFetchAccountUID::SaveObject(std::shared_ptr<FetchAccountUID> pUID, String &result, PersistenceMode mode)
   {
      SQLStatement oStatement;

      oStatement.SetTable("hm_fetchaccounts_uids");

      oStatement.AddColumnInt64("uidfaid", pUID->GetAccountID());
      oStatement.AddColumn("uidvalue", pUID->GetUID());
      oStatement.AddColumnDate("uidtime", pUID->GetCreationDate());

      if (pUID->GetID() == 0)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("uidid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);
         oStatement.SetWhereClause(Formatter::Format("uidid = {0}", pUID->GetID()));
      }

      bool bNewObject = pUID->GetID() == 0;

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pUID->SetID((int) iDBID);

      return bRetVal;

   }

   bool
   PersistentFetchAccountUID::DeleteObject(std::shared_ptr<FetchAccountUID> fetchAccountUID)
   {
      return DeleteUID(fetchAccountUID->GetID());
   }

   bool
   PersistentFetchAccountUID::DeleteUID(__int64 iUIDID)
   {
      SQLCommand command("delete from hm_fetchaccounts_uids where uidid = @UIDID");
      command.AddParameter("@UIDID", iUIDID);

      return Application::Instance()->GetDBManager()->Execute(command);
   }

}