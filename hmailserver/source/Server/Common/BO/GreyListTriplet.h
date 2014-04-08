// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class GreyListTriplet
   {
   public:
      GreyListTriplet();
      ~GreyListTriplet(void);

      void SetID(__int64 iNewID);
      __int64 GetID() const;

      void SetCreateTime(const String &sNewVal);
      String GetCreateTime() const;

      void SetBlockEndTime(const String &sNewVal);
      String GetBlockEndTime() const;

      void SetDeleteTime(const String &sNewVal);
      String GetDeleteTime() const;

      void SetIPAddress(IPAddress iNewVal);
      IPAddress GetIPAddress() const;
      
      void SetSenderAddress(const String &sNewVal);
      String GetSenderAddress() const;

      void SetRecipientAddress(const String &sNewVal);
      String GetRecipientAddress() const;

      void SetPassedCount(int iNewVal);
      int GetPassedCount() const;

      void SetBlockedCount(int iNewVal);
      int GetBlockedCount() const;

   private:

      __int64 m_iID;

      String m_sCreateTime;
      String m_sBlockEndTime;
      String m_sDeleteTime;

      IPAddress m_iIPAddress;
      String m_sSenderAddress;
      String m_sRecipientAddress;

      int m_iPassedCount;
      int m_iBlockedCount;

   };
}