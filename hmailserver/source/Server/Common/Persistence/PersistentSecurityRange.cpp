// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "PersistentSecurityRange.h"
#include "PersistenceMode.h"

#include "../BO/SecurityRange.h"

#include "../Util/Time.h"

#include "../SQL/IPAddressSQLHelper.h"
#include "PreSaveLimitationsCheck.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentSecurityRange::PersistentSecurityRange()
   {

   }

   PersistentSecurityRange::~PersistentSecurityRange()
   {

   }

   bool
   PersistentSecurityRange::DeleteObject(std::shared_ptr<SecurityRange> pSR)
   {
      assert(pSR->GetID());

      bool bResult = false;
      if (pSR->GetID() > 0)
      {
         SQLCommand command("delete from hm_securityranges where rangeid = @RANGEID");
         command.AddParameter("@RANGEID", pSR->GetID());
         
         bResult = Application::Instance()->GetDBManager()->Execute(command);
      }

      return bResult;

   }

   bool
   PersistentSecurityRange::SaveObject(std::shared_ptr<SecurityRange> pSR)
   {
      String result;

      return SaveObject(pSR, result, PersistenceModeNormal);
   }

   bool
   PersistentSecurityRange::SaveObject(std::shared_ptr<SecurityRange> pSR, String &result,  PersistenceMode mode)
   {
      if (!Validate(pSR, result))
         return false;

      if (!PreSaveLimitationsCheck::CheckLimitations(mode, pSR, result))
         return false;

      DateTime rangeExpiresTime = pSR->GetExpiresTime();
      if (rangeExpiresTime.GetStatus() != DateTime::valid)
         rangeExpiresTime.SetDateTime(2001,01,01,0,0,0);

      String name = pSR->GetName();
      if (name.GetLength() > 100)
         name = name.Mid(0, 100);

      IPAddressSQLHelper helper;

      SQLStatement oStatement;
      oStatement.SetTable("hm_securityranges");

      oStatement.AddColumn("rangename", name);
      oStatement.AddColumn("rangepriorityid", pSR->GetPriority());

      helper.AppendStatement(oStatement, pSR->GetLowerIP(), "rangelowerip1", "rangelowerip2");
      helper.AppendStatement(oStatement, pSR->GetUpperIP(), "rangeupperip1", "rangeupperip2");

      oStatement.AddColumn("rangeoptions", pSR->GetOptions());
      oStatement.AddColumn("rangeexpires", pSR->GetExpires());
      oStatement.AddColumn("rangeexpirestime", Time::GetTimeStampFromDateTime(rangeExpiresTime));

      if (pSR->GetID() == 0)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("rangeid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);

         String sWhere;
         sWhere.Format(_T("rangeid = %I64d"), pSR->GetID());
         oStatement.SetWhereClause(sWhere);
      }


      bool bNewObject = pSR->GetID() == 0;

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pSR->SetID((int) iDBID);

      if (!bRetVal)
         result = "Failed to save. Please see the hMailServer error log for details.";

      return bRetVal;
   }

   bool
   PersistentSecurityRange::ReadObject(std::shared_ptr<SecurityRange> pSR, __int64 lDBID)
   {
      SQLCommand command(_T("select * from hm_securityranges where rangeid = @RANGEID"));
      command.AddParameter("@RANGEID", lDBID);

      return ReadObject(pSR, command);
   }

   bool
   PersistentSecurityRange::ReadObject(std::shared_ptr<SecurityRange> pSR, const String &name)
   {
      SQLCommand command(_T("select * from hm_securityranges where rangename = @RANGENAME"));
      command.AddParameter("@RANGENAME", name);

      return ReadObject(pSR, command);
   }

   bool
   PersistentSecurityRange::ReadObject(std::shared_ptr<SecurityRange> pSR, const SQLCommand &command)
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
   PersistentSecurityRange::ReadObject(std::shared_ptr<SecurityRange> pSR, std::shared_ptr<DALRecordset> pRS)
   {
      IPAddressSQLHelper helper;

      pSR->SetID(pRS->GetLongValue("rangeid"));
      pSR->SetPriority(pRS->GetLongValue("rangepriorityid"));
      pSR->SetLowerIP(helper.Construct(pRS, "rangelowerip1", "rangelowerip2"));
      pSR->SetUpperIP(helper.Construct(pRS, "rangeupperip1", "rangeupperip2"));
      pSR->SetOptions(pRS->GetLongValue("rangeoptions"));
      pSR->SetName(pRS->GetStringValue("rangename"));
      
      pSR->SetExpires(pRS->GetLongValue("rangeexpires") == 1);
      pSR->SetExpiresTime(Time::GetDateFromSystemDate(pRS->GetStringValue("rangeexpirestime")));

      return true;
   }

   std::shared_ptr<SecurityRange>
   PersistentSecurityRange::ReadMatchingIP(const IPAddress &ipaddress)
   {
      std::shared_ptr<SecurityRange> empty;

      IPAddressSQLHelper helper;
      String sSQL;

      if (ipaddress.GetType() == IPAddress::IPV4)
      {
         std::shared_ptr<SecurityRange> pSR = std::shared_ptr<SecurityRange>(new SecurityRange());

         sSQL.Format(_T("select * from hm_securityranges where %s >= rangelowerip1 and %s <= rangeupperip1 and rangelowerip2 IS NULL and rangeupperip2 IS NULL order by rangepriorityid desc"), 
            String(helper.GetAddress1String(ipaddress)).c_str(), String(helper.GetAddress1String(ipaddress)).c_str());

         if (!ReadObject(pSR, SQLCommand(sSQL)))
            return empty;

         return pSR;
      }
      else
      {
         // Read all IPv6 items.
         std::shared_ptr<SecurityRange> bestMatch;

         SQLCommand command(_T("select * from hm_securityranges where rangelowerip2 is not null order by rangepriorityid desc"));
         
         std::shared_ptr<DALRecordset> recordset = Application::Instance()->GetDBManager()->OpenRecordset(command);
         if (!recordset)
            return empty;

         while (!recordset->IsEOF())
         {
            std::shared_ptr<SecurityRange> securityRange = std::shared_ptr<SecurityRange>(new SecurityRange());

            if (ReadObject(securityRange, recordset) == false)
               return empty;

            if (ipaddress.WithinRange(securityRange->GetLowerIP(), securityRange->GetUpperIP()))
            {
               // This IP range matches the client. Does it have higher prio than the currently
               // matching?

               if (!bestMatch || securityRange->GetPriority() > bestMatch->GetPriority())
                  bestMatch = securityRange;
            }

            recordset->MoveNext();
         }

         return bestMatch;
      }


      
   }


   bool 
   PersistentSecurityRange::DeleteExpired()
   {
      SQLCommand command(_T("delete from hm_securityranges where rangeexpires = 1 AND rangeexpirestime < @TIME"));
      command.AddParameter("@TIME", Time::GetCurrentDateTime());

      return Application::Instance()->GetDBManager()->Execute(command);
   }

   bool
   PersistentSecurityRange::Exists(const String &name)
   {
      String whereClause = "rangename = '" + SQLStatement::Escape(name) + "'";

      SQLStatement oStatement;
      oStatement.SetStatementType(SQLStatement::STSelect);
      oStatement.SetTable("hm_securityranges");
      oStatement.AddColumn("count(*) as c");
      oStatement.SetWhereClause(whereClause);

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(oStatement);
      if (!pRS)
         return false;

      bool bRetVal = false;
      if (!pRS->IsEOF())
      {
         int count = pRS->GetLongValue("c");
         if (count > 0)
            return true;
      }

      return false;
   }

   bool 
   PersistentSecurityRange::Validate(std::shared_ptr<SecurityRange> pSR, String &result)
   {
      if (pSR->GetName().IsEmpty())
      {
         result = "The name cannot be empty.";
         return false;
      }

      if (pSR->GetLowerIP().GetType() != pSR->GetUpperIP().GetType())
      {
         result = "The lower IP address and upper IP address must be of the same IP version type.";
         return false;
      }
      
      if (pSR->GetLowerIP() > pSR->GetUpperIP())
      {
         result = "The lower IP address must be lower or the same as the upper IP address.";
         return false;
      }

      return true;
   }
}