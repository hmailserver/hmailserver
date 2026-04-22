#pragma once

namespace HM
{
   class IPAddress;

   class BLCheck
   {
   public:
      BLCheck(void);
      ~BLCheck(void);

      static bool ClientExistsInDNSBL(const IPAddress &sClientIP, const String &sDNSBLHost, const String &sExpectedResult);

      static String GetRevertedIP(const String &sIP);

      static std::set<String> ExpandAddresses(const String &input);


   };

   class BLCheckTester
   {
   public:
      void Test();

   };
}
