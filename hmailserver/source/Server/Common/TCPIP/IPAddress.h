// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include <boost/asio.hpp>

namespace HM
{
   /*
      Represents an IP address.

      If the second address is zero, that means that it's
      an IPv4 address.

   */
   class IPAddress
   {
   public:
      IPAddress(void);
      ~IPAddress(void);
      
      IPAddress(boost::asio::ip::address address);
      
      IPAddress(__int64 address1, __int64 address2);
      IPAddress(__int64 address1);

      bool operator==(const IPAddress &other);
      bool operator!=(const IPAddress &other);
      bool operator>(const IPAddress &other);
      bool operator<(const IPAddress &other);

      bool TryParse(const AnsiString &string);
      bool TryParse(const AnsiString &string, bool reportOnFail);

      bool IsAny() const;

      enum Type
      {
         Unknown = 0,
         IPV4 = 1,
         IPV6 = 2
      };

      AnsiString ToString() const;
      AnsiString ToLongString() const;

      
      Type GetType() const;

      unsigned __int64 GetAddress1() const;
      unsigned __int64 GetAddress2() const;

      boost::asio::ip::address GetAddress() const {return address_; }     


      bool WithinRange(const IPAddress &lower, const IPAddress &upper) const;

      static bool IsValid(const AnsiString &address);

   private:

      void SetIPV4Address_(__int64 address1);
      void SetIPV6Address_(__int64 address1, __int64 address2);

      boost::asio::ip::address address_;     

   };
}