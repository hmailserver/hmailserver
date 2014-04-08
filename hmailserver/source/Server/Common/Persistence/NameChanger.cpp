// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "NameChanger.h"
#include "../BO/Domain.h"
#include "../BO/Accounts.h"
#include "../BO/Aliases.h"
#include "../BO/DistributionLists.h"
#include "../BO/DistributionListRecipients.h"
#include "../BO/DistributionListRecipient.h"

#include "PersistentDistributionListRecipient.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   NameChanger::NameChanger()
   {

   }

   NameChanger::~NameChanger()
   {

   }

   bool
   NameChanger::RenameDomain(const String& oldDomainName, shared_ptr<Domain> pDomain, String &errorMessage)
   {
      const String& newDomainName = pDomain->GetName();

      if (!_RenameDomainDataDirectory(oldDomainName, pDomain->GetName(), errorMessage))
         return false;

      // Update accounts...
      std::vector<shared_ptr<Account> > vecAccounts = pDomain->GetAccounts()->GetVector();
      std::vector<shared_ptr<Account> >::iterator iterAccount = vecAccounts.begin();
      std::vector<shared_ptr<Account> >::iterator iterAccountEnd = vecAccounts.end();

      for (; iterAccount != iterAccountEnd; iterAccount++)
      {
         shared_ptr<Account> pAccount = (*iterAccount);

         String sAddress = pAccount->GetAddress();
         _UpdateDomainName(sAddress, oldDomainName, newDomainName);
         pAccount->SetAddress(sAddress);

         String currentVal = pAccount->GetForwardAddress();
         if (_UpdateDomainName(currentVal, oldDomainName, newDomainName))
            pAccount->SetForwardAddress(currentVal);

         PersistentAccount::SaveObject(pAccount);
      }

      // Update aliases...
      std::vector<shared_ptr<Alias> > vecAliases = pDomain->GetAliases()->GetVector();
      std::vector<shared_ptr<Alias> >::iterator iterAlias = vecAliases.begin();
      std::vector<shared_ptr<Alias> >::iterator iterAliasEnd = vecAliases.end();

      for (; iterAlias != iterAliasEnd; iterAlias++)
      {
         shared_ptr<Alias> pAlias = (*iterAlias);

         String sAddress = pAlias->GetName();
         _UpdateDomainName(sAddress, oldDomainName, newDomainName);
         pAlias->SetName(sAddress);

         String aliasValue = pAlias->GetValue();

         if (_UpdateDomainName(aliasValue, oldDomainName, newDomainName))
            pAlias->SetValue(aliasValue);

         PersistentAlias::SaveObject(pAlias);
      }

      // Update lists...
      std::vector<shared_ptr<DistributionList> > vecLists = pDomain->GetDistributionLists()->GetVector();
      std::vector<shared_ptr<DistributionList> >::iterator iterList = vecLists.begin();
      std::vector<shared_ptr<DistributionList> >::iterator iterListEnd = vecLists.end();

      for (; iterList != iterListEnd; iterList++)
      {
         shared_ptr<DistributionList> pList = (*iterList);

         String sAddress = pList->GetAddress();
         _UpdateDomainName(sAddress, oldDomainName,newDomainName);
         pList->SetAddress(sAddress);

         vector<shared_ptr<HM::DistributionListRecipient>> recipients = pList->GetMembers()->GetVector();
         boost_foreach(shared_ptr<DistributionListRecipient> recipient, recipients)
         {
            String address = recipient->GetAddress();
            if (_UpdateDomainName(address,oldDomainName, newDomainName))
            {
               recipient->SetAddress(address);
               PersistentDistributionListRecipient::SaveObject(recipient);
            }
         }

         PersistentDistributionList::SaveObject(pList);
      }

      return true;

   }

   bool 
   NameChanger::_UpdateDomainName(String &sAddress, const String &oldDomainName, const String& newDomainName)
   {

      if (!sAddress.EndsWith("@" + oldDomainName))
      {
         // Doesn't seem to contain the domain name.
         return false;
      }

      int atPos = sAddress.Find(_T("@"));


      // Remove the current domain name.
      sAddress = sAddress.Mid(0, atPos+1);

      // Add the new one.
      sAddress += newDomainName;

      return true;
   }

   bool
   NameChanger::RenameAccount(const String& oldAccountName, shared_ptr<Account> pAccount, String &errorMessage)
   {
      String dataDirectory = IniFileSettings::Instance()->GetDataDirectory();
      String domainName = StringParser::ExtractDomain(oldAccountName);
      String domainDirectory = FileUtilities::Combine(dataDirectory, domainName);

      String oldMailboxName  = StringParser::ExtractAddress(oldAccountName);
      String newMailboxName  = StringParser::ExtractAddress(pAccount->GetAddress());
      
      String oldDirectoryName = FileUtilities::Combine(domainDirectory, oldMailboxName);
      String newDirectoryName = FileUtilities::Combine(domainDirectory, newMailboxName);

      return _RenameDirectory(oldDirectoryName, newDirectoryName, errorMessage);
   }

   bool 
   NameChanger::_RenameDomainDataDirectory(const String &oldDomainName, const String &newDomainName, String &errorMessage)
   {
      // Old director name
      String oldDirectory = FileUtilities::Combine(IniFileSettings::Instance()->GetDataDirectory(), oldDomainName);
      String newDirectory = FileUtilities::Combine(IniFileSettings::Instance()->GetDataDirectory(), newDomainName);

      return _RenameDirectory(oldDirectory, newDirectory, errorMessage);
   }

   bool 
   NameChanger::_RenameDirectory(const String &oldDirectory, const String &newDirectory, String &errorMessage)
   {
      if (FileUtilities::Exists(newDirectory))
      {
         errorMessage = Formatter::Format("Can't rename {0} to {1} since {1} already exists.", oldDirectory, newDirectory);
         return false;
      }

      if (!FileUtilities::Exists(oldDirectory))
      {
         // Nothing to rename.
         return true;
      }

      // Since renaming the directory is likely to fail (due to locked files and what not), we create a copy
      // of the entire directory and then delete the old one. Yes, this is slow as .. but it's more likely to
      // actually work.
      if (!FileUtilities::CopyDirectory(oldDirectory, newDirectory, errorMessage))
      {
         return false;
      }

      // We were successful in copying the data directory for the domain in question. Now drop the old structure.
      FileUtilities::DeleteDirectory(oldDirectory);

      return true;
   }
}

