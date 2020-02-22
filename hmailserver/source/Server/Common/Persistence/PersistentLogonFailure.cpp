// Copyright (c) 2009 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "PersistentLogonFailure.h"
#include "..\SQL\IPAddressSQLHelper.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM   
{
   PersistentLogonFailure::PersistentLogonFailure()
   {

   }

   PersistentLogonFailure::~PersistentLogonFailure()
   {

   }

   int 
   PersistentLogonFailure::GetCurrrentFailureCount(const IPAddress &ipaddress)
   {
      IPAddressSQLHelper helper;

      String sql;
      sql.Format(_T("select count(*) as c from hm_logon_failures where ipaddress1 %s and ipaddress2 %s"), 
         String(helper.GetAddress1Equals(ipaddress)).c_str(),
         String(helper.GetAddress2Equals(ipaddress)).c_str());

      SQLCommand command(sql);

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return 0;

      long count = pRS->GetLongValue("c");

      return count;
   }
   
   bool 
   PersistentLogonFailure::AddFailure(const IPAddress &ipaddress)
   {
      SQLStatement statement;

      IPAddressSQLHelper helper;
      helper.AppendStatement(statement, ipaddress, "ipaddress1", "ipaddress2");

      statement.AddColumnCommand("failuretime", SQLStatement::GetCurrentTimestamp());
      statement.SetStatementType(SQLStatement::STInsert);
      statement.SetTable("hm_logon_failures");

      return Application::Instance()->GetDBManager()->Execute(statement);
   }

   bool 
   PersistentLogonFailure::ClearFailuresByIP(const IPAddress &ipaddress)
   {
      IPAddressSQLHelper helper;

      String whereClause;
      whereClause.Format(_T("ipaddress1 %s and ipaddress2 %s"), 
         String(helper.GetAddress1Equals(ipaddress)).c_str(),
         String(helper.GetAddress2Equals(ipaddress)).c_str());

      SQLStatement statement;
      statement.SetStatementType(SQLStatement::STDelete);
      statement.SetWhereClause(whereClause);
      statement.SetTable("hm_logon_failures");

      return Application::Instance()->GetDBManager()->Execute(statement);
   }

   bool 
   PersistentLogonFailure::ClearOldFailures(int olderThanMinutes)
   {
      String whereClause;
      whereClause.Format(_T("failuretime < %s"), SQLStatement::GetCurrentTimestampPlusMinutes(-olderThanMinutes).c_str());

      SQLStatement statement;
      statement.SetStatementType(SQLStatement::STDelete);
      statement.SetWhereClause(whereClause);
      statement.SetTable("hm_logon_failures");

      return Application::Instance()->GetDBManager()->Execute(statement);

   }
}