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

      static std::shared_ptr<const Account> ValidatePassword(const String &sUsername, const String &sPassword);
      static std::shared_ptr<const Account> ValidatePassword(const String &sMasqname, const String &sUsername, const String &sPassword);

      // Validates the user password. Return the account if validation is OK. 

      static bool ValidatePassword(std::shared_ptr<const Account> pAccount, const String &sPassword);

      // Validates the user password. Return true if the password is correct.

   private:

      static void RehashPasswordIfNeeded_(std::shared_ptr<const Account> pAccount, const String &sPassword, const String &sStoredPassword);

      // Silently migrates the stored password to the currently configured algorithm
      // and cost. Only ever called after a stored credential has been verified.
   };
}