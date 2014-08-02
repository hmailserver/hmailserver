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

      __int64 GetRuleID() const {return rule_id_; }
      void SetRuleID(__int64 newval) {rule_id_ = newval; }

      int GetSortOrder() const {return sort_order_; }
      void SetSortOrder(int newval) {sort_order_ = newval; }
     

      Type GetType() const {return type_;}
      void SetType(Type newval) {type_ = newval; }

      String GetSubject() const {return subject_; }
      void SetSubject(const String &sNewVal) {subject_ = sNewVal; }

      String GetBody() const {return body_; }
      void SetBody(const String &sNewVal) {body_ = sNewVal; }

      String GetFromName() const {return from_name_; }
      void SetFromName(const String &sNewVal) {from_name_ = sNewVal; }

      String GetFromAddress() const {return from_address_; }
      void SetFromAddress(const String &sNewVal) {from_address_ = sNewVal; }

      String GetIMAPFolder() const {return imapfolder_; }
      void SetIMAPFolder(const String &sNewVal) {imapfolder_ = sNewVal; }

      String GetTo() const {return to_; }
      void SetTo(const String &sNewVal) {to_ = sNewVal; }

      String GetFilename() const {return filename_; }
      void SetFilename(const String &sNewVal) {filename_ = sNewVal; }

      String GetScriptFunction() const {return script_function_; }
      void SetScriptFunction(const String &sNewVal) {script_function_ = sNewVal; }

      bool XMLStore(XNode *pRuleNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions) {return true;};

      String GetHeaderName() const {return header_name_; }
      void SetHeaderName(const String &sNewVal) {header_name_ = sNewVal; }

      String GetValue() const {return value_; }
      void SetValue(const String &sNewVal) {value_ = sNewVal; }

      __int64 GetRouteID() {return route_id_; }
      void SetRouteID(__int64 newVal) {route_id_ = newVal; }

   private:

      __int64 rule_id_;
      __int64 route_id_;
      int sort_order_;
      Type type_;
      
      String header_name_;
      String value_;
      String subject_;
      String body_;
      String from_address_;
      String from_name_;
      String imapfolder_;
      String filename_;
      String to_;
      String script_function_;
   };
}