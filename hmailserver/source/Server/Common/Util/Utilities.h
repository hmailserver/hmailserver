// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class MimeHeader;
   class DateTime;

   class Utilities  
   {
   public:
	   Utilities();
	   virtual ~Utilities();

      static String ComputerName();
      static String GetExecutableDirectory();
      static String GetWin32TempDirectory();
      static String GetUniqueTempDirectory();
         
      static String GetIPAddress (SOCKADDR_IN addr);

      static String GenerateMessageID();
      static String GenerateReceivedHeader(const String &RemoteIP, String sHostName);

      static shared_ptr<MimeHeader> GetMimeHeader(const BYTE *pByteBuf, int iBufSize);

      static bool IsLocalHost(const String &sHostname);


      static String GetHostNameFromReceivedHeader(const String &sReceivedHeader);
      static String GetRecipientFromReceivedHeader(const String &sReceivedHeader);
      static DateTime GetDateTimeFromReceivedHeader(const String &sReceivedHeader);
      static IPAddress GetIPAddressFromReceivedHeader(const String &sReceivedHeader);

      bool IsValidIPAddress(const AnsiString string);

   private:

      static String m_sCachedWin32ComputerName;

      static String m_sCachedWin32TempDir;
   };

   class UtilitiesTester
   {
   public:
      UtilitiesTester() {};
      void Test();

   private:
      void _TestComputerName();
      void _TestReceivedHeaderParse();
   };

}
