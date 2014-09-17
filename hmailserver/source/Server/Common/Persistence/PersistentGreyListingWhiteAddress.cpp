// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "PersistentGreyListingWhiteAddress.h"
#include "..\BO\GreyListingWhiteAddress.h"
#include "..\SQL\SQLStatement.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentGreyListingWhiteAddress::PersistentGreyListingWhiteAddress(void)
   {
   }

   PersistentGreyListingWhiteAddress::~PersistentGreyListingWhiteAddress(void)
   {
   }

   bool
   PersistentGreyListingWhiteAddress::DeleteObject(std::shared_ptr<GreyListingWhiteAddress> pObject)
   {
      SQLCommand command("delete from hm_greylisting_whiteaddresses where whiteid = @WHITEID");
      command.AddParameter("@WHITEID", pObject->GetID());

      return Application::Instance()->GetDBManager()->Execute(command);
   }

   bool 
   PersistentGreyListingWhiteAddress::ReadObject(std::shared_ptr<GreyListingWhiteAddress> pObject, std::shared_ptr<DALRecordset> pRS)
   {
      pObject->SetID (pRS->GetLongValue("whiteid"));
      pObject->SetIPAddress(pRS->GetStringValue("whiteipaddress"));
      pObject->SetDescription(pRS->GetStringValue("whiteipdescription"));

      return true;
   }

   bool 
   PersistentGreyListingWhiteAddress::SaveObject(std::shared_ptr<GreyListingWhiteAddress> pObject, String &errorMessage, PersistenceMode mode)
   {
      // errorMessage - not supported yet.
      return SaveObject(pObject);
   }

   bool 
   PersistentGreyListingWhiteAddress::SaveObject(std::shared_ptr<GreyListingWhiteAddress> pObject)
   {
      SQLStatement oStatement;
      oStatement.SetTable("hm_greylisting_whiteaddresses");
      
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

      oStatement.AddColumn("whiteipaddress", pObject->GetIPAddress());
      oStatement.AddColumn("whiteipdescription", pObject->GetDescription());

      bool bNewObject = pObject->GetID() == 0;

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pObject->SetID((int) iDBID);

      return true;
   }

   /// Checks if a specific sender on a specific IP address is white listed.
   bool 
   PersistentGreyListingWhiteAddress::IsSenderWhitelisted(const String &address)
   {
     SQLCommand command1("SELECT COUNT(*) as c FROM hm_greylisting_whiteaddresses "
         "WHERE @ADDRESS LIKE whiteipaddress ESCAPE '/'");

      command1.AddParameter("@ADDRESS", address);
   
      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command1);
      
      if (!pRS)
         return false;

      long count = pRS->GetLongValue("c");

      if (count > 0)
         return true;
      else 
         return false;
   }
}