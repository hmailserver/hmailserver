// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


#pragma once

#include "../Util/VariantDateTime.h"

namespace HM
{
   class SecurityRange : public BusinessObject<SecurityRange>
   {
   public:
	   SecurityRange();
	   virtual ~SecurityRange();

      enum RangeOptions
      {
         IPRANGE_ALLOW_SMTP = 1,
         IPRANGE_ALLOW_POP3 = 2,
         IPRANGE_ALLOW_IMAP = 8,
         //IPRANGE_REQUIRE_SMTP_AUTH_ONLY_EXTERNAL = 16,
         //IPRANGE_REQUIRE_SMTP_AUTH = 32,
         IPRANGE_RELAY_LOCAL_TO_LOCAL = 64,
         IPRANGE_RELAY_LOCAL_TO_REMOTE = 128,
         IPRANGE_RELAY_REMOTE_TO_LOCAL = 256,
         IPRANGE_RELAY_REMOTE_TO_REMOTE = 512,
         IPRANGE_SPAM_PROTECTION = 1024,
         // IPRANGE_FORWARDING_RELAY = 2048, -- no longer used, as of 5.1. stored in separate object.
         IPRANGE_VIRUS_PROTECTION = 4096,
         IPRANGE_SMTP_AUTH_LOCAL_TO_LOCAL = 8192,
         IPRANGE_SMTP_AUTH_LOCAL_TO_EXTERNAL = 16384,
         IPRANGE_SMTP_AUTH_EXTERNAL_TO_LOCAL = 32768,
         IPRANGE_SMTP_AUTH_EXTERNAL_TO_EXTERNAL = 65536,

      };


   public:
	   String GetLowerIPString() const ;
	   String GetUpperIPString()const ;
	   bool SetLowerIPString(const String & sLowerIP);
	   bool SetUpperIPString(const String & sUpperIP);

	   void SetLowerIP(const IPAddress &lNewVal){m_lLowerIP = lNewVal;}
	   void SetUpperIP(const IPAddress &lNewVal){m_lUpperIP = lNewVal;}

      IPAddress GetLowerIP() const {return m_lLowerIP;}
      IPAddress GetUpperIP() const {return m_lUpperIP;}

	   long GetOptions() const { return m_lOptions;}
	   void SetOptions(long lNewVal) {m_lOptions = lNewVal; }

	   long GetPriority() const {return m_lPriority;}
	   void SetPriority(long lNewVal) {m_lPriority = lNewVal;}

	   bool GetAllowSMTP() const ;
	   void SetAllowSMTP(bool bAllow);

	   bool GetAllowIMAP() const ;
	   void SetAllowIMAP(bool bAllow);

	   bool GetAllowPOP3() const ;
	   void SetAllowPOP3(bool bAllow);

	   bool GetRequireSMTPAuthLocalToLocal() const ;
	   void SetRequireSMTPAuthLocalToLocal(bool bAllow);

      bool GetRequireSMTPAuthLocalToExternal() const ;
      void SetRequireSMTPAuthLocalToExternal(bool bAllow);

      bool GetRequireSMTPAuthExternalToLocal() const ;
      void SetRequireSMTPAuthExternalToLocal(bool bAllow);

      bool GetRequireSMTPAuthExternalToExternal() const ;
      void SetRequireSMTPAuthExternalToExternal(bool bAllow);

      bool GetAllowRelayL2L() const ;
	   void SetAllowRelayL2L(bool bAllow);

	   bool GetAllowRelayL2R()const ;
	   void SetAllowRelayL2R(bool bAllow);

	   bool GetAllowRelayR2L() const ;
	   void SetAllowRelayR2L(bool bAllow);

	   bool GetAllowRelayR2R() const ;
	   void SetAllowRelayR2R(bool bAllow);

      bool GetSpamProtection() const ;
      void SetSpamProtection(bool bAllow);

      bool GetVirusProtection() const ;
      void SetVirusProtection(bool bAllow);

      bool GetAllowOption(__int64 lOption) const ;
      
      String GetName() const  {return m_sName; }
      void SetName(const String &sNewVal) {m_sName = sNewVal;}

      bool GetExpires() const;
      void SetExpires(bool expires);

      DateTime GetExpiresTime() const;
      void SetExpiresTime(const DateTime &expiresTime);

      bool XMLStore (XNode *pParentNode, int iOptions);
      bool XMLLoad (XNode *pSecurityRangeNode, int iOptions);
      bool XMLLoadSubItems (XNode *pSecurityRangeNode, int iOptions) {return true;};

   private:

	   void SetAllowOption(int option, bool allow);

	   IPAddress m_lLowerIP;
	   IPAddress m_lUpperIP;

       // Options for this range
       long m_lOptions;

	   // the priority. 0 is highest, 10 is lowest.
	   long m_lPriority;
      
      String m_sName;

      bool _expires;
      DateTime _expiresTime;
   };

}
