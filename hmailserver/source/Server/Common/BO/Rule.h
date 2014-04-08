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

      __int64 GetAccountID() const {return m_iAccountID; }
      void SetAccountID(__int64 newval) {m_iAccountID = newval; }

      int GetSortOrder() const {return m_iSortOrder; }
      void SetSortOrder(int newval) {m_iSortOrder = newval; }

      String GetName() const {return m_sName; }
      void SetName(const String &sName) {m_sName = sName;}

      bool GetActive() const {return m_bActive; }
      void SetActive(bool bNewVal) {m_bActive = bNewVal; }

      bool GetUseAND() const {return m_bUseAND;}
      void SetUseAND(bool bNewVal) {m_bUseAND = bNewVal; }

      shared_ptr<RuleCriterias> GetCriterias();
      shared_ptr<RuleActions> GetActions();

      bool XMLStore(XNode *pParentNode, int iOptions);
      bool XMLLoad(XNode *pRuleNode, int iOptions);
      bool XMLLoadSubItems(XNode *pRuleNode, int iOptions);

   private:

      __int64 m_iAccountID;
      int m_iSortOrder;
      String m_sName;

      shared_ptr<RuleCriterias> m_pCriterias;
      shared_ptr<RuleActions> m_pActions;

      bool m_bActive;
      bool m_bUseAND;
   };
}