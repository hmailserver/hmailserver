// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


namespace HM
{
   class Account;

   class PersistentLogonFailure
   {
   public:
	   PersistentLogonFailure();
	   virtual ~PersistentLogonFailure();

      int GetCurrrentFailureCount(const IPAddress & ipaddress);
      bool AddFailure(const IPAddress &ipaddress);
      bool ClearOldFailures(int olderThanMinutes);
      bool ClearFailuresByIP(const IPAddress &ipaddress);
   };


}