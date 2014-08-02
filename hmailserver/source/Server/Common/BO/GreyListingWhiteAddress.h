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

      String GetDescription() const {return white_ipdescription_; }
      void SetDescription(const String &sDescription) {white_ipdescription_ = sDescription; }

      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems (XNode *pNode, int iOptions) {return true; }

   private:

      String white_ipaddress_;
      String white_ipdescription_;
      
   };
}