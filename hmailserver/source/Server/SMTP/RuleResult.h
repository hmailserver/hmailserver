// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class RuleResult
   {
   public:
      RuleResult(void);
      ~RuleResult(void);

      String GetMoveToFolder() const {return m_sMoveToFolder;}
      void SetMoveToFolder(const String &sIMAPFolderName) {m_sMoveToFolder = sIMAPFolderName;}
      
      String GetDeleteRuleName() const {return m_sDeleteRuleName;}
      void SetDeleteRuleName(const String &sDeleteRuleName) {m_sDeleteRuleName = sDeleteRuleName;}

      __int64 GetSendUsingRoute() const {return m_iSendUsingRoute;}
      void SetSendUsingRoute(__int64 iRuleID) {m_iSendUsingRoute = iRuleID; }

      bool GetDeleteEmail() const {return m_bDeleteEmail;}
      void SetDeleteEmail(bool bNewVal) {m_bDeleteEmail = bNewVal; }

      String GetBindToAddress() const {return m_sBindToAddress;}
      void SetBindToAddress(const String &sAddress) {m_sBindToAddress = sAddress;}

   private:

      String m_sMoveToFolder;
      __int64 m_iSendUsingRoute;
      
      bool m_bDeleteEmail;
      String m_sDeleteRuleName;

      String m_sBindToAddress;

   };
}