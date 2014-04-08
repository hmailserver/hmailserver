// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class GreyListingWhiteAddress : public BusinessObject<GreyListingWhiteAddress>
   {
   public:
      GreyListingWhiteAddress(void);
      ~GreyListingWhiteAddress(void);

      String GetName() const;

      String GetIPAddress() const;
      void SetIPAddress(const String &sIPAddress);

      String GetUserEditableIPAddress() const;
      void SetUserEditableIPAddress(const String &sIPAddress);

      String GetDescription() const {return m_sWhiteIPDescription; }
      void SetDescription(const String &sDescription) {m_sWhiteIPDescription = sDescription; }

      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems (XNode *pNode, int iOptions) {return true; }

   private:

      String m_sWhiteIPAddress;
      String m_sWhiteIPDescription;
      
   };
}