// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "AccountLogon.h"
#include "PasswordValidator.h"
#include "../Persistence/PersistentLogonFailure.h"
#include "../Persistence/PersistentAccount.h"
#include "../Persistence/PersistentSecurityRange.h"

#include "../BO/SecurityRange.h"
#include "../BO/Account.h"
#include "VariantDateTime.h"
#include "Time.h"
#include "GUIDCreator.h"
#include "PasswordGenerator.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   boost::recursive_mutex AccountLogon::ip_range_creation_mutex_;

   AccountLogon::AccountLogon(void)
   {
   }

   AccountLogon::~AccountLogon(void)
   {
   }

   std::shared_ptr<const Account>
   AccountLogon::Logon(const IPAddress &ipaddress, const String &username, const String &password, bool &disconnect)
   {
	   return Logon(ipaddress, _T(""), username, password, disconnect);
   }

   std::shared_ptr<const Account>
   AccountLogon::Logon(const IPAddress &ipaddress, const String &masqname, const String &username, const String &password, bool &disconnect)
   {
      disconnect = false;

      std::shared_ptr<const Account> account = PasswordValidator::ValidatePassword(masqname, username, password);
      if (account)
      {
         PersistentAccount::UpdateLastLogonTime(account);
         return account;
      }

      // is auto banning enabled?
      int maxInvalidLogonAttempts = Configuration::Instance()->GetMaxInvalidLogonAttempts();

      if (Configuration::Instance()->GetAutoBanLogonEnabled() == false || maxInvalidLogonAttempts == 0)
      {
         std::shared_ptr<Account> empty;
         return empty;
      }

      // Log on has failed.
      PersistentLogonFailure logonFailure;
      int failureCount = logonFailure.GetCurrrentFailureCount(ipaddress) + 1; // +1 because we count the current failure as well.

      if (failureCount >= maxInvalidLogonAttempts)
      {
         logonFailure.ClearFailuresByIP(ipaddress);

         int minutes = Configuration::Instance()->GetAutoBanMinutes();
         if (minutes == 0)
         {
            // disconnect, but don't block.
            disconnect = true;
            std::shared_ptr<Account> empty;
            return empty;
         }

         CreateIPRange(ipaddress, username, minutes);

         disconnect = true;

         // logon failed!
         std::shared_ptr<Account> empty;
         return empty;
      }

      // logon failed, add new failure.
      logonFailure.AddFailure(ipaddress);

      std::shared_ptr<Account> empty;
      return empty;
   }

   void
   AccountLogon::CreateIPRange(const IPAddress &ipaddress, const String &username, int minutes)
   {
      // Why do we need a lock here? IP ranges requires unique names.
      // To prevent conflicts, we need to auto-generate names. We could
      // create random names, but that would be ugly. Instead, we create
      // names containing just an index.
      //
      // The problem is if two clients are banned at the same time. There
      // is a risk that we create two IP ranges with the same name, which
      // will result in a DB error. To prevent this, we use locking to
      // ensure that only one IP range is created at a time.

      boost::lock_guard<boost::recursive_mutex> guard(ip_range_creation_mutex_);

      DateTime dt = DateTime::GetCurrentTime();         
      DateTimeSpan span;
      span.SetDateTimeSpan(0,0,minutes,0);
      dt = dt + span;


      std::shared_ptr<SecurityRange> pSecurityRange = std::shared_ptr<SecurityRange>(new SecurityRange);
      pSecurityRange->SetName(GetIPRangeName_(username));
      pSecurityRange->SetPriority(100);
      pSecurityRange->SetLowerIP(ipaddress);
      pSecurityRange->SetUpperIP(ipaddress);
      pSecurityRange->SetExpires(true);
      pSecurityRange->SetExpiresTime(dt);
      PersistentSecurityRange::SaveObject(pSecurityRange);


   }      

   String 
   AccountLogon::GetIPRangeName_(const String &username)
   {
      String suggestion = "Auto-ban: " + username;
      if (!PersistentSecurityRange::Exists(suggestion))
         return suggestion;

      for (int i = 1; i <= 10; i++)
      {
         suggestion = "Auto-ban: " + username + " (" + StringParser::IntToString(i)+ ")";
         if (!PersistentSecurityRange::Exists(suggestion))
            return suggestion;
      }

      for (int i = 1; i <= 10; i++)
      {
         String pass = PasswordGenerator::Generate();
         pass = pass.Mid(0,9);
         suggestion = "Auto-ban: " + username + " (Z" + pass + ")";
         if (!PersistentSecurityRange::Exists(suggestion))
            return suggestion;
      }

      // okay. we failed finding a unique password. not very likely.
      suggestion = "Auto-ban: " + username + " " + GUIDCreator::GetGUID() + "";
      return suggestion;
   }

}