// Copyright (c) Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SMTPMessageHeaderCreator.h"

#include "../Common/BO/Message.h"
#include "../Common/BO/MessageRecipients.h"
#include "../Common/BO/MessageRecipient.h"

#include "../Common/TCPIP/CipherInfo.h"
#include "../Common/TCPIP/DNSResolver.h"

#include "../Common/Util/Utilities.h"
#include "../Common/Util/Time.h"
#include "../Common/Util/ByteBuffer.h"
#include "../Common/Mime/Mime.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SMTPMessageHeaderCreator::SMTPMessageHeaderCreator(const String &username, const AnsiString &remote_ip_address, bool is_authenticated, String helo_host, std::shared_ptr<MimeHeader> original_headers, std::shared_ptr<Message> message) :
      username_(username),
      remote_ip_address_(remote_ip_address),
      is_authenticated_(is_authenticated),
      original_headers_(original_headers),
      helo_host_(helo_host),
      is_tls_(false),
      message_(message)
   {

   }

   void
   SMTPMessageHeaderCreator::SetCipherInfo(const CipherInfo &cipher_info)
   {
      cipher_info_ = cipher_info;
      is_tls_ = true;
   }


   AnsiString
   SMTPMessageHeaderCreator::Create()
   {
      // Add received by tag.
      String auth_replacement_ip = IniFileSettings::Instance()->GetAuthUserReplacementIP();
      bool add_x_auth_user_ip = IniFileSettings::Instance()->GetAddXAuthUserIP();
      
      // If sender is logged in and replace IP is enabled use it
      String overriden_received_ip_address;
      String overriden_authenticated_ip_address;
      if (!username_.IsEmpty() && !auth_replacement_ip.empty())
      {
         overriden_received_ip_address = auth_replacement_ip;
         overriden_authenticated_ip_address = remote_ip_address_;
      }
      else
      {
         overriden_received_ip_address = remote_ip_address_;
         overriden_authenticated_ip_address = overriden_received_ip_address;
      }

      String new_header_lines;
      new_header_lines += GenerateReceivedHeader_(overriden_received_ip_address);

      String sComputerName = Utilities::ComputerName();

      // Add Message-ID header if it does not exist.
      if (!original_headers_->FieldExists("Message-ID"))
      {
         String sTemp;
         sTemp.Format(_T("Message-ID: %s\r\n"), Utilities::GenerateMessageID().c_str());
         new_header_lines += sTemp;
      }

      // Add X-AuthUser header if it does not exist.
      if (IniFileSettings::Instance()->GetAddXAuthUserHeader() && !username_.IsEmpty())
      {
         if (!original_headers_->FieldExists("X-AuthUser"))
            new_header_lines += "X-AuthUser: " + username_ + "\r\n";
      }

      if (IniFileSettings::Instance()->GetAddXOriginalRcptToHeader() && message_)
      {
         auto recipients = message_->GetRecipients()->GetVector();
         auto recipientIter = recipients.begin();

         std::vector<String> originalLocalAddresses;

         while (recipientIter != recipients.end())
         {
            auto recipient = *recipientIter;

            if (recipient->GetIsLocalName())
            {
               auto originalAddress = recipient->GetOriginalAddress();

               if (!originalAddress.IsEmpty())
               {
                  originalLocalAddresses.push_back(originalAddress);
               }
            }

            recipientIter++;
         }

         if (originalLocalAddresses.size() > 0)
         {
            String header = "X-Original-Rcpt-To: ";

            String sRcptToAddresses = JoinWithFolding_(originalLocalAddresses, ",", header.GetLength());
            new_header_lines += header + sRcptToAddresses + "\r\n";
         }
      }

      // Now add x- header for AUTH user if enabled since it was replaced above if so
      // Likely would be good idea for this to be optional at some point
      if (!username_.IsEmpty() && !auth_replacement_ip.empty() && add_x_auth_user_ip)
      {
         if (!original_headers_->FieldExists("X-AuthUserIP"))
            new_header_lines += "X-AuthUserIP: " + overriden_authenticated_ip_address + "\r\n";
      }

      AnsiString new_header_lines_ansi = new_header_lines;
      return new_header_lines_ansi;
   }

   String
   SMTPMessageHeaderCreator::GenerateReceivedHeader_(const String &overriden_received_ip)
   {
      String local_computer_name = Utilities::ComputerName();

      std::vector<String> results;
      // do a PTR lookup, solves an issue with some spam filerting programs such as SA
      // not having a PTR in the Received header.
      String ptr_record_host;
      DNSResolver dns_resolver;
      if (!dns_resolver.GetPTRRecords(remote_ip_address_, results) || results.size() == 0)
      {
         LOG_DEBUG("Could not retrieve PTR record for IP (false)! " + remote_ip_address_);
         ptr_record_host = "Unknown";
      }
      else
      {
         ptr_record_host = results[0];
      }

      String remote_hostname = helo_host_.IsEmpty() ? remote_ip_address_ : helo_host_;

      String esmtp_additions;

      if (is_tls_)
         esmtp_additions += "S";

      if (is_authenticated_)
         esmtp_additions += "A";

      String cipher_line;

      if (is_tls_)
         cipher_line.Format(_T("\t(version=%s cipher=%s bits=%d)\r\n"), String(cipher_info_.GetVersion()).c_str(), String(cipher_info_.GetName()).c_str(), cipher_info_.GetBits());

      String sResult;
      sResult.Format(_T("Received: from %s (%s [%s])\r\n")
         _T("\tby %s with ESMTP%s\r\n")
         _T("%s")
         _T("\t; %s\r\n"),
         remote_hostname.c_str(),
         ptr_record_host.c_str(),
         overriden_received_ip.c_str(),
         local_computer_name.c_str(),
         esmtp_additions.c_str(),
         cipher_line.c_str(),
         Time::GetCurrentMimeDate().c_str());

      return sResult;

   }

   String
   SMTPMessageHeaderCreator::JoinWithFolding_(const std::vector<String> &sVector, const String &sSeperator, int initialLineLength)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Joins contents of a vector into a string, with a separator. If the join string exceeds the max
   // line length, it will be split across multiple lines.
   //---------------------------------------------------------------------------()
   {
      auto iterVec = sVector.begin();
      auto iterEnd = sVector.end();

      String result;

      const int maxLineLength = 70;
      int currentLineLength = initialLineLength;

      for (; iterVec != iterEnd; iterVec++)
      {
         String value = (*iterVec);
         int valueLength = value.GetLength();

         if (result.GetLength() > 0 && currentLineLength + value.GetLength() > maxLineLength)
         {
            // Break the line
            result += "\r\n\t";
            currentLineLength = 1;
         }

         result += value;
         currentLineLength += valueLength;

         if (iterVec + 1 != iterEnd)
         {
            result += sSeperator;
            currentLineLength += sSeperator.GetLength();
         }
      }

      return result;
   }

}