// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class FetchAccountUIDs;

   class FetchAccount : public BusinessObject<FetchAccount>
   {
   public:

      enum ServerType
      {
         POP3 = 0,
      };


      FetchAccount(void);
      ~FetchAccount(void);

      __int64 GetAccountID() const {return m_iAccountID; }
      void SetAccountID(__int64 iNewVal) {m_iAccountID = iNewVal; }

      String GetName() const {return m_sName;}
      void SetName(const String &sNewVal) {m_sName = sNewVal; }

      void SetServerAddress(const String &sNewVal) {m_sServerAddress = sNewVal; }
      String GetServerAddress() const {return m_sServerAddress; }

      ServerType GetServerType() const {return m_iServerType; }
      void SetServerType(ServerType iValue) {m_iServerType = iValue; }

      int GetPort() const {return m_iPort;}
      void SetPort(int iNewVal) {m_iPort = iNewVal; }

      String GetUsername() const {return m_sUsername; }
      void SetUsername(const String &sValue) {m_sUsername = sValue; }

      String GetPassword() const {return m_sPassword; }
      void SetPassword(const String &Value) {m_sPassword = Value; }
      
      int GetMinutesBetweenTry() const {return m_iMinutes; }
      void SetMinutesBetweenTry(int iMinutes) {m_iMinutes = iMinutes; }

      int GetDaysToKeep() const {return m_iDaysToKeep; }
      void SetDaysToKeep(int iMinutes) {m_iDaysToKeep = iMinutes; }

      bool GetActive() const {return m_bIsActive; }
      void SetActive(bool bActive) {m_bIsActive = bActive; }

      bool GetProcessMIMERecipients() const {return m_bProcessMIMERecipients; }
      void SetProcessMIMERecipients(bool bNewVal) {m_bProcessMIMERecipients = bNewVal; }

      bool GetProcessMIMEDate() const {return m_bProcessMIMEDate; }
      void SetProcessMIMEDate(bool bNewVal) {m_bProcessMIMEDate = bNewVal; }

      bool GetUseSSL() const {return m_bUseSSL; }
      void SetUseSSL(bool bNewVal) {m_bUseSSL = bNewVal; }

      String GetNextTry() const {return m_sNextTry;}
      void SetNextTry(const String &sNextTry) {m_sNextTry = sNextTry;}

      shared_ptr<FetchAccountUIDs> GetUIDs();

      bool GetUseAntiSpam() const {return _useAntiSpam; }
      void SetUseAntiSpam(bool bNewVal) {_useAntiSpam = bNewVal; }

      bool GetUseAntiVirus() const {return _useAntiVirus; }
      void SetUseAntiVirus(bool bNewVal) {_useAntiVirus = bNewVal; }

      bool GetEnableRouteRecipients() const {return _enableRouteRecipients;}
      void SetEnableRouteRecipients(bool enable) {_enableRouteRecipients = enable;}

      bool XMLStore(XNode *pFetchAccountsNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions);

   private:

      __int64 m_iAccountID;
      String m_sName;
      String m_sServerAddress;
      String m_sNextTry;
      ServerType m_iServerType;

      long m_iPort;
      String m_sUsername;
      String m_sPassword;
      int m_iMinutes;
      int m_iDaysToKeep;
      bool m_bIsActive;
      shared_ptr<FetchAccountUIDs> m_pUIDs;

      bool m_bProcessMIMERecipients;
      bool m_bProcessMIMEDate;
      bool m_bUseSSL;

      bool _useAntiSpam;
      bool _useAntiVirus;

      bool _enableRouteRecipients;

   };
}