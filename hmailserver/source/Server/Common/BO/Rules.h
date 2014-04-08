// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "../Persistence/PersistentRule.h"
#include "Rule.h"

namespace HM
{
   class Rules : public Collection<Rule, PersistentRule>
   {
   public:
      Rules(__int64 iAccountID);
      ~Rules(void);

      void Refresh();

      __int64 GetAccountID() const {return m_iAccountID; }
 
      void MoveUp(__int64 iRuleID);
      void MoveDown(__int64 iRuleID);

   protected:
      virtual String GetCollectionName() const {return "Rules"; }
      virtual bool PreSaveObject(shared_ptr<Rule> pRule, XNode *node);
   private:
      
      vector<shared_ptr<Rule> >::iterator _GetRuleIterator(__int64 iRuleID);
      void _UpdateSortOrder();

      __int64 m_iAccountID;
   };
}