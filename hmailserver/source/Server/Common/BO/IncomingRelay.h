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

	   void SetLowerIP(const IPAddress &lNewVal){_lowerIP = lNewVal;}
	   void SetUpperIP(const IPAddress &lNewVal){_upperIP = lNewVal;}
      IPAddress GetLowerIP() const {return _lowerIP;}
      IPAddress GetUpperIP() const {return _upperIP;}

      bool IPIsWithinRange(const IPAddress &address) const ;
   
      String GetName() const  {return _name; }
      void SetName(const String &sNewVal) {_name = sNewVal;}

      bool XMLStore (XNode *pParentNode, int iOptions);
      bool XMLLoad (XNode *pSecurityRangeNode, int iOptions);
      bool XMLLoadSubItems (XNode *pSecurityRangeNode, int iOptions) {return true;};

   protected:
	   
	   IPAddress _lowerIP;
	   IPAddress _upperIP;
     
      String _name;

   };

}
