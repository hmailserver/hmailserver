// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class RuleCriterias;
   class RuleActions;

   class Rule : public BusinessObject<Rule>
   {
   public:
      Rule(void);
      ~Rule(void);

      __int64 GetAccountID() const {return account_id_; }
      void SetAccountID(__int64 newval) {account_id_ = newval; }

      int GetSortOrder() const {return sort_order_; }
      void SetSortOrder(int newval) {sort_order_ = newval; }

      String GetName() const {return name_; }
      void SetName(const String &sName) {name_ = sName;}

      bool GetActive() const {return active_; }
      void SetActive(bool bNewVal) {active_ = bNewVal; }

      bool GetUseAND() const {return use_and_;}
      void SetUseAND(bool bNewVal) {use_and_ = bNewVal; }

      std::shared_ptr<RuleCriterias> GetCriterias();
      std::shared_ptr<RuleActions> GetActions();

      bool XMLStore(XNode *pParentNode, int iOptions);
      bool XMLLoad(XNode *pRuleNode, int iOptions);
      bool XMLLoadSubItems(XNode *pRuleNode, int iOptions);

   private:

      __int64 account_id_;
      int sort_order_;
      String name_;

      std::shared_ptr<RuleCriterias> criterias_;
      std::shared_ptr<RuleActions> actions_;

      bool active_;
      bool use_and_;
   };
}