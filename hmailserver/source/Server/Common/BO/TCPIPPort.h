// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../TCPIP/SocketConstants.h"

namespace HM
{
   class TCPIPPort : public BusinessObject<TCPIPPort>
   {
   public:
      TCPIPPort(void);
      ~TCPIPPort(void);

      String GetName() const;

      SessionType GetProtocol() const  {return m_iPortProtocol; }
      void SetProtocol(SessionType iProtcol) {m_iPortProtocol = iProtcol;}

      int GetPortNumber() const  {return m_iPortNumber; }
      void SetPortNumber(int iPortNumber) {m_iPortNumber = iPortNumber;}

      bool SetAddress(const String &address);
      void SetAddress(const IPAddress &address);
      String GetAddressString() const;
      IPAddress GetAddress() const;

      __int64 GetSSLCertificateID() const  {return m_iSSLCertificateID; }
      void SetSSLCertificateID(int iSSLCertificateID) {m_iSSLCertificateID = iSSLCertificateID;}

      ConnectionSecurity GetConnectionSecurity() const  {return connection_security_; }
      void SetConnectionSecurity(ConnectionSecurity connection_security) {connection_security_ = connection_security;}

      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems (XNode *pNode, int iOptions) {return true;};

   private:

      int _GetSSLCertificateID(const String &sSSLCertificateName);
      String _GetSSLCertificateName(__int64 iCertificateID);

      SessionType m_iPortProtocol;
      int m_iPortNumber;
      int m_iSSLCertificateID;

      ConnectionSecurity connection_security_;

      IPAddress address_;
   };
}