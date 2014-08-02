// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../TCPIP/SocketConstants.h"

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

      __int64 GetAccountID() const {return account_id_; }
      void SetAccountID(__int64 iNewVal) {account_id_ = iNewVal; }

      String GetName() const {return name_;}
      void SetName(const String &sNewVal) {name_ = sNewVal; }

      void SetServerAddress(const String &sNewVal) {server_address_ = sNewVal; }
      String GetServerAddress() const {return server_address_; }

      ServerType GetServerType() const {return server_type_; }
      void SetServerType(ServerType iValue) {server_type_ = iValue; }

      int GetPort() const {return port_;}
      void SetPort(int iNewVal) {port_ = iNewVal; }

      String GetUsername() const {return username_; }
      void SetUsername(const String &sValue) {username_ = sValue; }

      String GetPassword() const {return password_; }
      void SetPassword(const String &Value) {password_ = Value; }
      
      int GetMinutesBetweenTry() const {return minutes_; }
      void SetMinutesBetweenTry(int iMinutes) {minutes_ = iMinutes; }

      int GetDaysToKeep() const {return days_to_keep_; }
      void SetDaysToKeep(int iMinutes) {days_to_keep_ = iMinutes; }

      bool GetActive() const {return is_active_; }
      void SetActive(bool bActive) {is_active_ = bActive; }

      bool GetProcessMIMERecipients() const {return process_mimerecipients_; }
      void SetProcessMIMERecipients(bool bNewVal) {process_mimerecipients_ = bNewVal; }

      bool GetProcessMIMEDate() const {return process_mimedate_; }
      void SetProcessMIMEDate(bool bNewVal) {process_mimedate_ = bNewVal; }

      ConnectionSecurity GetConnectionSecurity() const  {return connection_security_; }
      void SetConnectionSecurity(ConnectionSecurity connection_security) {connection_security_ = connection_security; }

      String GetNextTry() const {return next_try_;}
      void SetNextTry(const String &sNextTry) {next_try_ = sNextTry;}

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

      __int64 account_id_;
      String name_;
      String server_address_;
      String next_try_;
      ServerType server_type_;

      long port_;
      String username_;
      String password_;
      int minutes_;
      int days_to_keep_;
      bool is_active_;
      shared_ptr<FetchAccountUIDs> uids_;

      bool process_mimerecipients_;
      bool process_mimedate_;

      bool _useAntiSpam;
      bool _useAntiVirus;

      bool _enableRouteRecipients;

      ConnectionSecurity connection_security_;
   };
}