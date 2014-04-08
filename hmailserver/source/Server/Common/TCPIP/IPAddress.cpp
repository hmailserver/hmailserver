// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\IPAddress.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IPAddress::IPAddress(void)
   {
    
   }

   IPAddress::~IPAddress(void)
   {

   }

   bool
   IPAddress::operator!=(const IPAddress &other)
   {
      return !(*this == other);
   }

   bool
   IPAddress::operator==(const IPAddress &other)
   {
      return (other.GetAddress1() == GetAddress1() &&
              other.GetAddress2() == GetAddress2());

   }

   bool
   IPAddress::operator>(const IPAddress &other)
   {
      bool inputIsSmaller =
         GetAddress1() > other.GetAddress1() ||
         (other.GetAddress1() == GetAddress1() && GetAddress2() > other.GetAddress2());

      return inputIsSmaller;
   }

   bool
   IPAddress::operator<(const IPAddress &other)
   {
      bool inputIsGreater =
         other.GetAddress1() > GetAddress1() ||
         (other.GetAddress1() == GetAddress1() && other.GetAddress2() > GetAddress2());

      return inputIsGreater;
   }

   IPAddress::IPAddress(boost::asio::ip::address address)
   {
      _address = address;
   }

   IPAddress::IPAddress(__int64 address1)
   {
      _SetIPV4Address(address1);
   }

   IPAddress::IPAddress(__int64 address1, __int64 address2)
   {
      _SetIPV6Address(address1, address2);
   }

   bool
   IPAddress::TryParse(const AnsiString &addressString)
   {
      return TryParse(addressString, true);
   }

   bool
   IPAddress::TryParse(const AnsiString &addressString, bool reportOnFail)
   {
      boost::system::error_code error;

      if (addressString.Find(":") >= 0)
         _address = boost::asio::ip::address_v6::from_string(addressString, error);
      else
      {
         // Windows 2000 Workaround:
         // On Windows 2000, WSAStringToAddress won't be able to parse the address
         // 255.255.255.255. This is a bug in Windows 2000, which appears to be fixed
         // in later versions. It does work properly on Windows XP. 
         // According to Wine guys...
         // http://www.winehq.org/pipermail/wine-patches/2005-August/020220.html
         // .. this appears to be a known bug.
         // So we just do a hack to get around it.
         // 
         if (addressString == "255.255.255.255")
            _SetIPV4Address(0xFFFFFFFF);
         else
            _address = boost::asio::ip::address_v4::from_string(addressString, error);         

      }

      if (error)
      {
         if (reportOnFail)
         {
            // Parsing of the TCP/IP address failed. Report error to debug log.
            String formattedMessage;
            formattedMessage.Format(_T("Unable to parse TCP/IP address. Address: %s, Error code: %d, Error message: %s"), String(addressString), error.value(), String(error.message()));
            LOG_DEBUG(formattedMessage);
         }

         return false;
      }
      else
         return true;
   }

   void 
   IPAddress::_SetIPV4Address(__int64 address1)
   {
      int i1 = (address1 >> 24) & 0xFF;
      int i2 = (address1 >> 16) & 0xFF;
      int i3 = (address1 >> 8) & 0xFF;
      int i4 = address1 & 0xFF;

      boost::asio::ip::address_v4::bytes_type bytes_value = { { i1, i2, i3, i4 } };

      _address = boost::asio::ip::address_v4(bytes_value);
   }

   void 
   IPAddress::_SetIPV6Address(__int64 address1, __int64 address2)
   {
      int i1 = (address1 >> 56) & 0xFF;
      int i2 = (address1 >> 48) & 0xFF;
      int i3 = (address1 >> 40) & 0xFF;
      int i4 = (address1 >> 32) & 0xFF;
      int i5 = (address1 >> 24) & 0xFF;
      int i6 = (address1 >> 16) & 0xFF;
      int i7 = (address1 >> 8) & 0xFF;
      int i8 = address1 & 0xFF;

      int b1 = (address2 >> 56) & 0xFF;
      int b2 = (address2 >> 48) & 0xFF;
      int b3 = (address2 >> 40) & 0xFF;
      int b4 = (address2 >> 32) & 0xFF;
      int b5 = (address2 >> 24) & 0xFF;
      int b6 = (address2 >> 16) & 0xFF;
      int b7 = (address2 >> 8) & 0xFF;
      int b8 = address2 & 0xFF;

      boost::asio::ip::address_v6::bytes_type bytes_value = { { i1, i2, i3, i4, i5, i6, i7, i8, b1, b2, b3, b4, b5, b6, b7, b8 } };

      _address = boost::asio::ip::address_v6(bytes_value);

   }

   IPAddress::Type 
   IPAddress::GetType() const
   {
      if (_address.is_v4())
         return IPAddress::IPV4;
      else if (_address.is_v6())
         return IPAddress::IPV6;
      else
      {
         assert(0);
         return IPAddress::Unknown;
      }
   }
   
   AnsiString 
   IPAddress::ToString() const
   {
      boost::system::error_code error;
      AnsiString result = _address.to_string(error);
      
      if (error)
         return "";
      else
         return result;
   }

   unsigned __int64 
   IPAddress::GetAddress1() const
   {
      if (_address.is_v4())
      {
         boost::asio::ip::address_v4::bytes_type bytes_value = _address.to_v4().to_bytes();

         __int64 ret = 0xFFFFFFFF & ((bytes_value[0] << 24) | (bytes_value[1] << 16) | (bytes_value[2] << 8) | bytes_value[3]);

         return ret;
      }
      else if (_address.is_v6())
      {
         boost::asio::ip::address_v6::bytes_type bytes_value = _address.to_v6().to_bytes();
         
         unsigned __int64 ret = 0xFFFFFFFFFFFFFFFF & (__int64) bytes_value[0] << 56;
         ret |=  (__int64) bytes_value[1] << 48;
         ret |=  (__int64) bytes_value[2] << 40;
         ret |=  (__int64) bytes_value[3] << 32;
         ret |=  (__int64) bytes_value[4] << 24;
         ret |=  (__int64) bytes_value[5] << 16;
         ret |=  (__int64) bytes_value[6] << 8;
         ret |=  (__int64) bytes_value[7] << 0;

         return ret;
      }
      else
         return 0;
   }
   
   unsigned __int64 
   IPAddress::GetAddress2() const
   {
      if (_address.is_v4())
         return 0;
      else if (_address.is_v6())
      {
         boost::asio::ip::address_v6::bytes_type bytes_value = _address.to_v6().to_bytes();

         unsigned __int64 ret = 0xFFFFFFFFFFFFFFFF & (__int64) bytes_value[8] << 56;
         ret |=  (__int64) bytes_value[9] << 48;
         ret |=  (__int64) bytes_value[10] << 40;
         ret |=  (__int64) bytes_value[11] << 32;
         ret |=  (__int64) bytes_value[12] << 24;
         ret |=  (__int64) bytes_value[13] << 16;
         ret |=  (__int64) bytes_value[14] << 8;
         ret |=  (__int64) bytes_value[15] << 0;

         return ret;
      }
      else 
         return 0;
   }

   bool 
   IPAddress::IsAny() const
   {
      if (_address.is_v4())
      {
         if (_address.to_v4().to_bytes() == boost::asio::ip::address_v4::any().to_bytes())
         {
            return true;
         }
      }
      else if (_address.is_v6())
      {
         if (_address.to_v6().to_bytes() == boost::asio::ip::address_v6::any().to_bytes())
         {
            return true;
         }
      }
      
      return false;
   }

   bool 
   IPAddress::WithinRange(const IPAddress &lower, const IPAddress &upper) const
   {
#ifdef _DEBUG
      String lower1Str = lower.ToString();
      String lower2Str = upper.ToString();
      String thisIP = ToString();
#endif

      unsigned __int64 addressToCheck1 = GetAddress1();
      unsigned __int64 addressToCheck2 = GetAddress2();
      
      // If it's an IPv4 address, we don't need to compare the second part.
      if (_address.is_v4())
      {
         if (addressToCheck1 >= lower.GetAddress1() && addressToCheck1 <= upper.GetAddress1())
            return true;
      }
      else if (_address.is_v6())
      {
         // Check that it's same or higher as the lower bound.
         if (addressToCheck1 >  lower.GetAddress1() || 
            (addressToCheck1 == lower.GetAddress1() && addressToCheck2 >= lower.GetAddress2()))
         {
            // Check that it's same or less than upper bound.
            if (addressToCheck1 <  upper.GetAddress1() || 
               (addressToCheck1 == upper.GetAddress1() && addressToCheck2 <= upper.GetAddress2()))
            {
               return true;
            }
         }
      }

      return false   ;
   }

   bool 
   IPAddress::IsValid(const AnsiString &address)
   {
      boost::system::error_code errorCode;
      boost::asio::ip::address adr = boost::asio::ip::address::from_string(address, errorCode);

      if (errorCode)
         return false;
      else
         return true;
   }
}