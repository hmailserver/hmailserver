// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include ".\COMAuthentication.h"
#include "..\Common\BO\Account.h"
#include "..\Common\Util\PasswordValidator.h"
#include "..\Common\Util\Crypt.h"

#include "COMError.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   COMAuthentication::COMAuthentication(void)
   {
      
   }

   COMAuthentication::~COMAuthentication(void)
   {

   }

   std::shared_ptr<const Account>
   COMAuthentication::Authenticate(const String &sUsername, const String &sPassword)
   {
      // Try to fetch this account
      account_.reset();

      if (sUsername.CompareNoCase(_T("administrator")) == 0)
      {
         String sPasswordCorrect = HM::IniFileSettings::Instance()->GetAdministratorPassword();

         if (sPasswordCorrect.IsEmpty())
         {
            // The administrators password has not been set yet. It's likely
            // that we have just installed or upgraded hMailServer.
            
            // Has the empty password, so we can compare. The upgrade tool first
            // tries to authenticate with an empty password.
            sPasswordCorrect = HM::Crypt::Instance()->EnCrypt(sPasswordCorrect, HM::Crypt::ETSHA256);
         }

         
         Crypt::EncryptionType type = HM::Crypt::Instance()->GetHashType(sPasswordCorrect);

         // Validate the password.
         if (HM::Crypt::Instance()->Validate(sPassword, sPasswordCorrect, type))
         {
            // Create a dummy account since the administrator
            // does not have a real email account.

            account_ = std::shared_ptr<Account> 
               (
                  new Account("Administrator", Account::ServerAdmin)
               );

         }
      }
      else
      {
         account_ = HM::PasswordValidator::ValidatePassword(sUsername, sPassword);
      }

      return account_;
   }

   void 
   COMAuthentication::AttempAnonymousAuthentication()
   {
      // No authentication is required if the administration password is empty.
      String sAdminPassword = HM::IniFileSettings::Instance()->GetAdministratorPassword();
      if (sAdminPassword.IsEmpty())
      {
         // Create a dummy account since the administrator
         // does not have a real email account.

         account_ = std::shared_ptr<Account> (new Account("Administrator", Account::ServerAdmin));
      }
   }

   bool 
   COMAuthentication::GetIsAuthenticated() const
   {
      return account_ != 0;
   }

   __int64 
   COMAuthentication::GetAccountID() const
   {
      return account_->GetID();
   }

   __int64 
   COMAuthentication::GetDomainID() const
   {
      return account_->GetDomainID();
   }

   bool 
   COMAuthentication::GetIsDomainAdmin() const
   {
      if (GetIsServerAdmin())
         return true;

      return account_ && 
             account_->GetAdminLevel() == Account::DomainAdmin;
   }

   bool 
   COMAuthentication::GetIsServerAdmin() const
   {
      return (account_ && account_->GetAdminLevel() == Account::ServerAdmin);
   }

   int 
   COMAuthentication::GetAccessDenied() const
   {
      return COMError::GenerateError("You do not have access to this property / method. Ensure that hMailServer.Application.Authenticate() is called with proper login credentials.");
   }

}