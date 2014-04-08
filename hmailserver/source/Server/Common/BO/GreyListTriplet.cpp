// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "GreyListTriplet.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   GreyListTriplet::GreyListTriplet(void) :
      m_iID(0),
      m_iPassedCount(0),
      m_iBlockedCount(0)
   {

   }

   GreyListTriplet::~GreyListTriplet(void)
   {
   }

   void 
   GreyListTriplet::SetID(__int64 iNewID)
   {
      m_iID = iNewID;
   }

   __int64 
   GreyListTriplet::GetID() const
   {
      return m_iID;
   }

   void 
   GreyListTriplet::SetCreateTime(const String &sNewVal)
   {
      m_sCreateTime = sNewVal; 
   }

   String
   GreyListTriplet::GetCreateTime() const
   {
      return m_sCreateTime;
   }

   void 
   GreyListTriplet::SetBlockEndTime(const String &sNewVal)
   {
      m_sBlockEndTime = sNewVal; 
   }

   String
   GreyListTriplet::GetBlockEndTime() const
   {
      return m_sBlockEndTime;
   }

   void 
   GreyListTriplet::SetDeleteTime(const String &sNewVal)
   {
      m_sDeleteTime = sNewVal; 
   }

   String
   GreyListTriplet::GetDeleteTime() const
   {
      return m_sDeleteTime;
   }

   void 
   GreyListTriplet::SetIPAddress(IPAddress iNewVal)
   {
      m_iIPAddress = iNewVal;
   }
   
   IPAddress 
   GreyListTriplet::GetIPAddress() const
   {
      return m_iIPAddress;
   }

   void 
   GreyListTriplet::SetRecipientAddress(const String &sNewVal)
   {
      m_sRecipientAddress = sNewVal; 
   }

   String
   GreyListTriplet::GetRecipientAddress() const
   {
      return m_sRecipientAddress;
   }

   void 
   GreyListTriplet::SetSenderAddress(const String &sNewVal)
   {
      m_sSenderAddress = sNewVal; 
   }

   String
   GreyListTriplet::GetSenderAddress() const
   {
      return m_sSenderAddress;
   }


   void 
   GreyListTriplet::SetPassedCount(int iNewVal)
   {
      m_iPassedCount = iNewVal;
   }

   int 
   GreyListTriplet::GetPassedCount() const
   {
      return m_iPassedCount;
   }

   void 
   GreyListTriplet::SetBlockedCount(int iNewVal)
   {
      m_iPassedCount = iNewVal;
   }

   int 
   GreyListTriplet::GetBlockedCount() const
   {
      return m_iPassedCount;
   }

}