// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class MimeHeader;
   class DateTime;
   class CipherInfo;

   class Utilities  
   {
   public:
	   Utilities();
	   virtual ~Utilities();

      static String ComputerName();
      static String GetBinDirectory();
      static String GetUniqueTempDirectory();
      static String GetIPAddress (SOCKADDR_IN addr);
      static String GenerateMessageID();


      static std::shared_ptr<MimeHeader> GetMimeHeader(const BYTE *pByteBuf, size_t iBufSize);

      static bool IsLocalHost(const String &sHostname);


      static String GetHostNameFromReceivedHeader(const String &sReceivedHeader);
      static String GetRecipientFromReceivedHeader(const String &sReceivedHeader);
      static DateTime GetDateTimeFromReceivedHeader(const String &sReceivedHeader);
      static IPAddress GetIPAddressFromReceivedHeader(const String &sReceivedHeader);

      bool IsValidIPAddress(const AnsiString string);

   private:

      

      static String cached_win_32computer_name_;

      static String cached_win_32temp_dir_;
   };

   class UtilitiesTester
   {
   public:
      UtilitiesTester() {};
      void Test();

   private:
      void TestComputerName_();
      void TestReceivedHeaderParse_();
   };

}
