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

      long GetPort() const { return port_; }
      void SetPort(long lNewVal) {port_ = lNewVal; }

      void SetHELO(const String &HELO) { helo_ = HELO; }
      String GetHELO() const { return helo_; }

      bool GetIsAuthenticated() const { return is_authenticated_; }
      void SetIsAuthenticated(bool bNewVal) { is_authenticated_ = bNewVal; };

   private:

      String IPAddress;
      long port_;
      String username_;
      String helo_;
      bool is_authenticated_;
   };
}
