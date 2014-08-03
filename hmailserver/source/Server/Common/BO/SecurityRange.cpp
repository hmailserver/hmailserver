// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "SecurityRange.h"
#include "../Util/Time.h"
#include "../TCPIP/IPAddress.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   SecurityRange::SecurityRange() :
      expires_(false),
      options_(0),
      priority_(0)
   {

   }

   SecurityRange::~SecurityRange()
   {

   }

   String 
   SecurityRange::GetLowerIPString() const 
   {
	   return lower_ip_.ToString();
   }

   bool
   SecurityRange::SetLowerIPString(const String & sValue)
   {
      return lower_ip_.TryParse(sValue);
   }

   String 
   SecurityRange::GetUpperIPString() const 
   {
      return upper_ip_.ToString();
   }


   bool
   SecurityRange::SetUpperIPString(const String & sValue)
   {
      return upper_ip_.TryParse(sValue);
   }


   bool
   SecurityRange::GetAllowSMTP() const
   {
	   return (options_ & IPRANGE_ALLOW_SMTP) ? true : false;
   }

   void
   SecurityRange::SetAllowSMTP(bool bAllow)
   {
      if (bAllow)
         options_ = options_ | IPRANGE_ALLOW_SMTP;
      else
         options_ = options_ & ~IPRANGE_ALLOW_SMTP;
	   
   }

   bool
   SecurityRange::GetAllowIMAP() const
   {
	   return (options_ & IPRANGE_ALLOW_IMAP) ? true : false;
   }

   void
   SecurityRange::SetAllowIMAP(bool bAllow)
   {
      if (bAllow)
         options_ = options_ | IPRANGE_ALLOW_IMAP;
      else
         options_ = options_ & ~IPRANGE_ALLOW_IMAP;
	   
   }

   bool
   SecurityRange::GetAllowPOP3() const
   {
	   return (options_ & IPRANGE_ALLOW_POP3) ? true : false;
   }

   void
   SecurityRange::SetAllowPOP3(bool bAllow)
   {
      if (bAllow)
         options_ = options_ | IPRANGE_ALLOW_POP3;
      else
         options_ = options_ & ~IPRANGE_ALLOW_POP3;
	   
   }

   bool
   SecurityRange::GetRequireSMTPAuthLocalToLocal() const
   {
      return GetAllowOption(IPRANGE_SMTP_AUTH_LOCAL_TO_LOCAL);
   }

   void
   SecurityRange::SetRequireSMTPAuthLocalToLocal(bool bAllow)
   {
      SetAllowOption(IPRANGE_SMTP_AUTH_LOCAL_TO_LOCAL, bAllow);
   }

   bool
   SecurityRange::GetRequireSMTPAuthLocalToExternal() const
   {
      return GetAllowOption(IPRANGE_SMTP_AUTH_LOCAL_TO_EXTERNAL);
   }

   void
   SecurityRange::SetRequireSMTPAuthLocalToExternal(bool bAllow)
   {
      SetAllowOption(IPRANGE_SMTP_AUTH_LOCAL_TO_EXTERNAL, bAllow);
   }

   bool
   SecurityRange::GetRequireSMTPAuthExternalToLocal() const
   {
      return GetAllowOption(IPRANGE_SMTP_AUTH_EXTERNAL_TO_LOCAL);
   }

   void
   SecurityRange::SetRequireSMTPAuthExternalToLocal(bool bAllow)
   {
      SetAllowOption(IPRANGE_SMTP_AUTH_EXTERNAL_TO_LOCAL, bAllow);
   }

   bool
   SecurityRange::GetRequireSMTPAuthExternalToExternal() const
   {
      return GetAllowOption(IPRANGE_SMTP_AUTH_EXTERNAL_TO_EXTERNAL);
   }

   void
   SecurityRange::SetRequireSMTPAuthExternalToExternal(bool bAllow)
   {
      SetAllowOption(IPRANGE_SMTP_AUTH_EXTERNAL_TO_EXTERNAL, bAllow);
   }

   bool
   SecurityRange::GetRequireTLSForAuth() const
   {
      return GetAllowOption(IPRANGE_REQUIRE_TLS_FOR_AUTH);
   }

   void
   SecurityRange::SetRequireTLSForAuth(bool bAllow)
   {
      SetAllowOption(IPRANGE_REQUIRE_TLS_FOR_AUTH, bAllow);
   }

   bool
   SecurityRange::GetAllowRelayL2L() const
   {
	   return (options_ & IPRANGE_RELAY_LOCAL_TO_LOCAL) ? true : false;
   }

   void
   SecurityRange::SetAllowRelayL2L(bool bAllow)
   {
      if (bAllow)
         options_ = options_ | IPRANGE_RELAY_LOCAL_TO_LOCAL;
      else
         options_ = options_ & ~IPRANGE_RELAY_LOCAL_TO_LOCAL;
	   
   }

   bool
   SecurityRange::GetAllowRelayL2R() const
   {
	   return (options_ & IPRANGE_RELAY_LOCAL_TO_REMOTE) ? true : false;
   }

   void
   SecurityRange::SetAllowRelayL2R(bool bAllow)
   {
      if (bAllow)
         options_ = options_ | IPRANGE_RELAY_LOCAL_TO_REMOTE;
      else
         options_ = options_ & ~IPRANGE_RELAY_LOCAL_TO_REMOTE;
	   
   }

   bool
   SecurityRange::GetAllowRelayR2L() const
   {
	   return (options_ & IPRANGE_RELAY_REMOTE_TO_LOCAL) ? true : false;
   }

   void
   SecurityRange::SetAllowRelayR2L(bool bAllow)
   {
      if (bAllow)
         options_ = options_ | IPRANGE_RELAY_REMOTE_TO_LOCAL;
      else
         options_ = options_ & ~IPRANGE_RELAY_REMOTE_TO_LOCAL;
	   
   }

   bool
   SecurityRange::GetAllowRelayR2R() const
   {
	   return (options_ & IPRANGE_RELAY_REMOTE_TO_REMOTE) ? true : false;
   }

   void
   SecurityRange::SetAllowRelayR2R(bool bAllow)
   {
      if (bAllow)
         options_ = options_ | IPRANGE_RELAY_REMOTE_TO_REMOTE;
      else
         options_ = options_ & ~IPRANGE_RELAY_REMOTE_TO_REMOTE;
   }

   bool
   SecurityRange::GetSpamProtection() const
   {
      return (options_ & IPRANGE_SPAM_PROTECTION) ? true : false;
   }

   void
   SecurityRange::SetSpamProtection(bool bAllow)
   {
      if (bAllow)
         options_ = options_ | IPRANGE_SPAM_PROTECTION;
      else
         options_ = options_ & ~IPRANGE_SPAM_PROTECTION;
   }

   bool
   SecurityRange::GetVirusProtection() const
   {
      return (options_ & IPRANGE_VIRUS_PROTECTION) ? true : false;
   }

   void
   SecurityRange::SetVirusProtection(bool bAllow)
   {
      if (bAllow)
         options_ = options_ | IPRANGE_VIRUS_PROTECTION;
      else
         options_ = options_ & ~IPRANGE_VIRUS_PROTECTION;
   }

   bool
   SecurityRange::GetAllowOption(__int64 lOption) const
   {
      return (options_ & lOption) ? true : false;
   }

   void 
   SecurityRange::SetAllowOption(int option, bool allow)
   {
      if (allow)
         options_ = options_ | option;
      else
         options_ = options_ & ~option;
   }

   bool
   SecurityRange::GetExpires() const
   {
      return expires_;
   }

   void 
   SecurityRange::SetExpires(bool expires)
   {
      expires_ = expires;
   }

   DateTime
   SecurityRange::GetExpiresTime() const
   {
      return expires_Time;
   }

   void
   SecurityRange::SetExpiresTime(const DateTime &expiresTime)
   {
      expires_Time = expiresTime;
   }

   bool 
   SecurityRange::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("SecurityRange"));

      pNode->AppendAttr(_T("Name"), name_);
      pNode->AppendAttr(_T("LowerIP"), String(lower_ip_.ToString()));
      pNode->AppendAttr(_T("UpperIP"), String(upper_ip_.ToString()));
      pNode->AppendAttr(_T("Priority"), StringParser::IntToString(priority_));
      pNode->AppendAttr(_T("Options"), StringParser::IntToString(options_));

      pNode->AppendAttr(_T("ExpiresTime"), Time::GetTimeStampFromDateTime(expires_Time));
      pNode->AppendAttr(_T("Expires"), expires_ ? _T("1") : _T("0"));

      return true;
   }

   bool 
   SecurityRange::XMLLoad(XNode *pSecurityRangeNode, int iOptions)
   {
      name_ = pSecurityRangeNode->GetAttrValue(_T("Name"));
      lower_ip_.TryParse(pSecurityRangeNode->GetAttrValue(_T("LowerIP")));
      upper_ip_.TryParse(pSecurityRangeNode->GetAttrValue(_T("UpperIP")));
      priority_ = _ttoi(pSecurityRangeNode->GetAttrValue(_T("Priority")));
      options_ = _ttoi(pSecurityRangeNode->GetAttrValue(_T("Options")));
   
      expires_Time = Time::GetDateFromSystemDate(pSecurityRangeNode->GetAttrValue(_T("ExpiresTime")));
      expires_ = pSecurityRangeNode->GetAttrValue(_T("Expires")) == _T("1");

      return true;
   }
}