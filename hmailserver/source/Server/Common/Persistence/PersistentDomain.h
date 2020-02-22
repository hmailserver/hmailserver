// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Domain;
   enum PersistenceMode;

   class PersistentDomain
   {
   public:
	   PersistentDomain();
	   virtual ~PersistentDomain();

      static bool DeleteObject(std::shared_ptr<Domain> pDomain);
      static bool SaveObject(std::shared_ptr<Domain> pDomain);
      static bool SaveObject(std::shared_ptr<Domain> pDomain, String &sErrorMessage, PersistenceMode mode);

      static bool ReadObject(std::shared_ptr<Domain> pDomain, std::shared_ptr<DALRecordset> pRS);
      static bool ReadObject(std::shared_ptr<Domain> pDomain, const String & sDomainName);
      static bool ReadObject(std::shared_ptr<Domain> pDomain, const SQLCommand &command);
      static bool ReadObject(std::shared_ptr<Domain> pDomain, __int64 ObjectID);

      static bool DomainExists(const String &DomainName, bool &bIsActive);

      static int GetSize(std::shared_ptr<Domain> pDomain);
      static __int64 GetAllocatedSize(std::shared_ptr<Domain> pDomain);

   };

}
