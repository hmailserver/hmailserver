// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Utilities.h"
#include "File.h"
#include "Time.h"
#include "Registry.h"


#include "../Mime/Mime.h"
#include "GUIDCreator.h"

#include "../TCPIP/LocalIPAddresses.h"
#include "../TCPIP/IPAddress.h"
#include "../TCPIP/TCPServer.h"
#include "../TCPIP/DNSResolver.h"
#include "../../SMTP/SMTPConnection.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String Utilities::cached_win_32computer_name_ = "";
   String Utilities::cached_win_32temp_dir_ = "";

   Utilities::Utilities()
   {
      
   }

   Utilities::~Utilities()
   {

   }

   String 
   Utilities::GetUniqueTempDirectory()
   {
      return FileUtilities::Combine(IniFileSettings::Instance()->GetTempDirectory(), GUIDCreator::GetGUID());
   }


   String 
   Utilities::ComputerName()
   {

      String sComputerName = Configuration::Instance()->GetHostName();

      if (!sComputerName.IsEmpty())
      {
         // Use the computer name specified by the user.
         return sComputerName;
      }
         

      // No username specified. Fetch local computer name.
      if (!cached_win_32computer_name_.IsEmpty())
         return cached_win_32computer_name_;


      cached_win_32computer_name_ = "LOCALHOST";
      TCHAR pCharBuf[255];
      unsigned long iSize = 255;
      if (::GetComputerName(pCharBuf, &iSize) == TRUE)
         cached_win_32computer_name_ = pCharBuf;
      
      return cached_win_32computer_name_ ;

   }

   std::shared_ptr<MimeHeader>
   Utilities::GetMimeHeader(const BYTE *pByteBuf, size_t iBufSize)
   {
      // First locate end of header in the buffer.
      const char *pBuffer = (const char*) pByteBuf;
      const char *pBufferEndPos = StringParser::Search(pBuffer, iBufSize, "\r\n\r\n");

      std::shared_ptr<MimeHeader> pMimeHeader = std::shared_ptr<MimeHeader>(new MimeHeader);

      if (!pBufferEndPos)
      {
         // The end of header could not be found. Message format error.
         return pMimeHeader;
      }

      // Calculate the length of the header.
      size_t iHeaderSize = pBufferEndPos - pBuffer + 2; // +2 for the last newline.

      // Load the header
      pMimeHeader->Load(pBuffer, iHeaderSize, true);

      return pMimeHeader;

   }

   String
   Utilities::GetBinDirectory()
   {
      // The install key in the registry should be enough to tell us where we're installed.
      String install_path;
      Registry registry;

      if (registry.GetStringValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\hMailServer", "InstallLocation", install_path))
      {
         return FileUtilities::Combine(install_path, "Bin");
      }
      else
      {

         // Lookup executable path.
         String executable_full_path = Application::GetExecutableName();
         int last_slash = executable_full_path.ReverseFind(_T("\\"));

         return executable_full_path.Mid(0, last_slash);
      }
   }

   String
   Utilities::GetIPAddress(SOCKADDR_IN addr)
   {
      String sRetVal;
      try
      {
         BYTE bt1 = addr.sin_addr.S_un.S_un_b.s_b1;
         BYTE bt2 = addr.sin_addr.S_un.S_un_b.s_b2;
         BYTE bt3 = addr.sin_addr.S_un.S_un_b.s_b3;
         BYTE bt4 = addr.sin_addr.S_un.S_un_b.s_b4 ;
         sRetVal.Format(_T("%d.%d.%d.%d"), bt1, bt2, bt3, bt4);
      }
      catch (...)
      {
         sRetVal = "<Unknown>";
         throw;
      }

      return sRetVal;
   }



   String 
   Utilities::GenerateMessageID()
   {
      String sGUID = GUIDCreator::GetGUID();
      sGUID.Replace(_T("{"), _T(""));
      sGUID.Replace(_T("}"), _T(""));

      String sRetVal;
      sRetVal.Format(_T("<%s@%s>"), sGUID.c_str(), Utilities::ComputerName().c_str());
      
      return sRetVal;
   }

   bool
   Utilities::IsLocalHost(const String &sHostname)
   {
      USES_CONVERSION;

      SOCKADDR_IN addr;
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = inet_addr(T2A(sHostname));

      struct hostent *host;

      if(addr.sin_addr.s_addr == INADDR_NONE)
      {
         host = NULL;
         host = gethostbyname(T2A(sHostname));
         if (!host)
            return false;

         memcpy(&addr.sin_addr, host->h_addr_list[0], host->h_length);
      }

      String sIPAddress = GetIPAddress(addr);

      IPAddress address;
      address.TryParse(sIPAddress);

      if (LocalIPAddresses::Instance()->IsLocalIPAddress(address))
         return true;
      else
         return false;

   }

   String 
   Utilities::GetRecipientFromReceivedHeader(const String &sReceivedHeader)
   {
      int iLastSemicolon = sReceivedHeader.ReverseFind(_T(";"));
      if (iLastSemicolon == -1)
         return "";

      String sFirstPart = sReceivedHeader.Mid(0, iLastSemicolon);

      /*
      sFirstPart now contains the following
      received =  "Received"    ":"            ; one per relay
            ["from" domain]           ; sending host
            ["by"   domain]           ; receiving host
            ["via"  atom]             ; physical path
            *("with" atom)             ; link/mail protocol
            ["id"   msg-id]           ; receiver msg id
            ["for"  addr-spec]        ; initial form
            ";"    date-time         ; time received

      http://cr.yp.to/immhf/envelope.html
      In theory, the value of a Received field is tokenizable.
      In practice, SMTP servers put all sorts of badly formatted information into Received lines. 
      Hence: We only do a quick search
      */

      int iForPos = sFirstPart.ReverseFind(_T("for "));
      
      if (iForPos == -1)
         return "";
      
      String sAddressPart = sFirstPart.Mid(iForPos + 4);

      sAddressPart.TrimLeft(_T(" \r\n\t"));
      sAddressPart.TrimRight(_T(" \r\n\t"));

      sAddressPart.Replace(_T("<"), _T(""));
      sAddressPart.Replace(_T(">"), _T(""));
      sAddressPart.Replace(_T(" "), _T(""));

      if (!StringParser::IsValidEmailAddress(sAddressPart))
         return "";

      return sAddressPart;
   }

   DateTime
   Utilities::GetDateTimeFromReceivedHeader(const String &sReceivedHeader)
   {
      DateTime dtRetValue;

      int iLastSemicolon = sReceivedHeader.ReverseFind(_T(";"));
      if (iLastSemicolon == -1)
         return dtRetValue;

      String sDatePart = sReceivedHeader.Mid(iLastSemicolon + 1);

      /*
      sFirstPart now contains the following
      received =  "Received"    ":"            ; one per relay
      ["from" domain]           ; sending host
      ["by"   domain]           ; receiving host
      ["via"  atom]             ; physical path
      *("with" atom)             ; link/mail protocol
      ["id"   msg-id]           ; receiver msg id
      ["for"  addr-spec]        ; initial form
      ";"    date-time         ; time received

      http://cr.yp.to/immhf/envelope.html
      In theory, the value of a Received field is tokenizable.
      In practice, SMTP servers put all sorts of badly formatted information into Received lines. 
      Hence: We only do a quick search
      */

      sDatePart.Trim();

      dtRetValue = Time::GetDateTimeFromMimeHeader(sDatePart);
      
      return dtRetValue;
   }

   bool
   Utilities::IsValidIPAddress(const AnsiString string)
   {
      return IPAddress::IsValid(string);
   }


   String
   Utilities::GetHostNameFromReceivedHeader(const String &sReceivedHeader)
   {
      /*
         sFirstPart now contains the following
         received =  "Received"    ":"            ; one per relay
         ["from" domain]           ; sending host
         ["by"   domain]           ; receiving host
         ["via"  atom]             ; physical path
         *("with" atom)             ; link/mail protocol
         ["id"   msg-id]           ; receiver msg id
         ["for"  addr-spec]        ; initial form
         ";"    date-time         ; time received

         http://cr.yp.to/immhf/envelope.html
         In theory, the value of a Received field is tokenizable.
         In practice, SMTP servers put all sorts of badly formatted information into Received lines. 
         Hence: We only do a quick search
      */

      int iFromPos = sReceivedHeader.Find(_T("from "));
      if (iFromPos == -1)
      {
         return "";
      }

      int startPos = iFromPos + 5;
      int endPos = sReceivedHeader.Find(_T(" "), startPos );
      if (endPos == -1)
      {
         assert(0);
         return "";
      }

      int length = endPos - startPos;

      if (length == -1)
      {
         assert(0);
         return "";
      }

      String hostName = sReceivedHeader.Mid(startPos, length);

      if (!StringParser::IsValidDomainName(hostName))
      {
         return "";
      }

      return hostName;

   }

   IPAddress
   Utilities::GetIPAddressFromReceivedHeader(const String &sReceivedHeader)
   {
      /*
      sFirstPart now contains the following
      received =  "Received"    ":"            ; one per relay
      ["from" domain]           ; sending host
      ["by"   domain]           ; receiving host
      ["via"  atom]             ; physical path
      *("with" atom)             ; link/mail protocol
      ["id"   msg-id]           ; receiver msg id
      ["for"  addr-spec]        ; initial form
      ";"    date-time         ; time received

      http://cr.yp.to/immhf/envelope.html
      In theory, the value of a Received field is tokenizable.
      In practice, SMTP servers put all sorts of badly formatted information into Received lines. 
      Hence: We only do a quick search
      */

      int iFromPos = sReceivedHeader.Find(_T("from "));
      if (iFromPos == -1)
      {
         // Could not locate IP address.
         return IPAddress();
      }

      int iBracketPos = sReceivedHeader.Find(_T("["), iFromPos );
      if (iBracketPos == -1)
      {
         // Could not locate IP address.
         return IPAddress();
      }

      int iByPos = sReceivedHeader.Find(_T("by "));
      if (iByPos >= 0 && iByPos < iBracketPos)
      {
         // Found from but no bracket.
         return IPAddress();
      }

      int iBracketEndPos = sReceivedHeader.Find(_T("]"), iBracketPos);

      int iIPLength = iBracketEndPos - iBracketPos - 1;

      String sIPAddress = sReceivedHeader.Mid(iBracketPos + 1, iIPLength);

      if (!StringParser::IsValidIPAddress(sIPAddress))
      {
         // Could not locate IP address
         assert(0);
         return IPAddress();
      }

      IPAddress address;
      address.TryParse(sIPAddress);

      return address;

   }


   void
   UtilitiesTester::Test()
   {
      TestReceivedHeaderParse_();
      TestComputerName_();

      Utilities utilities;
      if (utilities.IsValidIPAddress("127.0.0.A"))
         throw;

      if (!utilities.IsValidIPAddress("127.0.0"))
         throw;

      if (Configuration::Instance()->IsIPv6Available())
      {
         if (!utilities.IsValidIPAddress("2001:0db8:0000:0000:0000:0000:1428:07ab"))
            throw;

         if (!utilities.IsValidIPAddress("001:0db8:0000:0000:0000::1428:07ab"))
            throw;

         if (!utilities.IsValidIPAddress("2001:db8::1428:7ab"))
            throw;

      }
   }

   void 
   UtilitiesTester::TestReceivedHeaderParse_()
   {
      String sHeader = "from host.edu (host.edu [1.2.3.4]) by mail.host.edu (8.8.5) id 004A21; Tue, Mar 18 1997 14:36:17 -0800 (PST)";
      String sRecipient = Utilities::GetRecipientFromReceivedHeader(sHeader);
      if (sRecipient != _T(""))
         throw;

      String hostName = Utilities::GetHostNameFromReceivedHeader(sHeader);
      if (hostName != _T("host.edu"))
            throw;

      sHeader = "from host.edu (host.edu [1.2.3.4]) by mailhost.host.edu (8.8.5/8.7.2) with ESMTP id LAA20869 for <tmh@host.edu>; Tue, 18 Mar 1997 14:39:24 -0800 (PST)";
      sRecipient = Utilities::GetRecipientFromReceivedHeader(sHeader);
      if (sRecipient != _T("tmh@host.edu"))
         throw;

      hostName = Utilities::GetHostNameFromReceivedHeader(sHeader);
      if (hostName != _T("host.edu"))
         throw;

      sHeader = "Received: from unknown (HELO mail-***.icp-***-irony4.*****.***.au) ([123.123.123.123])\r\n "
                "(envelope-sender <sender@sender.com>)\r\n"
                "by mail.*****.***.au (qmail-ldap-1.03) with SMTP\r\n"
                "for <sender@sender.com>; 20 Oct 2006 04:42:47 -0000\r\n";

      sRecipient = Utilities::GetRecipientFromReceivedHeader(sHeader);
      if (sRecipient != _T("sender@sender.com"))
         throw;

      hostName = Utilities::GetHostNameFromReceivedHeader(sHeader);
      if (hostName != _T("unknown"))
         throw;

      sHeader = "Received: from mail.lysator.liu.se (mail.lysator.liu.se [130.236.254.3]) "
                "by j218.ryd.student.liu.se (Postfix) with ESMTP id D84F433035 "
                "for <email@student.liu.se>; Thu, 14 Mar 2002 21:39:17 +0100 (CET) ";

      sRecipient = Utilities::GetRecipientFromReceivedHeader(sHeader);
      if (sRecipient != _T("email@student.liu.se"))
         throw;

      hostName = Utilities::GetHostNameFromReceivedHeader(sHeader);
      if (hostName != _T("mail.lysator.liu.se"))
         throw;

      sHeader = "Received: from mail.lysator.liu.se (mail.lysator.liu.se [130.236.254.3]) "
         "by j218.ryd.student.liu.se (Postfix) with ESMTP id D84F433035 "
         "for <email@student.liu.se>; Thu, 14 Mar 2002 21:39:17 +0100 (CET) ";

      DateTime dtTime = Utilities::GetDateTimeFromReceivedHeader(sHeader);
      if (dtTime.GetYear() != 2002 || 
          dtTime.GetMonth() != 3 ||
          dtTime.GetDay() != 14 ||
          dtTime.GetHour() != 20 ||
          dtTime.GetMinute() != 39 ||
          dtTime.GetSecond() != 17)
         throw;

      hostName = Utilities::GetHostNameFromReceivedHeader(sHeader);
      if (hostName != _T("mail.lysator.liu.se"))
         throw;

      // Test IP Address parsing
      sHeader = "Received: from mail.lysator.liu.se (mail.lysator.liu.se [130.236.254.3]) "
         "by j218.ryd.student.liu.se (Postfix) with ESMTP id D84F433035 "
         "for <email@student.liu.se>; Thu, 14 Mar 2002 21:39:17 +0100 (CET) ";

      String sIPAddress = Utilities::GetIPAddressFromReceivedHeader(sHeader).ToString();
      if (sIPAddress != _T("130.236.254.3"))
         throw;

      hostName = Utilities::GetHostNameFromReceivedHeader(sHeader);
         if (hostName != _T("mail.lysator.liu.se"))
            throw;
      
      sHeader = "from host.edu (host.edu [1.2.3.4]) by mail.host.edu (8.8.5) id 004A21; Tue, Mar 18 1997 14:36:17 -0800 (PST)";
      sIPAddress = Utilities::GetIPAddressFromReceivedHeader(sHeader).ToString();
      if (sIPAddress != _T("1.2.3.4"))
         throw;

      hostName = Utilities::GetHostNameFromReceivedHeader(sHeader);
      if (hostName != _T("host.edu"))
         throw;

      sHeader = "Received: from unknown (HELO mail-***.icp-***-irony4.*****.***.au) ([123.123.123.125])\r\n "
         "(envelope-sender <sender@sender.com>)\r\n"
         "by mail.*****.***.au (qmail-ldap-1.03) with SMTP\r\n"
         "for <sender@sender.com>; 20 Oct 2006 04:42:47 -0000\r\n";
      sIPAddress = Utilities::GetIPAddressFromReceivedHeader(sHeader).ToString();
      if (sIPAddress != _T("123.123.123.125"))
         throw;

      hostName = Utilities::GetHostNameFromReceivedHeader(sHeader);
      if (hostName != _T("unknown"))
        throw;

      sHeader = "Received: from outbound1.den.paypal.com ([216.113.188.96])\r\n"
                "  by mail.hmailserver.com\r\n"
                "  ; Sun, 13 Dec 2009 09:22:54 +0100\r\n";
      
      sIPAddress = Utilities::GetIPAddressFromReceivedHeader(sHeader).ToString();
      if (sIPAddress != _T("216.113.188.96"))
         throw;

      hostName = Utilities::GetHostNameFromReceivedHeader(sHeader);
      if (hostName != _T("outbound1.den.paypal.com"))
         throw;
   }



   void 
   UtilitiesTester::TestComputerName_()
   {
      String sComputerName = Utilities::ComputerName();
      Utilities::IsLocalHost(sComputerName); // We can't check the result of this since 
                                             // it's not always possible to say this safely.
   }


}
