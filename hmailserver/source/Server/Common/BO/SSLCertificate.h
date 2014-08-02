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

      String GetName() const {return name_; }
      void SetName(const String &sName) {name_ = sName; }

      String GetCertificateFile() const {return certificate_file_; }
      void SetCertificateFile(const String &sName) {certificate_file_ = sName; }

      String GetPrivateKeyFile() const {return private_key_file_; }
      void SetPrivateKeyFile(const String &sName) {private_key_file_ = sName; }

      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems (XNode *pNode, int iOptions) {return true;};

   private:

      String name_;
      String certificate_file_;
      String private_key_file_;

      
   };
}