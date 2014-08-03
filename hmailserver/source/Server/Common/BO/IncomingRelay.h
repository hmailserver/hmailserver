// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class IncomingRelay : public BusinessObject<IncomingRelay>
   {
   public:
	   IncomingRelay();
	   virtual ~IncomingRelay();

   public:
	   String GetLowerIPString() const ;
	   String GetUpperIPString()const ;
	   bool SetLowerIPString(const String & sLowerIP);
	   bool SetUpperIPString(const String & sUpperIP);

	   void SetLowerIP(const IPAddress &lNewVal){lower_ip_ = lNewVal;}
	   void SetUpperIP(const IPAddress &lNewVal){upper_ip_ = lNewVal;}
      IPAddress GetLowerIP() const {return lower_ip_;}
      IPAddress GetUpperIP() const {return upper_ip_;}

      bool IPIsWithinRange(const IPAddress &address) const ;
   
      String GetName() const  {return name_; }
      void SetName(const String &sNewVal) {name_ = sNewVal;}

      bool XMLStore (XNode *pParentNode, int iOptions);
      bool XMLLoad (XNode *pSecurityRangeNode, int iOptions);
      bool XMLLoadSubItems (XNode *pSecurityRangeNode, int iOptions) {return true;};

   protected:
	   
	   IPAddress lower_ip_;
	   IPAddress upper_ip_;
     
      String name_;

   };

}
