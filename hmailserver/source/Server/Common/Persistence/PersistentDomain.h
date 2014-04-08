// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Domain;

   class PersistentDomain
   {
   public:
	   PersistentDomain();
	   virtual ~PersistentDomain();

      static bool DeleteObject(shared_ptr<Domain> pDomain);
      static bool SaveObject(shared_ptr<Domain> pDomain);
      static bool SaveObject(shared_ptr<Domain> pDomain, String &sErrorMessage);

      static bool ReadObject(shared_ptr<Domain> pDomain, shared_ptr<DALRecordset> pRS);
      static bool ReadObject(shared_ptr<Domain> pDomain, const String & sDomainName);
      static bool ReadObject(shared_ptr<Domain> pDomain, const SQLCommand &command);
      static bool ReadObject(shared_ptr<Domain> pDomain, __int64 ObjectID);

      static bool DomainExists(const String &DomainName, bool &bIsActive);

      static int GetSize(shared_ptr<Domain> pDomain);
      static __int64 GetAllocatedSize(shared_ptr<Domain> pDomain);

   };

}
