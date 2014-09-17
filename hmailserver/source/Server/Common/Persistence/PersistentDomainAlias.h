// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class DomainAlias;
   enum PersistenceMode;

   class PersistentDomainAlias
   {
   public:
      PersistentDomainAlias(void);
      ~PersistentDomainAlias(void);

      static bool ReadObject(std::shared_ptr<DomainAlias> oFA, const SQLCommand & sSQL);
      static bool ReadObject(std::shared_ptr<DomainAlias> oFA, std::shared_ptr<DALRecordset> pRS);
      static bool SaveObject(std::shared_ptr<DomainAlias> oFA);
      static bool SaveObject(std::shared_ptr<DomainAlias> oFA, String &sErrorMessage, PersistenceMode mode);
      static bool DeleteObject(std::shared_ptr<DomainAlias> pDA);
   };
}