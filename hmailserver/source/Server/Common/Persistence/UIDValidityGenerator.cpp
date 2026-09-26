// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "UIDValidityGenerator.h"

#include "../Util/Time.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   boost::mutex UIDValidityGenerator::mutex_;

   unsigned int
   UIDValidityGenerator::GetNext(__int64 account_id)
   {
      boost::lock_guard<boost::mutex> guard(mutex_);

      __int64 value = 0;
      bool exists = false;

      if (!GetCounter_(account_id, value, exists))
         return 0;

      value++;

      // UIDVALIDITY is a 32-bit number.
      if (value > UINT_MAX)
         return 0;

      if (!SetCounter_(account_id, value, exists))
         return 0;

      return (unsigned int) value;
   }

   bool
   UIDValidityGenerator::DeleteAccount(__int64 account_id)
   {
      boost::lock_guard<boost::mutex> guard(mutex_);

      SQLCommand command("delete from hm_uidvalidity where uidvalidityaccountid = @ACCOUNTID");
      command.AddParameter("@ACCOUNTID", account_id);

      return Application::Instance()->GetDBManager()->Execute(command);
   }

   bool
   UIDValidityGenerator::GetCounter_(__int64 account_id, __int64 &value, bool &exists)
   {
      SQLCommand command("select uidvalidityvalue from hm_uidvalidity where uidvalidityaccountid = @ACCOUNTID");
      command.AddParameter("@ACCOUNTID", account_id);

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return false;

      exists = !pRS->IsEOF();

      if (exists)
      {
         value = pRS->GetInt64Value("uidvalidityvalue");
         return true;
      }

      /*
         The account's folders so far used their creation time as UIDVALIDITY, including
         folders since deleted. Those were created no later than now, so starting at now keeps
         the values increasing. A creation time in the future is covered as well.
      */
      value = DateTime::GetCurrentTime().ToInt();

      SQLCommand newestCommand("select max(foldercreationtime) as newest from hm_imapfolders where folderaccountid = @ACCOUNTID");
      newestCommand.AddParameter("@ACCOUNTID", account_id);

      std::shared_ptr<DALRecordset> pNewestRS = Application::Instance()->GetDBManager()->OpenRecordset(newestCommand);
      if (!pNewestRS)
         return false;

      String newest = pNewestRS->IsEOF() ? String() : pNewestRS->GetStringValue("newest");
      if (!newest.IsEmpty())
         value = (std::max)(value, (__int64) Time::GetDateFromSystemDate(newest).ToInt());

      return true;
   }

   bool
   UIDValidityGenerator::SetCounter_(__int64 account_id, __int64 value, bool exists)
   {
      // SQL CE binds parameters by position, so they're added in the order the query uses them.
      if (exists)
      {
         SQLCommand command("update hm_uidvalidity set uidvalidityvalue = @VALUE where uidvalidityaccountid = @ACCOUNTID");
         command.AddParameter("@VALUE", value);
         command.AddParameter("@ACCOUNTID", account_id);

         return Application::Instance()->GetDBManager()->Execute(command);
      }

      SQLCommand command("insert into hm_uidvalidity (uidvalidityaccountid, uidvalidityvalue) values (@ACCOUNTID, @VALUE)");
      command.AddParameter("@ACCOUNTID", account_id);
      command.AddParameter("@VALUE", value);

      return Application::Instance()->GetDBManager()->Execute(command);
   }
}
