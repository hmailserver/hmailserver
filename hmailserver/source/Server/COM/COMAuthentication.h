// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Account;

   class COMAuthentication
   {
   public:
      COMAuthentication(void);
      ~COMAuthentication(void);

      std::shared_ptr<const Account> Authenticate(const String &sUsername, const String &sPassword);

      void AttempAnonymousAuthentication();

      bool GetIsAuthenticated() const;

      bool GetIsDomainAdmin() const;
      bool GetIsServerAdmin() const;

      __int64 GetAccountID() const;
      __int64 GetDomainID() const;

      int GetAccessDenied() const;

   private:

      std::shared_ptr<const Account> account_;
   };
}