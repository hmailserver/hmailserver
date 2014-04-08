// Copyright (c) 2008 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "PreSaveLimitationsCheck.h"

#include "../BO/Domain.h"
#include "../BO/DomainAlias.h"
#include "../BO/DomainAliases.h"
#include "../BO/Accounts.h"
#include "../BO/Aliases.h"
#include "../BO/DistributionLists.h"
#include "../BO/DistributionListRecipient.h"
#include "../BO/Groups.h"
#include "../BO/Route.h"
#include "../BO/Routes.h"
#include "../Cache/CacheContainer.h"
#include "../Application/ObjectCache.h"

#include "PersistentDomain.h"

#include "../../IMAP/IMAPConfiguration.h"
#include "../../SMTP/SMTPConfiguration.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM   
{
   PreSaveLimitationsCheck::PreSaveLimitationsCheck()
   {
   }

   PreSaveLimitationsCheck::~PreSaveLimitationsCheck()
   {

   }

   shared_ptr<Domain> 
   PreSaveLimitationsCheck::GetDomain(__int64 id)
   {
      shared_ptr<Domain> domain = shared_ptr<Domain>(new Domain);

      PersistentDomain::ReadObject(domain, id);

      return domain;
   }

   bool
   PreSaveLimitationsCheck::CheckLimitations(shared_ptr<Account> account, String &resultDescription)
   {
      if (account->GetVacationMessage().GetLength() > 1000)
      {
         resultDescription = "The auto reply message length exceeds the 1000 character limit.";
         return false;
      }

      if (account->GetAddress().FindOneOf(_T("?*|\\/<>")) >= 0)
      {
         resultDescription = "The characters ?*|\\/<> are not permitted in hMailServer account addresses.";
         return false;
      }


      shared_ptr<Domain> domain = GetDomain(account->GetDomainID());

      if (GetDuplicateExist(domain, TypeAccount, account->GetID(), account->GetAddress()))      
         return DuplicateError(resultDescription);

      if (account->GetID() == 0)
      {
         if (domain->GetMaxNoOfAccountsEnabled() && 
             domain->GetAccounts()->GetCount() >= domain->GetMaxNoOfAccounts())
         {
            resultDescription = "The maximum number of accounts have been created.";
            return false;
         }
      }

      if (domain->GetMaxAccountSize() != 0)
      {
         if (account->GetAccountMaxSize() > domain->GetMaxAccountSize())
         {
            resultDescription = "The account is larger than the maximum account size specified in the domain settings.";
            return false;
         }

         if (account->GetAccountMaxSize() == 0)
         {
            resultDescription = "The domain has a maximum account size set. When this is the case, all accounts in the domains must have an account size set.";
            return false;
         }
      }

      if (domain->GetMaxSizeMB() > 0)
      {

         if (account->GetAccountMaxSize() == 0)
         {
            resultDescription = "The domain has a maximum account size set. When this is the case, all accounts in the domains must have an account size set.";
            return false;
         }

         String sError = "Account could not be saved. The total size of all accounts in the domain would exceed the maximum size for the domain.";
         
         __int64 currentSize = PersistentDomain::GetAllocatedSize(domain);

         if (account->GetID() == 0)
         {
            if (currentSize + account->GetAccountMaxSize() > domain->GetMaxSizeMB())
            {
               resultDescription = sError;
               return false;
            }
         }
         else
         {
            shared_ptr<Account> currentAccountSettings = shared_ptr<Account>(new Account);
            PersistentAccount::ReadObject(currentAccountSettings, account->GetID());

            if (currentSize - currentAccountSettings->GetAccountMaxSize() + account->GetAccountMaxSize() > domain->GetMaxSizeMB())
            {
               resultDescription = sError;
               return false;
            }
         }
      }

      String address = account->GetAddress().ToLower();
      if (address.Find(_T("\\")) >= 0 || address.Find(_T("/")) >= 0)
      {
         resultDescription = "An account name may not contain the characters \\ or /.";
         return false;
      }


      return true;
   }

   bool
   PreSaveLimitationsCheck::CheckLimitations(shared_ptr<DistributionListRecipient> recipient, String &resultDescription)
   {
      if (recipient->GetAddress().GetLength() == 0)
      {
         resultDescription = "The recipient address is empty";
         return false;
      }

      return true;
   }

   bool
   PreSaveLimitationsCheck::CheckLimitations(shared_ptr<Alias> alias, String &resultDescription)
   {
      shared_ptr<Domain> domain = GetDomain(alias->GetDomainID());

      if (GetDuplicateExist(domain, TypeAlias, alias->GetID(), alias->GetName()))      
         return DuplicateError(resultDescription);

      if (alias->GetID() == 0)
      {
         if (domain->GetMaxNoOfAliasesEnabled() && 
             domain->GetAliases()->GetCount() >= domain->GetMaxNoOfAliases())
         {
            resultDescription = "The maximum number of aliases have been created.";
            return false;
         }
      }


      return true;
   }

   bool
   PreSaveLimitationsCheck::CheckLimitations(shared_ptr<DistributionList> list, String &resultDescription)
   {
      shared_ptr<Domain> domain = GetDomain(list->GetDomainID());

      if (GetDuplicateExist(domain, TypeList,list->GetID(), list->GetAddress()))      
         return DuplicateError(resultDescription);

      if (list->GetID() == 0)
      {
         if (domain->GetMaxNoOfDistributionLists()  && 
            domain->GetDistributionLists()->GetCount() >= domain->GetMaxNoOfDistributionLists())
         {
            resultDescription = "The maximum number of distribution lists have been created.";
            return false;
         }
      }

      return true;
   }

   bool
   PreSaveLimitationsCheck::CheckLimitations(shared_ptr<Group> group, String &resultDescription)
   {
      shared_ptr<Group> pGroup = Configuration::Instance()->GetIMAPConfiguration()->GetGroups()->GetItemByName(group->GetName());

      if (pGroup && (group->GetID() == 0 || group->GetID() != pGroup->GetID()))
      {
         resultDescription = "Another group with this name already exists.";
         return false;
      }

      return true;
   }

   bool
   PreSaveLimitationsCheck::DuplicateError(String &resultDescription)
   {
      resultDescription = "Another object with the same name already exists in this domain.";
      return false;
   }

   bool 
   PreSaveLimitationsCheck::GetDuplicateExist(shared_ptr<Domain> domain, ObjectType objectType, __int64 objectID, const String &objectName)
   {
      shared_ptr<Account> pAccount = shared_ptr<Account>(new Account);
      if (PersistentAccount::ReadObject(pAccount, objectName))
      {
         if (pAccount && (pAccount->GetID() != objectID || objectType != TypeAccount) )
            return true;
      }

      shared_ptr<Alias> pAlias = shared_ptr<Alias>(new Alias);
      if (PersistentAlias::ReadObject(pAlias, objectName))
      {
         if (pAlias && (pAlias->GetID() != objectID || objectType != TypeAlias))
            return true;
      }

      shared_ptr<DistributionList> pList = shared_ptr<DistributionList>(new DistributionList);; 
      if (PersistentDistributionList::ReadObject(pList,objectName ))
      {
         if (pList && (pList->GetID() != objectID || objectType != TypeList))
            return true;
      }

      return false;
   }

   bool
   PreSaveLimitationsCheck::CheckLimitations(shared_ptr<Domain> domain, String &resultDescription)
   {
      shared_ptr<const Domain> pDomain = CacheContainer::Instance()->GetDomain(domain->GetName());

      if (pDomain && (domain->GetID() == 0 || domain->GetID() != pDomain->GetID()))
      {
         resultDescription = "Another domain with this name already exists.";
         return false;
      }

      String sName = domain->GetName();
      if (!StringParser::IsValidDomainName(sName))
      {
         resultDescription = "The domain name you have entered is not a valid domain name.";
         return false;
      }

      // Check if there's a domain alias with this name. If so, this domain would be a duplciate.
      shared_ptr<DomainAliases> pDomainAliases = ObjectCache::Instance()->GetDomainAliases();
      shared_ptr<DomainAlias> pDomainAlias = pDomainAliases->GetItemByName(domain->GetName());
      if (pDomainAlias)
      {
         resultDescription = "A domain alias with this name already exists.";
         return false;
      }
   
      return true;
   }

   bool
   PreSaveLimitationsCheck::CheckLimitations(shared_ptr<DomainAlias> domainAlias, String &resultDescription)
   {
      shared_ptr<const Domain> pDomain = CacheContainer::Instance()->GetDomain(domainAlias->GetName());

      if (pDomain)
      {
         resultDescription = "Another domain with this name already exists.";
         return false;
      }

      // Check if there's a domain alias with this name. If so, this domain would be a duplciate.
      shared_ptr<DomainAliases> pDomainAliases = ObjectCache::Instance()->GetDomainAliases();
      shared_ptr<DomainAlias> pDomainAlias = pDomainAliases->GetItemByName(domainAlias->GetName());
      if (pDomainAlias && (domainAlias->GetID() == 0 || domainAlias->GetID() != pDomainAlias->GetID()))
      {
         resultDescription = "A domain alias with this name already exists.";
         return false;
      }

      return true;
   }

   bool 
   PreSaveLimitationsCheck::CheckLimitations(shared_ptr<Route> route, String &resultDescription)
   {
      shared_ptr<Routes> pRoutes = Configuration::Instance()->GetSMTPConfiguration()->GetRoutes();

      vector<shared_ptr<Route> > vecRoutes = pRoutes->GetItemsByName(route->GetName());
 
      boost_foreach(shared_ptr<Route> pExistingRoute, vecRoutes)
      {
         if (route->GetID() == 0 || route->GetID() != pExistingRoute->GetID())
         {
            resultDescription = "Another route with this name already exists.";
            return false;
         }
      }

      return "";
   }

}