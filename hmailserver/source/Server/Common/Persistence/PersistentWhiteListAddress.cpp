// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "PersistentWhiteListAddress.h"
#include "..\BO\WhiteListAddress.h"
#include "..\AntiSpam\WhiteListCache.h"
#include "..\SQL\SQLStatement.h"
#include "../SQL/IPAddressSQLHelper.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentWhiteListAddress::PersistentWhiteListAddress(void)
   {
   }

   PersistentWhiteListAddress::~PersistentWhiteListAddress(void)
   {
   }

   bool
   PersistentWhiteListAddress::DeleteObject(std::shared_ptr<WhiteListAddress> pObject)
   {
      SQLCommand command("delete from hm_whitelist where whiteid = @WHITEID");
      command.AddParameter("@WHITEID", pObject->GetID());

      return Application::Instance()->GetDBManager()->Execute(command);
   }

   bool 
   PersistentWhiteListAddress::ReadObject(std::shared_ptr<WhiteListAddress> pObject, std::shared_ptr<DALRecordset> pRS)
   {
      IPAddressSQLHelper helper;

      pObject->SetID (pRS->GetLongValue("whiteid"));
      pObject->SetLowerIPAddress(helper.Construct(pRS, "whiteloweripaddress1", "whiteloweripaddress2"));
      pObject->SetUpperIPAddress(helper.Construct(pRS, "whiteupperipaddress1", "whiteupperipaddress2"));
      pObject->SetEMailAddress(pRS->GetStringValue("whiteemailaddress"));
      pObject->SetDescription(pRS->GetStringValue("whitedescription"));
      
      return true;
   }

   bool 
   PersistentWhiteListAddress::SaveObject(std::shared_ptr<WhiteListAddress> pObject, String &errorMessage, PersistenceMode mode)
   {
      return SaveObject(pObject);
   }

   bool 
   PersistentWhiteListAddress::SaveObject(std::shared_ptr<WhiteListAddress> pObject)
   {
      SQLStatement oStatement;
      oStatement.SetTable("hm_whitelist");
      
      if (pObject->GetID() == 0)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("whiteid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);
         String sWhere;
         sWhere.Format(_T("whiteid = %I64d"), pObject->GetID());
         oStatement.SetWhereClause(sWhere);
      }

      IPAddressSQLHelper helper;
      helper.AppendStatement(oStatement, pObject->GetLowerIPAddress(), "whiteloweripaddress1", "whiteloweripaddress2");
      helper.AppendStatement(oStatement, pObject->GetUpperIPAddress(), "whiteupperipaddress1", "whiteupperipaddress2");

      oStatement.AddColumn("whiteemailaddress", pObject->GetEmailAddress());
      oStatement.AddColumn("whitedescription", pObject->GetDescription());

      bool bNewObject = pObject->GetID() == 0;

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pObject->SetID((int) iDBID);

      WhiteListCache::SetNeedRefresh();

      return true;
   }
}