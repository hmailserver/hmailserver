// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Account;

   class AccountLogon
   {
   public:
      AccountLogon(void);
      ~AccountLogon(void);

      std::shared_ptr<const Account> Logon(const IPAddress &ipaddress, const String &sUsername, const String &sPassword, bool &disconnect);
      std::shared_ptr<const Account> Logon(const IPAddress &ipaddress, const String &sMasqname, const String &sUsername, const String &sPassword, bool &disconnect);

   private:

      void CreateIPRange(const IPAddress &ipaddress, const String &username, int minutes);

      String GetIPRangeName_(const String &username);

      static boost::recursive_mutex ip_range_creation_mutex_;
   };
}