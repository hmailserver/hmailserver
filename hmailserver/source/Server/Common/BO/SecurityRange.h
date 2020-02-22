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

         IPRANGE_REQUIRE_TLS_FOR_AUTH = 131072
      };


   public:
	   String GetLowerIPString() const ;
	   String GetUpperIPString()const ;
	   bool SetLowerIPString(const String & sLowerIP);
	   bool SetUpperIPString(const String & sUpperIP);

	   void SetLowerIP(const IPAddress &lNewVal){lower_ip_ = lNewVal;}
	   void SetUpperIP(const IPAddress &lNewVal){upper_ip_ = lNewVal;}

      IPAddress GetLowerIP() const {return lower_ip_;}
      IPAddress GetUpperIP() const {return upper_ip_;}

	   long GetOptions() const { return options_;}
	   void SetOptions(long lNewVal) {options_ = lNewVal; }

	   long GetPriority() const {return priority_;}
	   void SetPriority(long lNewVal) {priority_ = lNewVal;}

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
      
      String GetName() const  {return name_; }
      void SetName(const String &sNewVal) {name_ = sNewVal;}

      bool GetExpires() const;
      void SetExpires(bool expires);

      DateTime GetExpiresTime() const;
      void SetExpiresTime(const DateTime &expiresTime);

      bool GetRequireTLSForAuth() const ;
      void SetRequireTLSForAuth(bool require);


      bool XMLStore (XNode *pParentNode, int iOptions);
      bool XMLLoad (XNode *pSecurityRangeNode, int iOptions);
      bool XMLLoadSubItems (XNode *pSecurityRangeNode, int iOptions) {return true;};

   private:

	   void SetAllowOption(int option, bool allow);

	   IPAddress lower_ip_;
	   IPAddress upper_ip_;

       // Options for this range
       long options_;

	   // the priority. 0 is highest, 10 is lowest.
	   long priority_;
      
      String name_;

      bool expires_;
      DateTime expires_Time;
   };

}
