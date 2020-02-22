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

      SessionType GetProtocol() const  {return port_protocol_; }
      void SetProtocol(SessionType iProtcol) {port_protocol_ = iProtcol;}

      int GetPortNumber() const  {return port_number_; }
      void SetPortNumber(int iPortNumber) {port_number_ = iPortNumber;}

      bool SetAddress(const String &address);
      void SetAddress(const IPAddress &address);
      String GetAddressString() const;
      IPAddress GetAddress() const;

      __int64 GetSSLCertificateID() const  {return sslcertificate_id_; }
      void SetSSLCertificateID(int iSSLCertificateID) {sslcertificate_id_ = iSSLCertificateID;}

      ConnectionSecurity GetConnectionSecurity() const  {return connection_security_; }
      void SetConnectionSecurity(ConnectionSecurity connection_security) {connection_security_ = connection_security;}

      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems (XNode *pNode, int iOptions) {return true;};

   private:

      int GetSSLCertificateID_(const String &sSSLCertificateName);
      String GetSSLCertificateName_(__int64 iCertificateID);

      SessionType port_protocol_;
      int port_number_;
      int sslcertificate_id_;

      ConnectionSecurity connection_security_;

      IPAddress address_;
   };
}