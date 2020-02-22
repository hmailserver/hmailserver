// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SSLCertificate;

   class SslContextInitializer
   {
   public:

      static bool InitServer(boost::asio::ssl::context& context, std::shared_ptr<SSLCertificate> certificate, String ip_address, int port);
      static bool InitClient(boost::asio::ssl::context& context);


   private:

      static void SetContextOptions_(boost::asio::ssl::context& context);
 
      static std::string  GetPassword_();

      static void SetCipherList_(boost::asio::ssl::context& context);
      static void EnableEllipticCurveCrypto_(boost::asio::ssl::context& context);
      
   };
}