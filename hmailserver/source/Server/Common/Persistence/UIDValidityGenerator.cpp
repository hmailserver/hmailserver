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
   bool UIDValidityGenerator::loaded_ = false;
   unsigned int UIDValidityGenerator::last_uid_validity_ = 0;

   DateTime
   UIDValidityGenerator::GetNewCreationTime()
   {
      boost::lock_guard<boost::mutex> guard(mutex_);

      unsigned int last_uid_validity = GetLastUIDValidity_();

      DateTime creation_time = DateTime::GetCurrentTime();
      unsigned int uid_validity = creation_time.ToInt();

      if (uid_validity <= last_uid_validity)
      {
         creation_time = creation_time + DateTimeSpan((last_uid_validity + 1 - uid_validity) / 86400.0);
         uid_validity = creation_time.ToInt();
      }

      last_uid_validity_ = uid_validity;

      return creation_time;
   }

   unsigned int
   UIDValidityGenerator::GetLastUIDValidity_()
   {
      if (loaded_)
         return last_uid_validity_;

      SQLCommand command("select max(foldercreationtime) as newest from hm_imapfolders");

      // If the lookup fails, it's retried the next time a folder is created.
      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return last_uid_validity_;

      String newest = pRS->IsEOF() ? String() : pRS->GetStringValue("newest");
      if (!newest.IsEmpty())
         last_uid_validity_ = (std::max)(last_uid_validity_, Time::GetDateFromSystemDate(newest).ToInt());

      loaded_ = true;

      return last_uid_validity_;
   }
}
