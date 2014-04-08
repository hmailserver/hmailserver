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

      bool RenameDomain(const String& oldDomainName, shared_ptr<Domain> pDomain, String &errorMessage);
      bool RenameAccount(const String& oldAccountName, shared_ptr<Account> pAccount, String &errorMessage);

   private:

      bool _UpdateDomainName(String &sAddress, const String &oldDomainName, const String& newDomainName);
      bool _RenameDirectory(const String &oldDirectory, const String &newDirectory, String &errorMessage);
      bool _RenameDomainDataDirectory(const String &oldDomainName, const String &newDomainName, String &errorMessage);
   };

}
