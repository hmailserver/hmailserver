// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../TCPIP/IPAddress.h"

namespace HM
{
   class MessageData;
   class IPAddress;

   class SpamTestData
   {
   public:
      
      SpamTestData();
      virtual ~SpamTestData();

      void SetEnvelopeFrom(const String &sEnvelopeFrom);
      String GetEnvelopeFrom() const;

      void SetHeloHost(const String &sHeloHost);
      String GetHeloHost() const;

      void SetOriginatingIP(const IPAddress &address);
      const IPAddress &GetOriginatingIP() const;

      void SetConnectingIP(const IPAddress &address);
      const IPAddress &GetConnectingIP() const;


      void SetMessageData(const shared_ptr<MessageData> pMessageData);
      shared_ptr<MessageData> GetMessageData() const;

   private:

      String m_sEnvelopeFrom;

      IPAddress _originatingAddress;
      IPAddress _connectingAddress;
      String m_sHeloHost;

      shared_ptr<MessageData> m_pMessageData;
      
   };

}