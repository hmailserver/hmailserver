// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SSLCertificate : public BusinessObject<SSLCertificate>
   {
   public:
      SSLCertificate(void);
      ~SSLCertificate(void);

      String GetName() const {return m_sName; }
      void SetName(const String &sName) {m_sName = sName; }

      String GetCertificateFile() const {return m_sCertificateFile; }
      void SetCertificateFile(const String &sName) {m_sCertificateFile = sName; }

      String GetPrivateKeyFile() const {return m_sPrivateKeyFile; }
      void SetPrivateKeyFile(const String &sName) {m_sPrivateKeyFile = sName; }

      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems (XNode *pNode, int iOptions) {return true;};

   private:

      String m_sName;
      String m_sCertificateFile;
      String m_sPrivateKeyFile;

      
   };
}