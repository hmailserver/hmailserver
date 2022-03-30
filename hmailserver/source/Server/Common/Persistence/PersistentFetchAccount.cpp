// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\persistentfetchaccount.h"
#include "..\BO\FetchAccount.h"
#include "..\BO\FetchAccounts.h"
#include "..\BO\FetchAccountUIDs.h"
#include "..\BO\FetchAccountUID.h"
#include "..\Util\Time.h"
#include "../Util/Crypt.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentFetchAccount::PersistentFetchAccount(void)
   {
   }

   PersistentFetchAccount::~PersistentFetchAccount(void)
   {

   }
   
   void
   PersistentFetchAccount::Lock(__int64 ID)
   {
      SQLCommand command("update hm_fetchaccounts set falocked = 1 where faid = @FAID");
      command.AddParameter("@FAID", ID);

      bool bRetVal = Application::Instance()->GetDBManager()->Execute(command);
   }

   void 
   PersistentFetchAccount::Unlock(__int64 ID)
   {
      SQLCommand command("update hm_fetchaccounts set falocked = 0 where faid = @FAID");
      command.AddParameter("@FAID", ID);

      bool bRetVal = Application::Instance()->GetDBManager()->Execute(command);
   }

   void 
   PersistentFetchAccount::UnlockAll()
   {
      SQLCommand command("update hm_fetchaccounts set falocked = 0");

      bool bRetVal = Application::Instance()->GetDBManager()->Execute(command);
   }

	bool
	PersistentFetchAccount::IsLocked(__int64 ID)
	{
		SQLCommand command("select falocked from hm_fetchaccounts where faid = @FAID");
		command.AddParameter("@FAID", ID);

		std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
		if (!pRS || pRS->IsEOF())
			return false;

		return pRS->GetLongValue("falocked") == 1;
	}


   bool
   PersistentFetchAccount::ReadObject(std::shared_ptr<FetchAccount> oFA, const SQLCommand& command)
   {
      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return false;

      bool bRetVal = false;
      if (!pRS->IsEOF())
      {
         bRetVal = ReadObject(oFA, pRS);
      }

      return bRetVal;
   }

   bool
   PersistentFetchAccount::ReadObject(std::shared_ptr<FetchAccount> oFA, std::shared_ptr<DALRecordset> pRS)
   {

      if (pRS->IsEOF())
         return false;

      oFA->SetID(pRS->GetLongValue("faid"));
      oFA->SetActive(pRS->GetLongValue("faactive") == 1);
      oFA->SetAccountID(pRS->GetLongValue("faaccountid"));
      oFA->SetName(pRS->GetStringValue("faaccountname"));
      oFA->SetServerAddress(pRS->GetStringValue("faserveraddress"));
      oFA->SetPort(pRS->GetLongValue("faserverport"));
      oFA->SetServerType((HM::FetchAccount::ServerType)pRS->GetLongValue("faservertype"));
      oFA->SetUsername(pRS->GetStringValue("fausername"));
      oFA->SetPassword(Crypt::Instance()->DeCrypt(pRS->GetStringValue("fapassword"), Crypt::ETBlowFish));
      oFA->SetMinutesBetweenTry(pRS->GetLongValue("faminutes"));
      oFA->SetDaysToKeep(pRS->GetLongValue("fadaystokeep"));
      oFA->SetMIMERecipientHeaders(pRS->GetStringValue("famimerecipientheaders"));
      oFA->SetProcessMIMERecipients(pRS->GetLongValue("faprocessmimerecipients") == 1);
      oFA->SetProcessMIMEDate(pRS->GetLongValue("faprocessmimedate") == 1);
      oFA->SetConnectionSecurity((ConnectionSecurity) pRS->GetLongValue("faconnectionsecurity"));
      oFA->SetNextTry(pRS->GetStringValue("fanexttry"));
      oFA->SetUseAntiSpam(pRS->GetLongValue("fauseantispam") == 1);
      oFA->SetUseAntiVirus(pRS->GetLongValue("fauseantivirus") == 1);
      oFA->SetEnableRouteRecipients(pRS->GetLongValue("faenablerouterecipients") == 1);

      
      return true;
   }

   bool
   PersistentFetchAccount::DeleteObject(std::shared_ptr<FetchAccount> pFA)
   {
      SQLCommand command("delete from hm_fetchaccounts where faid = @FAID");
      command.AddParameter("@FAID", pFA->GetID());

      if (!Application::Instance()->GetDBManager()->Execute(command))
         return false;
      
      SQLCommand command2("delete from hm_fetchaccounts_uids where uidfaid = @UIDFAID");
      command2.AddParameter("@UIDFAID", pFA->GetID());

      if (!Application::Instance()->GetDBManager()->Execute(command2))
         return false;

      return true;

   }

   void
   PersistentFetchAccount::DeleteByAccountID(__int64 iAccountID)
   {
      std::shared_ptr<FetchAccounts> pFetchAccounts = std::shared_ptr<FetchAccounts>(new FetchAccounts(iAccountID));
      pFetchAccounts->Refresh();
      pFetchAccounts->DeleteAll();
   }

   bool 
   PersistentFetchAccount::SaveObject(std::shared_ptr<FetchAccount> pFA, String &errorMessage, PersistenceMode mode)
   {
      // errorMessage - Not supported yet.
      return SaveObject(pFA);
   }

   bool 
   PersistentFetchAccount::SaveObject(std::shared_ptr<FetchAccount> pFA)
   {
      SQLStatement oStatement;
      oStatement.SetTable("hm_fetchaccounts");

      bool bNewObject = pFA->GetID() == 0;

      oStatement.AddColumnInt64("faaccountid", pFA->GetAccountID());
      oStatement.AddColumn("faactive", pFA->GetActive() ? 1 : 0);
      oStatement.AddColumn("faaccountname", pFA->GetName());
      oStatement.AddColumn("faserveraddress", pFA->GetServerAddress());
      oStatement.AddColumn("faserverport", pFA->GetPort());
      oStatement.AddColumn("faservertype", pFA->GetServerType());
      oStatement.AddColumn("fausername", pFA->GetUsername());
      oStatement.AddColumn("fapassword", Crypt::Instance()->EnCrypt(pFA->GetPassword(), Crypt::ETBlowFish));
      oStatement.AddColumn("faminutes", pFA->GetMinutesBetweenTry());
      oStatement.AddColumn("fadaystokeep", pFA->GetDaysToKeep());
      oStatement.AddColumn("fanexttry", Time::GetCurrentDateTime());
      oStatement.AddColumn("famimerecipientheaders", pFA->GetMIMERecipientHeaders());
      oStatement.AddColumn("faprocessmimerecipients", pFA->GetProcessMIMERecipients());
      oStatement.AddColumn("faprocessmimedate", pFA->GetProcessMIMEDate());
      oStatement.AddColumn("faconnectionsecurity", pFA->GetConnectionSecurity());
      oStatement.AddColumn("fauseantispam", pFA->GetUseAntiSpam());
      oStatement.AddColumn("fauseantivirus", pFA->GetUseAntiVirus());
      oStatement.AddColumn("faenablerouterecipients", pFA->GetEnableRouteRecipients());

      if (bNewObject)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("faid");

         oStatement.AddColumn("falocked", 0);
      }
      else
      {
         String sWhere;
         sWhere.Format(_T("faid = %I64d"), pFA->GetID());

         oStatement.SetStatementType(SQLStatement::STUpdate);
         oStatement.SetWhereClause(sWhere);
      }

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pFA->SetID((int) iDBID);

      return bRetVal;
   }

   void 
   PersistentFetchAccount::SetRetryNow(__int64 iFAID)
   {
      SQLCommand command(Formatter::Format("update hm_fetchaccounts set fanexttry = {0} where faid = @FAID", SQLStatement::GetCurrentTimestamp()));

      command.AddParameter("@FAID", iFAID);
      Application::Instance()->GetDBManager()->Execute(command);
   }
      
   void
   PersistentFetchAccount::SetNextTryTime(std::shared_ptr<FetchAccount> pFA)
   {
      SQLCommand command;

      switch (IniFileSettings::Instance()->GetDatabaseType())
      {
      case DatabaseSettings::TypeMYSQLServer:
         command.SetQueryString(_T("update hm_fetchaccounts set fanexttry = DATE_ADD(CONCAT(CURDATE(), ' ', CURTIME()), INTERVAL faminutes MINUTE) where faid = @FAID"));
         break;
      case DatabaseSettings::TypeMSSQLServer:
      case DatabaseSettings::TypeMSSQLCompactEdition:
         command.SetQueryString(_T("update hm_fetchaccounts set fanexttry = DATEADD(mi, faminutes, GETDATE()) where faid = @FAID"));
         break;
      case DatabaseSettings::TypePGServer:
         command.SetQueryString(_T("update hm_fetchaccounts set fanexttry = current_timestamp + INTERVAL '@MINUTES minutes' where faid = @FAID"));
         command.AddParameter("@MINUTES", pFA->GetMinutesBetweenTry());
         break;
      }

      command.AddParameter("@FAID", pFA->GetID());

      Application::Instance()->GetDBManager()->Execute(command);
   }
}