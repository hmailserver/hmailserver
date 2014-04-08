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
      m_bUseSSL(false),
      m_iPortNumber(0),
      m_iPortProtocol(STUnknown),
      m_iSSLCertificateID(0)
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
      pNode->AppendAttr(_T("UseSSL"), m_bUseSSL ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("Address"), String(_address.ToString()));
      
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
      m_bUseSSL = pNode->GetAttrValue(_T("UseSSL")) == _T("1");
      _address.TryParse(pNode->GetAttrValue(_T("Address")));
      m_iSSLCertificateID  = _GetSSLCertificateID(pNode->GetAttrValue(_T("SSLCertificateName")));

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
      _address = address;
   }

   String
   TCPIPPort::GetAddressString() const
   {
      return _address.ToString();
   }

   IPAddress
   TCPIPPort::GetAddress() const
   {
      return _address;
   }
}