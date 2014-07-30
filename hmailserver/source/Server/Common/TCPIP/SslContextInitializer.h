// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SSLCertificate;

   class SslContextInitializer
   {
   public:

      static bool InitServer(boost::asio::ssl::context& context, shared_ptr<SSLCertificate> certificate, String ip_address, int port);
      static bool InitClient(boost::asio::ssl::context& context);

   private:
 
      enum WindowsCertificateType
      {
         None,
         Certificate,
         PKCS7,
         X509CRL,
      };

      static void AddCertificate_(boost::asio::ssl::context& context, bool is_revocation, DWORD encodingType, BYTE *encoding_type, int certificate_length);
      static bool LoadWindowsCA_(boost::asio::ssl::context& context);
      static std::string  GetPassword_();
      
   };
}