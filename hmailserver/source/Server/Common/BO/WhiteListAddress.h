// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class WhiteListAddress : public BusinessObject<WhiteListAddress>
   {
   public:
      WhiteListAddress(void);
      ~WhiteListAddress(void);


      String GetName() const;

      IPAddress GetLowerIPAddress() const {return white_lower_ipaddress_; }
      String GetLowerIPAddressString() const {return white_lower_ipaddress_.ToString(); }
      void SetLowerIPAddress(const IPAddress &iIPAddress) {white_lower_ipaddress_ = iIPAddress;}
      bool SetLowerIPAddress(const String &iIPAddress);

      IPAddress GetUpperIPAddress() const {return white_upper_ipaddress_; }
      String GetUpperIPAddressString() const {return white_upper_ipaddress_.ToString(); }
      void SetUpperIPAddress(const IPAddress &iIPAddress) {white_upper_ipaddress_ = iIPAddress;}
      bool SetUpperIPAddress(const String &iIPAddress);

      String GetEmailAddress() const {return white_email_address_; }
      void SetEMailAddress(const String &sEmail) {white_email_address_ = sEmail; }

      String GetDescription() const {return white_description_; }
      void SetDescription(const String &sDescription) {white_description_ = sDescription; }

      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems (XNode *pNode, int iOptions) {return true;};

   private:

      IPAddress white_lower_ipaddress_;
      IPAddress white_upper_ipaddress_;

      

      String white_email_address_;
      String white_description_;
     
      
   };
}