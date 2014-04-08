// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Account;

   class PasswordValidator
   {
   public:
      PasswordValidator(void);
      ~PasswordValidator(void);

      static shared_ptr<const Account> ValidatePassword(const String &sUsername, const String &sPassword);
      // Validates the user password. Return the account if validation is OK. 

      static bool ValidatePassword(shared_ptr<const Account> pAccount, const String &sPassword);
      // Validates the user password. Return true if the password is correct.
   };
}