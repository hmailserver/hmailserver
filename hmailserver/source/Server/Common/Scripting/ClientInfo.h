// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


#pragma once

namespace HM
{
   class ClientInfo  
   {
   public:
	   ClientInfo();
	   virtual ~ClientInfo();

      void SetIPAddress(const String &IP) { IPAddress = IP; }
      String GetIPAddress() const { return IPAddress; }

      void SetUsername(const String &sUsernme) { username_ = sUsernme; }
      String GetUsername() const { return username_; }

      void SetPort(long lNewVal) { port_ = lNewVal; }
      long GetPort() const { return port_; }

      void SetSessionID(long lNewVal) { session_ = lNewVal; }
      long GetSessionID() const { return session_; }

      void SetHELO(const String &HELO) { helo_ = HELO; }
      String GetHELO() const { return helo_; }

      void SetIsAuthenticated(bool bNewVal) { is_authenticated_ = bNewVal; };
      bool GetIsAuthenticated() const { return is_authenticated_; }

      void SetIsEncryptedConnection(bool bNewVal) { is_encryptedconnection_ = bNewVal; };
      bool GetIsEncryptedConnection() const { return is_encryptedconnection_; }

      void SetCipherVersion(const String& sCipherVersion) { cipher_version_ = sCipherVersion; }
      String GetCipherVersion() const { return cipher_version_; }

      void SetCipherName(const String& sCipherName) { cipher_name_ = sCipherName; }
      String GetCipherName() const { return cipher_name_; }

      void SetCipherBits(long lNewVal) { cipher_bits_ = lNewVal; }
      long GetCipherBits() const { return cipher_bits_; }

   private:

      String IPAddress;
      long port_;
      long session_;
      String username_;
      String helo_;
      bool is_authenticated_;
      bool is_encryptedconnection_;
      String cipher_version_;
      String cipher_name_;
      long cipher_bits_;

   };
}
