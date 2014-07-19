// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "TCPIPPort.h"
#include "SSLCertificates.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TCPIPPort::TCPIPPort(void) :
      m_iPortNumber(0),
      m_iPortProtocol(STUnknown),
      m_iSSLCertificateID(0),
      connection_security_(CSNone)
   {

   }

   TCPIPPort::~TCPIPPort(void)
   {
   }

   String 
   TCPIPPort::GetName() const
   {
      String sName;
      sName.Format(_T("%d-%d"), m_iPortProtocol, m_iPortNumber);

      return sName;
   }

   bool 
   TCPIPPort::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("TCPIPPort"));

      pNode->AppendAttr(_T("Name"), GetName());
      pNode->AppendAttr(_T("PortProtocol"), StringParser::IntToString(m_iPortProtocol));
      pNode->AppendAttr(_T("PortNumber"), StringParser::IntToString(m_iPortNumber));
      pNode->AppendAttr(_T("ConnectionSecurity"), StringParser::IntToString(connection_security_));
      pNode->AppendAttr(_T("Address"), String(address_.ToString()));
      
      if (m_iSSLCertificateID > 0)
      {
         pNode->AppendAttr(_T("SSLCertificateName"), _GetSSLCertificateName(m_iSSLCertificateID));
      }

      return true;
   }

   bool 
   TCPIPPort::XMLLoad(XNode *pNode, int iOptions)
   {
      m_iPortProtocol = (SessionType) _ttoi(pNode->GetAttrValue(_T("PortProtocol")));
      m_iPortNumber = _ttoi(pNode->GetAttrValue(_T("PortNumber")));

      address_.TryParse(pNode->GetAttrValue(_T("Address")));
      m_iSSLCertificateID  = _GetSSLCertificateID(pNode->GetAttrValue(_T("SSLCertificateName")));

      // Backwards compatibiltiy
      if (pNode->GetAttrValue(_T("UseSSL")) == _T("1"))
      {
         connection_security_ = CSSSL;
      }
      else
      {
         connection_security_ = (ConnectionSecurity) _ttoi(pNode->GetAttrValue(_T("ConnectionSecurity")));
      }

      return true;
   }

   int
   TCPIPPort::_GetSSLCertificateID(const String &sSSLCertificateName)
   {
      if (sSSLCertificateName.IsEmpty())
         return 0;

      shared_ptr<SSLCertificates> pSSLCertificates = shared_ptr<SSLCertificates>(new SSLCertificates);
      pSSLCertificates->Refresh();

      shared_ptr<SSLCertificate> pCertificate = pSSLCertificates->GetItemByName(sSSLCertificateName);
      if (pCertificate)
         return (int) pCertificate->GetID();
      else
         return 0;

   }

   String 
   TCPIPPort::_GetSSLCertificateName(__int64 iCertificateID)
   {
      shared_ptr<SSLCertificates> pSSLCertificates = shared_ptr<SSLCertificates>(new SSLCertificates);
      pSSLCertificates->Refresh();

      shared_ptr<SSLCertificate> pCertificate = pSSLCertificates->GetItemByDBID(iCertificateID);
      if (pCertificate)
         return pCertificate->GetName();
      else
         return "";

   }

   bool
   TCPIPPort::SetAddress(const String &address)
   {
      IPAddress listeningAddress;
      if (!listeningAddress.TryParse(address))
         return false;
      
      SetAddress(listeningAddress);
      return true;
   }

   void 
   TCPIPPort::SetAddress(const IPAddress &address)
   {
      address_ = address;
   }

   String
   TCPIPPort::GetAddressString() const
   {
      return address_.ToString();
   }

   IPAddress
   TCPIPPort::GetAddress() const
   {
      return address_;
   }
}