// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "PersistentARCTrustedSealer.h"
#include "../BO/ARCTrustedSealer.h"
#include "../SQL/SQLStatement.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool
   PersistentARCTrustedSealer::DeleteObject(std::shared_ptr<ARCTrustedSealer> pObject)
   {
      SQLCommand command("delete from hm_arc_trusted_sealers where sealerid = @SEALERID");
      command.AddParameter("@SEALERID", pObject->GetID());

      return Application::Instance()->GetDBManager()->Execute(command);
   }

   bool
   PersistentARCTrustedSealer::ReadObject(std::shared_ptr<ARCTrustedSealer> pObject, std::shared_ptr<DALRecordset> pRS)
   {
      pObject->SetID(pRS->GetInt64Value("sealerid"));
      pObject->SetDomain(pRS->GetStringValue("sealerdomain"));
      pObject->SetDescription(pRS->GetStringValue("sealerdescription"));

      return true;
   }

   bool
   PersistentARCTrustedSealer::SaveObject(std::shared_ptr<ARCTrustedSealer> pObject, String &errorMessage, PersistenceMode mode)
   {
      // errorMessage - not supported yet.
      return SaveObject(pObject);
   }

   bool
   PersistentARCTrustedSealer::SaveObject(std::shared_ptr<ARCTrustedSealer> pObject)
   {
      SQLStatement oStatement;
      oStatement.SetTable("hm_arc_trusted_sealers");

      if (pObject->GetID() == 0)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("sealerid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);
         String sWhere;
         sWhere.Format(_T("sealerid = %I64d"), pObject->GetID());
         oStatement.SetWhereClause(sWhere);
      }

      String domain = pObject->GetDomain();
      domain.Trim();

      oStatement.AddColumn("sealerdomain", domain);
      oStatement.AddColumn("sealerdescription", pObject->GetDescription());

      bool bNewObject = pObject->GetID() == 0;

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pObject->SetID(iDBID);

      return bRetVal;
   }
}
