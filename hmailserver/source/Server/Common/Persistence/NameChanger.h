// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Domain;

   class NameChanger
   {
   public:
	   NameChanger();
	   virtual ~NameChanger();

      bool RenameDomain(const String& oldDomainName, std::shared_ptr<Domain> pDomain, String &errorMessage);
      bool RenameAccount(const String& oldAccountName, std::shared_ptr<Account> pAccount, String &errorMessage);

   private:

      bool UpdateDomainName_(String &sAddress, const String &oldDomainName, const String& newDomainName);
      bool RenameDirectory_(const String &oldDirectory, const String &newDirectory, String &errorMessage);
      bool RenameDomainDataDirectory_(const String &oldDomainName, const String &newDomainName, String &errorMessage);
   };

}
