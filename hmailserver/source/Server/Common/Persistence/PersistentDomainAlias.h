// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class DomainAlias;

   class PersistentDomainAlias
   {
   public:
      PersistentDomainAlias(void);
      ~PersistentDomainAlias(void);

      static bool ReadObject(shared_ptr<DomainAlias> oFA, const SQLCommand & sSQL);
      static bool ReadObject(shared_ptr<DomainAlias> oFA, shared_ptr<DALRecordset> pRS);
      static bool SaveObject(shared_ptr<DomainAlias> oFA);
      static bool SaveObject(shared_ptr<DomainAlias> oFA, String &sErrorMessage);
      static bool DeleteObject(shared_ptr<DomainAlias> pDA);
   };
}