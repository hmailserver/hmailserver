// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SSLCertificate.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SSLCertificate::SSLCertificate(void)
   {
      
   }

   SSLCertificate::~SSLCertificate(void)
   {
   }

   bool 
   SSLCertificate::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("SSLCertificate"));

      pNode->AppendAttr(_T("Name"), name_);
      pNode->AppendAttr(_T("CertificateFile"), certificate_file_);
      pNode->AppendAttr(_T("PrivateKeyFile"), private_key_file_);

      return true;
   }

   bool 
   SSLCertificate::XMLLoad(XNode *pNode, int iOptions)
   {
      name_ = pNode->GetAttrValue(_T("Name"));
      certificate_file_ = pNode->GetAttrValue(_T("CertificateFile"));
      private_key_file_ = pNode->GetAttrValue(_T("PrivateKeyFile"));

      return true;
   }
}