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

      IPAddress GetLowerIPAddress() const {return m_iWhiteLowerIPAddress; }
      String GetLowerIPAddressString() const {return m_iWhiteLowerIPAddress.ToString(); }
      void SetLowerIPAddress(const IPAddress &iIPAddress) {m_iWhiteLowerIPAddress = iIPAddress;}
      bool SetLowerIPAddress(const String &iIPAddress);

      IPAddress GetUpperIPAddress() const {return m_iWhiteUpperIPAddress; }
      String GetUpperIPAddressString() const {return m_iWhiteUpperIPAddress.ToString(); }
      void SetUpperIPAddress(const IPAddress &iIPAddress) {m_iWhiteUpperIPAddress = iIPAddress;}
      bool SetUpperIPAddress(const String &iIPAddress);

      String GetEmailAddress() const {return m_sWhiteEmailAddress; }
      void SetEMailAddress(const String &sEmail) {m_sWhiteEmailAddress = sEmail; }

      String GetDescription() const {return m_sWhiteDescription; }
      void SetDescription(const String &sDescription) {m_sWhiteDescription = sDescription; }

      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems (XNode *pNode, int iOptions) {return true;};

   private:

      IPAddress m_iWhiteLowerIPAddress;
      IPAddress m_iWhiteUpperIPAddress;

      

      String m_sWhiteEmailAddress;
      String m_sWhiteDescription;
     
      
   };
}