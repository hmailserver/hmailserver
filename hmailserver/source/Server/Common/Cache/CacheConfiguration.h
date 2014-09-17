// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class CacheConfiguration
   {
   public:
	   CacheConfiguration();
	   virtual ~CacheConfiguration();

      bool GetUseCache() const;
      void SetUseCache(bool bNewVal);

      int GetDomainCacheTTL() const;
      void SetDomainCacheTTL(int iNewVal);
      int GetAccountCacheTTL() const;
      void SetAccountCacheTTL(int iNewVal);
      int GetAliasCacheTTL() const;
      void SetAliasCacheTTL(int iNewVal);
      int GetDistributionListCacheTTL() const;
      void SetDistributionListCacheTTL(int iNewVal);      


      bool XMLStore(XNode *pBackupNode, int Options);
      bool XMLLoad(XNode *pBackupNode, int iRestoreOptions);

   private:

      std::shared_ptr<PropertySet> GetSettings_() const;

   };
}
