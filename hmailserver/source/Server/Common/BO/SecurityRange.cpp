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
      _expires(false),
      m_lOptions(0),
      m_lPriority(0)
   {

   }

   SecurityRange::~SecurityRange()
   {

   }

   String 
   SecurityRange::GetLowerIPString() const 
   {
	   return m_lLowerIP.ToString();
   }

   bool
   SecurityRange::SetLowerIPString(const String & sValue)
   {
      return m_lLowerIP.TryParse(sValue);
   }

   String 
   SecurityRange::GetUpperIPString() const 
   {
      return m_lUpperIP.ToString();
   }


   bool
   SecurityRange::SetUpperIPString(const String & sValue)
   {
      return m_lUpperIP.TryParse(sValue);
   }


   bool
   SecurityRange::GetAllowSMTP() const
   {
	   return (m_lOptions & IPRANGE_ALLOW_SMTP) ? true : false;
   }

   void
   SecurityRange::SetAllowSMTP(bool bAllow)
   {
      if (bAllow)
         m_lOptions = m_lOptions | IPRANGE_ALLOW_SMTP;
      else
         m_lOptions = m_lOptions & ~IPRANGE_ALLOW_SMTP;
	   
   }

   bool
   SecurityRange::GetAllowIMAP() const
   {
	   return (m_lOptions & IPRANGE_ALLOW_IMAP) ? true : false;
   }

   void
   SecurityRange::SetAllowIMAP(bool bAllow)
   {
      if (bAllow)
         m_lOptions = m_lOptions | IPRANGE_ALLOW_IMAP;
      else
         m_lOptions = m_lOptions & ~IPRANGE_ALLOW_IMAP;
	   
   }

   bool
   SecurityRange::GetAllowPOP3() const
   {
	   return (m_lOptions & IPRANGE_ALLOW_POP3) ? true : false;
   }

   void
   SecurityRange::SetAllowPOP3(bool bAllow)
   {
      if (bAllow)
         m_lOptions = m_lOptions | IPRANGE_ALLOW_POP3;
      else
         m_lOptions = m_lOptions & ~IPRANGE_ALLOW_POP3;
	   
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
   SecurityRange::GetAllowRelayL2L() const
   {
	   return (m_lOptions & IPRANGE_RELAY_LOCAL_TO_LOCAL) ? true : false;
   }

   void
   SecurityRange::SetAllowRelayL2L(bool bAllow)
   {
      if (bAllow)
         m_lOptions = m_lOptions | IPRANGE_RELAY_LOCAL_TO_LOCAL;
      else
         m_lOptions = m_lOptions & ~IPRANGE_RELAY_LOCAL_TO_LOCAL;
	   
   }

   bool
   SecurityRange::GetAllowRelayL2R() const
   {
	   return (m_lOptions & IPRANGE_RELAY_LOCAL_TO_REMOTE) ? true : false;
   }

   void
   SecurityRange::SetAllowRelayL2R(bool bAllow)
   {
      if (bAllow)
         m_lOptions = m_lOptions | IPRANGE_RELAY_LOCAL_TO_REMOTE;
      else
         m_lOptions = m_lOptions & ~IPRANGE_RELAY_LOCAL_TO_REMOTE;
	   
   }

   bool
   SecurityRange::GetAllowRelayR2L() const
   {
	   return (m_lOptions & IPRANGE_RELAY_REMOTE_TO_LOCAL) ? true : false;
   }

   void
   SecurityRange::SetAllowRelayR2L(bool bAllow)
   {
      if (bAllow)
         m_lOptions = m_lOptions | IPRANGE_RELAY_REMOTE_TO_LOCAL;
      else
         m_lOptions = m_lOptions & ~IPRANGE_RELAY_REMOTE_TO_LOCAL;
	   
   }

   bool
   SecurityRange::GetAllowRelayR2R() const
   {
	   return (m_lOptions & IPRANGE_RELAY_REMOTE_TO_REMOTE) ? true : false;
   }

   void
   SecurityRange::SetAllowRelayR2R(bool bAllow)
   {
      if (bAllow)
         m_lOptions = m_lOptions | IPRANGE_RELAY_REMOTE_TO_REMOTE;
      else
         m_lOptions = m_lOptions & ~IPRANGE_RELAY_REMOTE_TO_REMOTE;
   }

   bool
   SecurityRange::GetSpamProtection() const
   {
      return (m_lOptions & IPRANGE_SPAM_PROTECTION) ? true : false;
   }

   void
   SecurityRange::SetSpamProtection(bool bAllow)
   {
      if (bAllow)
         m_lOptions = m_lOptions | IPRANGE_SPAM_PROTECTION;
      else
         m_lOptions = m_lOptions & ~IPRANGE_SPAM_PROTECTION;
   }

   bool
   SecurityRange::GetVirusProtection() const
   {
      return (m_lOptions & IPRANGE_VIRUS_PROTECTION) ? true : false;
   }

   void
   SecurityRange::SetVirusProtection(bool bAllow)
   {
      if (bAllow)
         m_lOptions = m_lOptions | IPRANGE_VIRUS_PROTECTION;
      else
         m_lOptions = m_lOptions & ~IPRANGE_VIRUS_PROTECTION;
   }

   bool
   SecurityRange::GetAllowOption(__int64 lOption) const
   {
      return (m_lOptions & lOption) ? true : false;
   }

   void 
   SecurityRange::SetAllowOption(int option, bool allow)
   {
      if (allow)
         m_lOptions = m_lOptions | option;
      else
         m_lOptions = m_lOptions & ~option;
   }

   bool
   SecurityRange::GetExpires() const
   {
      return _expires;
   }

   void 
   SecurityRange::SetExpires(bool expires)
   {
      _expires = expires;
   }

   DateTime
   SecurityRange::GetExpiresTime() const
   {
      return _expiresTime;
   }

   void
   SecurityRange::SetExpiresTime(const DateTime &expiresTime)
   {
      _expiresTime = expiresTime;
   }

   bool 
   SecurityRange::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("SecurityRange"));

      pNode->AppendAttr(_T("Name"), m_sName);
      pNode->AppendAttr(_T("LowerIP"), String(m_lLowerIP.ToString()));
      pNode->AppendAttr(_T("UpperIP"), String(m_lUpperIP.ToString()));
      pNode->AppendAttr(_T("Priority"), StringParser::IntToString(m_lPriority));
      pNode->AppendAttr(_T("Options"), StringParser::IntToString(m_lOptions));

      pNode->AppendAttr(_T("ExpiresTime"), Time::GetTimeStampFromDateTime(_expiresTime));
      pNode->AppendAttr(_T("Expires"), _expires ? _T("1") : _T("0"));

      return true;
   }

   bool 
   SecurityRange::XMLLoad(XNode *pSecurityRangeNode, int iOptions)
   {
      m_sName = pSecurityRangeNode->GetAttrValue(_T("Name"));
      m_lLowerIP.TryParse(pSecurityRangeNode->GetAttrValue(_T("LowerIP")));
      m_lUpperIP.TryParse(pSecurityRangeNode->GetAttrValue(_T("UpperIP")));
      m_lPriority = _ttoi(pSecurityRangeNode->GetAttrValue(_T("Priority")));
      m_lOptions = _ttoi(pSecurityRangeNode->GetAttrValue(_T("Options")));
   
      _expiresTime = Time::GetDateFromSystemDate(pSecurityRangeNode->GetAttrValue(_T("ExpiresTime")));
      _expires = pSecurityRangeNode->GetAttrValue(_T("Expires")) == _T("1");

      return true;
   }
}