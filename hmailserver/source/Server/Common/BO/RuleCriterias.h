// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "..\Persistence\PersistentRuleCriteria.h"
#include "RuleCriteria.h"

namespace HM
{
   class RuleCriterias : public Collection<RuleCriteria, PersistentRuleCriteria>
   {
   public:
      RuleCriterias(__int64 iRuleID);
      ~RuleCriterias(void);

      void Refresh();

      __int64 GetRuleID() const {return m_iRuleID; }

   protected:
      virtual String GetCollectionName() const {return "RuleCriterias"; }
      virtual bool PreSaveObject(shared_ptr<RuleCriteria> pRuleCriteria, XNode *node);
   private:
      __int64 m_iRuleID;
   };
}