// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class RuleCriteria;

   class PersistentRuleCriteria
   {
   public:
      PersistentRuleCriteria(void);
      ~PersistentRuleCriteria(void);

      static bool ReadObject(shared_ptr<RuleCriteria> pRuleCriteria, const SQLCommand &sSQL);
      static bool ReadObject(shared_ptr<RuleCriteria> pRuleCriteria, shared_ptr<DALRecordset> pRS);

      
      static bool SaveObject(shared_ptr<RuleCriteria> pRuleCriteria, String &errorMessage);
      static bool SaveObject(shared_ptr<RuleCriteria> pRuleCriteria);
      static bool DeleteObject(shared_ptr<RuleCriteria> pRuleCriteria);

      static bool DeleteObjects(__int64 iRuleID);
   };
}