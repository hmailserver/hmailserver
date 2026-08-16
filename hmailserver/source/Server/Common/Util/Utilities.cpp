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
         // The header ends directly after the host name.
         endPos = sReceivedHeader.GetLength();
      }

      int length = endPos - startPos;

      if (length <= 0)
      {
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

      // Only the part of the header describing the sending host is of interest.
      // What follows "by" describes the receiving host, and may contain values
      // which look like IP addresses, such as software version numbers.
      int iEndPos = sReceivedHeader.Find(_T("by "), iFromPos);
      if (iEndPos == -1)
         iEndPos = sReceivedHeader.GetLength();

      // The sending host may be described both by the host name it presented in
      // HELO/EHLO, and by the address the receiving server saw it connect from:
      //
      //    from [198.51.100.7] (unknown [203.0.113.99]) by ...
      //
      // The client decides what to put in HELO/EHLO, so when both are given, the
      // address observed by the receiving server - the last one - is the one to
      // use.
      IPAddress result;

      int iSearchPos = iFromPos;
      int iBracketPos = sReceivedHeader.Find(_T("["), iSearchPos);

      while (iBracketPos >= 0 && iBracketPos < iEndPos)
      {
         int iBracketEndPos = sReceivedHeader.Find(_T("]"), iBracketPos);
         if (iBracketEndPos == -1 || iBracketEndPos > iEndPos)
            break;

         String sPreceding = sReceivedHeader.Mid(iSearchPos, iBracketPos - iSearchPos);
         sPreceding.TrimRight();

         // Some servers state the host name given in HELO/EHLO in the comment -
         // "(helo=[198.51.100.7])" - instead of the address they observed. What
         // is marked that way comes from the client, and is not used.
         bool givenInHelo = sPreceding.EndsWith(_T("helo=")) ||
                            sPreceding.EndsWith(_T(" helo")) ||
                            sPreceding.EndsWith(_T("(helo"));

         if (!givenInHelo)
         {
            String sIPAddress = sReceivedHeader.Mid(iBracketPos + 1, iBracketEndPos - iBracketPos - 1);

            IPAddress address;

            if (address.TryParse(sIPAddress, false))
               result = address;
         }

         iSearchPos = iBracketEndPos + 1;
         iBracketPos = sReceivedHeader.Find(_T("["), iSearchPos);
      }

      return result;

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

      TestHeloDependentReceivedHeaderParse_();
      TestMalformedReceivedHeaderParse_();
   }

   void
   UtilitiesTester::TestHeloDependentReceivedHeaderParse_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // https://github.com/hmailserver/hmailserver/issues/168
   //
   // The host name a client presents in HELO/EHLO is copied verbatim into the
   // "from" part of the Received header. Since the sender decides what to put
   // there, nothing in that part of the header may be allowed to decide which IP
   // address we consider the message to originate from. The IP address which the
   // server writing the header actually observed is the one within the
   // parentheses.
   //---------------------------------------------------------------------------()
   {
      // The client presented an address literal in HELO. 203.0.113.99 is the IP
      // address the message was received from; 198.51.100.7 is the literal the
      // sender chose to present.
      AssertIPAddress_("Received: from [198.51.100.7] (unknown [203.0.113.99])\r\n"
                       "\tby mail.example.test with ESMTP\r\n"
                       "\t; Fri, 06 May 2016 03:49:14 +0200\r\n", _T("203.0.113.99"));

      // Same header, written in the format hMailServer itself uses.
      AssertIPAddress_("Received: from [198.51.100.7] (Unknown [203.0.113.99])\r\n"
                       "\tby MAILSERVER with ESMTP\r\n"
                       "\t; Fri, 06 May 2016 03:49:14 +0200\r\n", _T("203.0.113.99"));

      // The client presented an unqualified host name which isn't a valid domain
      // name. The IP address is still unambiguous.
      AssertIPAddress_("Received: from my_pc (unknown [203.0.113.99])\r\n"
                       "\tby mail.example.test with ESMTP\r\n"
                       "\t; Fri, 06 May 2016 03:49:14 +0200\r\n", _T("203.0.113.99"));

      // Servers which state the observed address first, and the host name given
      // in HELO/EHLO in the comment.
      AssertIPAddress_("Received: from [203.0.113.99] (helo=mail.example.test)\r\n"
                       "\tby mail.example.test with esmtp id 1abcDE-000123-45\r\n"
                       "\t; Fri, 06 May 2016 03:49:14 +0200\r\n", _T("203.0.113.99"));

      AssertIPAddress_("Received: from [203.0.113.99] (port=54321 helo=[198.51.100.7])\r\n"
                       "\tby mail.example.test with esmtp id 1abcDE-000123-45\r\n"
                       "\t; Fri, 06 May 2016 03:49:14 +0200\r\n", _T("203.0.113.99"));

      AssertIPAddress_("Received: from unknown (HELO [198.51.100.7]) ([203.0.113.99])\r\n"
                       "\tby mail.example.test (qmail-ldap-1.03) with SMTP\r\n"
                       "\t; Fri, 06 May 2016 03:49:14 +0200\r\n", _T("203.0.113.99"));

      // The host name is reported as the sender presented it, so that the HELO
      // host name spam test can compare the two.
      AssertHostName_("Received: from [198.51.100.7] (unknown [203.0.113.99])\r\n"
                      "\tby mail.example.test with ESMTP\r\n"
                      "\t; Fri, 06 May 2016 03:49:14 +0200\r\n", _T("[198.51.100.7]"));
   }

   void
   UtilitiesTester::TestMalformedReceivedHeaderParse_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Received headers are written by other servers, and partly from data given
   // by the client, so anything may end up in them. Parsing one must never
   // report an address which isn't stated in the header - anti spam tests are
   // run against whatever is reported.
   //---------------------------------------------------------------------------()
   {
      // Only the part of the header describing the sending host is parsed. What
      // follows "by" describes the receiving host.
      AssertIPAddress_("from mail.example.test by relay.example.test [203.0.113.99]; date", _T("0.0.0.0"));
      AssertIPAddress_("from mail.example.test (unknown [203.0.113.99])"
                       " by relay.example.test (Postfix) with ESMTP id [198.51.100.7]; date", _T("203.0.113.99"));
      AssertIPAddress_("from host.edu (host.edu [1.2.3.4]) by mail.host.edu (8.8.5) id 004A21; date", _T("1.2.3.4"));

      // Headers which don't state where the message came from.
      AssertIPAddress_("", _T("0.0.0.0"));
      AssertIPAddress_("by 10.103.12.12 with SMTP id p12cs158601mui; date", _T("0.0.0.0"));
      AssertIPAddress_("from mail.example.test by relay.example.test; date", _T("0.0.0.0"));
      AssertIPAddress_("Received: from ", _T("0.0.0.0"));

      // Headers where the address can't be parsed.
      AssertIPAddress_("from mail.example.test ([]) by relay.example.test; date", _T("0.0.0.0"));
      AssertIPAddress_("from mail.example.test (unknown [not-an-ip-address]) by relay.example.test; date", _T("0.0.0.0"));
      AssertIPAddress_("from mail.example.test (unknown [203.0.113.999]) by relay.example.test; date", _T("0.0.0.0"));
      AssertIPAddress_("from [203.0.113.99 (unknown [198.51.100.7]) by relay.example.test; date", _T("0.0.0.0"));

      // IPv6.
      AssertIPAddress_("from mail.example.test (mail.example.test [2001:db8::1])\r\n"
                       "\tby mail.example.test with ESMTP\r\n"
                       "\t; date", _T("2001:db8::1"));
      AssertIPAddress_("from [IPv6:2001:db8::1] (unknown [2001:db8::99])\r\n"
                       "\tby mail.example.test with ESMTP\r\n"
                       "\t; date", _T("2001:db8::99"));

      // Host names which aren't valid domain names are not reported. The address
      // stated by the header is used regardless of them.
      AssertHostName_("from mail.example.test(unknown [203.0.113.99]) by relay.example.test; date", _T(""));
      AssertHostName_("from mail.example.test. (unknown [203.0.113.99]) by relay.example.test; date", _T(""));
      AssertHostName_("from my_pc (unknown [203.0.113.99]) by relay.example.test; date", _T(""));
      AssertHostName_("from -mail.example.test (unknown [203.0.113.99]) by relay.example.test; date", _T(""));
      AssertHostName_("from [IPv6:2001:db8::1] (unknown [203.0.113.99]) by relay.example.test; date", _T(""));

      AssertIPAddress_("from mail.example.test(unknown [203.0.113.99]) by relay.example.test; date", _T("203.0.113.99"));
      AssertIPAddress_("from my_pc (unknown [203.0.113.99]) by relay.example.test; date", _T("203.0.113.99"));

      // Host names which are.
      AssertHostName_("from mail.example.test (unknown [203.0.113.99]) by relay.example.test; date", _T("mail.example.test"));
      AssertHostName_("from localhost (unknown [203.0.113.99]) by relay.example.test; date", _T("localhost"));
      AssertHostName_("from [198.51.100.7] (unknown [203.0.113.99]) by relay.example.test; date", _T("[198.51.100.7]"));
      AssertHostName_("from [IPv6:2001:0db8:85a3:0000:0000:8a2e:0370:7334] (unknown [203.0.113.99]) by relay.example.test; date",
                      _T("[IPv6:2001:0db8:85a3:0000:0000:8a2e:0370:7334]"));

      // Headers which don't state a host name.
      AssertHostName_("", _T(""));
      AssertHostName_("by 10.103.12.12 with SMTP id p12cs158601mui; date", _T(""));
      AssertHostName_("Received: from ", _T(""));
   }

   void
   UtilitiesTester::AssertHostName_(const String &receivedHeader, const String &expectedHostName)
   {
      String actualHostName = Utilities::GetHostNameFromReceivedHeader(receivedHeader);

      if (actualHostName != expectedHostName)
      {
         throw std::logic_error(Formatter::FormatAsAnsi("Expected the host name {0}, but got {1}. Header: {2}",
            expectedHostName, actualHostName, receivedHeader));
      }
   }

   void
   UtilitiesTester::AssertIPAddress_(const String &receivedHeader, const String &expectedIPAddress)
   {
      String actualIPAddress = Utilities::GetIPAddressFromReceivedHeader(receivedHeader).ToString();

      if (actualIPAddress != expectedIPAddress)
      {
         throw std::logic_error(Formatter::FormatAsAnsi("Expected the IP address {0}, but got {1}. Header: {2}",
            expectedIPAddress, actualIPAddress, receivedHeader));
      }
   }

   void 
   UtilitiesTester::TestComputerName_()
   {
      String sComputerName = Utilities::ComputerName();
      Utilities::IsLocalHost(sComputerName); // We can't check the result of this since 
                                             // it's not always possible to say this safely.
   }


}
