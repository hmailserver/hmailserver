// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class HostNameAndIpAddress
   {
   public:
   
      void SetHostName(const String &host_name) {host_name_ = host_name;}
      void SetIpAddress(const String &ip_address) {ip_address_ = ip_address;}

      String GetHostName() {return host_name_;}
      String GetIpAddress() {return ip_address_; }

   private:
      
      String host_name_;
      String ip_address_;

   };


}
