// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class RuleCriteria : public BusinessObject<RuleCriteria>
   {
   public:

      enum PredefinedField
      {
         FTUnknown = 0,
         FTFrom = 1,
         FTTo = 2,
         FTCC = 3,
         FTSubject = 4,
         FTBody = 5,
         FTMessageSize = 6,
         FTRecipientList = 7,
         FTDeliveryAttempts = 8
      };

      enum MatchType
      {
         None = 0,
         Equals = 1,
         Contains = 2,
         LessThan = 3,
         GreaterThan = 4,
         MatchesRegEx = 5,
         NotContains = 6,
         NotEquals = 7,
         Wildcard = 8,
         
      };

      RuleCriteria(void);
      ~RuleCriteria(void);

      String GetName() const {return ""; }

      __int64 GetRuleID() const {return m_iRuleID; }
      void SetRuleID(__int64 newval) {m_iRuleID = newval; }

      String GetMatchValue() const {return m_sMatchString; }
      void SetMatchValue(const String &iNewVal) {m_sMatchString = iNewVal; }

      PredefinedField GetPredefinedField() const {return m_eFieldType; }
      void SetPredefinedField(PredefinedField i) {m_eFieldType = i; }

      MatchType GetMatchType() const {return m_eMatchType; }
      void SetMatchType(MatchType mt) {m_eMatchType = mt; }

      String GetHeaderField() const {return m_sHeaderField; }
      void SetHeaderField(const String &sValue) {m_sHeaderField = sValue; }

      bool GetUsePredefined() const {return m_bUsePredefinedField;}
      void SetUsePredefined(bool bValue) {m_bUsePredefinedField = bValue; }

      bool XMLStore(XNode *pRuleNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions) {return true;}

   private:

      __int64 m_iRuleID;

      String m_sMatchString;

      PredefinedField m_eFieldType;
      MatchType m_eMatchType;

      String m_sHeaderField;
      
      bool m_bUsePredefinedField;
   };
}