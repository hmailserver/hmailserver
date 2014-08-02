// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "SpamTestData.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SpamTestData::SpamTestData(void)
   {

   }

   SpamTestData::~SpamTestData(void)
   {

   }

   void 
   SpamTestData::SetEnvelopeFrom(const String &sEnvelopeFrom)
   {
      m_sEnvelopeFrom = sEnvelopeFrom;
   }

   String 
   SpamTestData::GetEnvelopeFrom() const
   {
      return m_sEnvelopeFrom;
   }

   void 
   SpamTestData::SetHeloHost(const String &sNewVal)
   {
      m_sHeloHost = sNewVal;
   }

   String 
   SpamTestData::GetHeloHost() const
   {
      return m_sHeloHost;
   }

   void 
   SpamTestData::SetOriginatingIP(const IPAddress &address)
   {
      originatingAddress_ = address;
   }

   const IPAddress&
   SpamTestData::GetOriginatingIP() const
   {
      return originatingAddress_;
   }

   void 
   SpamTestData::SetConnectingIP(const IPAddress &iIPAddress)
   {
      connectingAddress_ = iIPAddress;
   }

   const IPAddress&
   SpamTestData::GetConnectingIP() const
   {
      return connectingAddress_;
   }

   void 
   SpamTestData::SetMessageData(shared_ptr<MessageData> pMessageData)
   {
      message_data_ = pMessageData;
   }

   shared_ptr<MessageData>
   SpamTestData::GetMessageData()  const
   {
      return message_data_;
   }

}