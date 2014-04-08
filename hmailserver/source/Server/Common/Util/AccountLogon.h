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

      shared_ptr<const Account> Logon(const IPAddress & ipaddress, const String &sUsername, const String &sPassword, bool &disconnect); 

   private:

      void CreateIPRange(const IPAddress &ipaddress, const String &username, int minutes);

      String _GetIPRangeName(const String &username);

      static CriticalSection _IPRangeCreationLock; 
   };
}