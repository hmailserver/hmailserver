// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class RuleAction : public BusinessObject<RuleAction>
   {
   public:
      RuleAction(void);
      ~RuleAction(void);

      enum Type
      {
         Unknown = 0,
         Delete = 1,
         Forward = 2,
         Reply = 3,
         MoveToIMAPFolder = 4,
         ScriptFunction = 5,
         StopRuleProcessing = 6,
         SetHeaderValue = 7,
         SendUsingRoute = 8,
         CreateCopy = 9,
         BindToAddress = 10,
      };

      String GetName() const {return ""; }

      __int64 GetRuleID() const {return m_iRuleID; }
      void SetRuleID(__int64 newval) {m_iRuleID = newval; }

      int GetSortOrder() const {return m_iSortOrder; }
      void SetSortOrder(int newval) {m_iSortOrder = newval; }
     

      Type GetType() const {return m_iType;}
      void SetType(Type newval) {m_iType = newval; }

      String GetSubject() const {return m_sSubject; }
      void SetSubject(const String &sNewVal) {m_sSubject = sNewVal; }

      String GetBody() const {return m_sBody; }
      void SetBody(const String &sNewVal) {m_sBody = sNewVal; }

      String GetFromName() const {return m_sFromName; }
      void SetFromName(const String &sNewVal) {m_sFromName = sNewVal; }

      String GetFromAddress() const {return m_sFromAddress; }
      void SetFromAddress(const String &sNewVal) {m_sFromAddress = sNewVal; }

      String GetIMAPFolder() const {return m_sIMAPFolder; }
      void SetIMAPFolder(const String &sNewVal) {m_sIMAPFolder = sNewVal; }

      String GetTo() const {return m_sTo; }
      void SetTo(const String &sNewVal) {m_sTo = sNewVal; }

      String GetFilename() const {return m_sFilename; }
      void SetFilename(const String &sNewVal) {m_sFilename = sNewVal; }

      String GetScriptFunction() const {return m_sScriptFunction; }
      void SetScriptFunction(const String &sNewVal) {m_sScriptFunction = sNewVal; }

      bool XMLStore(XNode *pRuleNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions) {return true;};

      String GetHeaderName() const {return m_sHeaderName; }
      void SetHeaderName(const String &sNewVal) {m_sHeaderName = sNewVal; }

      String GetValue() const {return m_sValue; }
      void SetValue(const String &sNewVal) {m_sValue = sNewVal; }

      __int64 GetRouteID() {return m_iRouteID; }
      void SetRouteID(__int64 newVal) {m_iRouteID = newVal; }

   private:

      __int64 m_iRuleID;
      __int64 m_iRouteID;
      int m_iSortOrder;
      Type m_iType;
      
      String m_sHeaderName;
      String m_sValue;
      String m_sSubject;
      String m_sBody;
      String m_sFromAddress;
      String m_sFromName;
      String m_sIMAPFolder;
      String m_sFilename;
      String m_sTo;
      String m_sScriptFunction;
   };
}