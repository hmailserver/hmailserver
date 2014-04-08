// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Utilities.h"
#include "File.h"
#include "Time.h"

#include "../Mime/Mime.h"
#include "GUIDCreator.h"

#include "../TCPIP/LocalIPAddresses.h"
#include "../TCPIP/IPAddress.h"
#include "../TCPIP/TCPServer.h"
#include "../TCPIP/DNSResolver.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String Utilities::m_sCachedWin32ComputerName = "";
   String Utilities::m_sCachedWin32TempDir = "";

   Utilities::Utilities()
   {
      
   }

   Utilities::~Utilities()
   {

   }

   String 
   Utilities::GetWin32TempDirectory()
   {
      // No username specified. Fetch local computer name.
      if (!m_sCachedWin32TempDir.IsEmpty())
         return m_sCachedWin32TempDir;

      
      unsigned long iSize = 255;
      TCHAR pCharBuf[255];
      
      if (::GetTempPath(iSize, pCharBuf) != 0)
      {
         String sShort = pCharBuf;

         if (sShort.Right(1) == _T("\\"))
            sShort = sShort.Left(sShort.GetLength() - 1);

         m_sCachedWin32TempDir = FileUtilities::GetLongPath(sShort);
        
      }

      return m_sCachedWin32TempDir;

   }

   String 
   Utilities::GetUniqueTempDirectory()
   {
      return FileUtilities::Combine(GetWin32TempDirectory(), GUIDCreator::GetGUID());
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
      if (!m_sCachedWin32ComputerName.IsEmpty())
         return m_sCachedWin32ComputerName;


      m_sCachedWin32ComputerName = "LOCALHOST";
      TCHAR pCharBuf[255];
      unsigned long iSize = 255;
      if (::GetComputerName(pCharBuf, &iSize) == TRUE)
         m_sCachedWin32ComputerName = pCharBuf;
      
      return m_sCachedWin32ComputerName ;

   }

   shared_ptr<MimeHeader>
   Utilities::GetMimeHeader(const BYTE *pByteBuf, int iBufSize)
   {
      // First locate end of header in the buffer.
      const char *pBuffer = (const char*) pByteBuf;
      const char *pBufferEndPos = StringParser::Search(pBuffer, iBufSize, "\r\n\r\n");

      shared_ptr<MimeHeader> pMimeHeader = shared_ptr<MimeHeader>(new MimeHeader);

      if (!pBufferEndPos)
      {
         // The end of header could not be found. Message format error.
         return pMimeHeader;
      }

      // Calculate the length of the header.
      int iHeaderSize = pBufferEndPos - pBuffer + 2; // +2 for the last newline.

      // Load the header
      pMimeHeader->Load(pBuffer, iHeaderSize, true);

      return pMimeHeader;

   }

   String
   Utilities::GetExecutableDirectory()
   {
      String sPathIncExe = Application::GetExecutableName();
      // Find last slash.
      int iLastSlash = sPathIncExe.ReverseFind(_T("\\"));

      return sPathIncExe.Mid(0, iLastSlash);
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
   Utilities::GenerateReceivedHeader(const String &sRemoteIP, String sHostName)
   {
      String sComputerName = Utilities::ComputerName(); 
      vector<String> results;
      // do a PTR lookup, solves an issue with some spam filerting programs such as SA
      // not having a PTR in the Received header.
      String ptrRecord;
      DNSResolver resolver;
      if (!resolver.GetPTRRecords(sRemoteIP, results))
      {
         LOG_DEBUG("Could not get PTR record for IP (false)! " + sRemoteIP);
         ptrRecord = "Unknown";
      }
      else
      {
         if (results.size() == 0)
         {
            LOG_DEBUG("Could not get PTR record for IP (empty)! " + sRemoteIP);
            ptrRecord = "Unknown";
         }
         else ptrRecord = results[0];
      }

      if (sHostName.IsEmpty())
         sHostName = sRemoteIP;

      // Time-stamp-line = "Received:" FWS Stamp <CRLF>
      // Stamp = From-domain By-domain Opt-info ";"  FWS date-time
      // From-domain = "FROM" FWS Extended-Domain CFWS
      // By-domain = "BY" FWS Extended-Domain CFWS
      // Extended-Domain = Domain /
      //                   ( Domain FWS "(" TCP-info ")" ) /           
      //                   ( Address-literal FWS "(" TCP-info ")" )
      // TCP-info        = Address-literal / 
      //                   ( Domain FWS Address-literal )          
      //                   ; Information derived by server from TCP connection
      //                   ; not client EHLO.
      //Opt-info = [Via] [With] [ID] [For]
      //
      // The header produced by hMailServer used to look like this:
      //
      // Received: from <hostinhelo> ([ip-address])
      //              by <thiscomputername> 
      //              with hMailServer; timestamp
      //
      // The header produced by hMailServer now looks like this:
      //
      // Received: from <hostinhelo> ([ip-address])
      //              by <thiscomputername> 
      //              ; timestamp

      // JDR: insert the PTR result here. If none was found Unknown is used.
      String sResult;
      sResult.Format(_T("from %s (%s [%s])\r\n")
                     _T("\tby %s\r\n")
                     _T("\t; %s"), 
                        sHostName,
                        ptrRecord,
                        sRemoteIP,
                        sComputerName, 
                        Time::GetCurrentMimeDate());

      return sResult;

   }

   String 
   Utilities::GenerateMessageID()
   {
      String sGUID = GUIDCreator::GetGUID();
      sGUID.Replace(_T("{"), _T(""));
      sGUID.Replace(_T("}"), _T(""));

      String sRetVal;
      sRetVal.Format(_T("<%s@%s>"), sGUID , Utilities::ComputerName());
      
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
      _TestReceivedHeaderParse();
      _TestComputerName();

      Utilities utilities;
      if (utilities.IsValidIPAddress("127.0.0.A"))
         throw;

      if (!utilities.IsValidIPAddress("127.0.0"))
         throw;

      if (TCPServer::HasIPV6())
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
   UtilitiesTester::_TestReceivedHeaderParse()
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
      if (hostName != _T(""))
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
      if (hostName != _T(""))
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
   UtilitiesTester::_TestComputerName()
   {
      String sComputerName = Utilities::ComputerName();
      Utilities::IsLocalHost(sComputerName); // We can't check the result of this since 
                                             // it's not always possible to say this safely.
   }


}
