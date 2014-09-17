// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class RuleAction;
   enum PersistenceMode;

   class PersistentRuleAction
   {
   public:
      PersistentRuleAction(void);
      ~PersistentRuleAction(void);

      static bool ReadObject(std::shared_ptr<RuleAction> pRuleAction, const SQLCommand& sSQL);
      static bool ReadObject(std::shared_ptr<RuleAction> pRuleAction, std::shared_ptr<DALRecordset> pRS);

      static bool SaveObject(std::shared_ptr<RuleAction> pRule, String &errorMessage, PersistenceMode mode);
      static bool SaveObject(std::shared_ptr<RuleAction> pRule);
      static bool DeleteObject(std::shared_ptr<RuleAction> pRule);

      static bool DeleteObjects(__int64 iRuleID);

      static bool GetExistsFolderReferenceContainingCharacter(String theChar);
      static bool UpdateHierarchyDelimiter(String oldDelimiter, String newDelimiter);
   };
}