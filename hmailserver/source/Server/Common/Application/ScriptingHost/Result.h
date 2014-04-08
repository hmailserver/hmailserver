// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


namespace HM
{
   class Result 
   {
   public:
      Result(void);
      ~Result(void);

      void SetValue(long lNewVal){m_lValue = lNewVal; }
      long GetValue() const; 

      void SetParameter(int lNewVal){_parameter = lNewVal; }
      int GetParameter() const; 


      void SetMessage(const String& sValue){m_sMessage = sValue; }
      String GetMessage() const;

   private:
      long m_lValue;
      int _parameter;

      String m_sMessage;
   };
}