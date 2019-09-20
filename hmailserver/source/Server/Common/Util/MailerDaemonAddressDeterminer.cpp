// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "MailerDaemonAddressDeterminer.h"
#include "../Persistence/PersistentDomain.h"
#include "../BO/MessageRecipient.h"
#include "../BO/MessageRecipients.h"
#include "../BO/Message.h"
#include "../Util/Utilities.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MailerDaemonAddressDeterminer::MailerDaemonAddressDeterminer()
   {

   }

   MailerDaemonAddressDeterminer::~MailerDaemonAddressDeterminer()
   {

   }

   String
   MailerDaemonAddressDeterminer::GetMailerDaemonAddress(const std::shared_ptr<Message> pOrigMessage)
   {
      String sOriginalSender = pOrigMessage->GetFromAddress();
      String sOriginialRecipient;

      std::shared_ptr<MessageRecipients> pRecipients = pOrigMessage->GetRecipients();
      std::vector<std::shared_ptr<MessageRecipient> > & recipients = pRecipients->GetVector();
      if (recipients.size() > 0)
         sOriginialRecipient = (*recipients.begin())->GetAddress();

      return GetMailerDaemonAddress(sOriginalSender, sOriginialRecipient);
      
   }

   String
   MailerDaemonAddressDeterminer::GetMailerDaemonAddress(const String &sOrigSender, const String &sOrigReceiver)
   {
      String sHostName = Configuration::Instance()->GetHostName();

      if (sHostName.IsEmpty())
      {
         // A: Check if sender is on a local domain. If so, we should use mailer-daemon@localdomain.com   
         String sSenderDomain = StringParser::ExtractDomain(sOrigSender);
         bool bIsActive = false;
         if (PersistentDomain::DomainExists(sSenderDomain, bIsActive))
            sHostName = sSenderDomain;
      }

      if (sHostName.IsEmpty())
      {
         // A: Check if sender is on a local domain. If so, we should use mailer-daemon@localdomain.com   
         String sReceiverDomain = StringParser::ExtractDomain(sOrigReceiver);
         bool bIsActive = false;
         if (PersistentDomain::DomainExists(sReceiverDomain, bIsActive))
            sHostName = sReceiverDomain;
      }

      if (sHostName.IsEmpty())
      {
         sHostName = Utilities::ComputerName();
      }

      if (!IniFileSettings::Instance()->GetDaemonAddressDomain().IsEmpty())
      {
         sHostName = IniFileSettings::Instance()->GetDaemonAddressDomain();
      }

      String sRetVal;
      sRetVal.Format(_T("mailer-daemon@%s"), sHostName.c_str());

      return sRetVal;

   }

   
   bool
   MailerDaemonAddressDeterminer::IsMailerDaemonAddress(const String &sAddress)
   {
      String sAddressPart = StringParser::ExtractAddress(sAddress);

      if (sAddressPart.CompareNoCase(_T("MAILER-DAEMON")) == 0)
          return true;
      else
         return false;
   }


}











































