// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "SslContextInitializer.h"

#include "../BO/SSLCertificate.h"
#include "../Util/Encoding/Base64.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   const int CertificateAlreadyInStore = 185057381;

   bool
   SslContextInitializer::InitServer(boost::asio::ssl::context& context, std::shared_ptr<SSLCertificate> certificate, String ip_address, int port)
   {  
      if (!certificate)
      {
         String errorMessage = Formatter::Format("Error initializing SSL. Certificate not set. Address: {0}, Port: {1}", ip_address, port);
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5113, "SslContextInitializer::InitServer", errorMessage);
         return false;
      }

      try
      {
         context.set_options(boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_sslv2);
      }
      catch (boost::system::system_error ec)
      {
         String asioError = ec.what();

         String errorMessage;
         errorMessage.Format(_T("Failed to set SSL context options. Address: %s, Port: %i, Error: %s"), 
            String(ip_address).c_str(), port, asioError.c_str());

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5113, "SslContextInitializer::InitServer", errorMessage);

         return false;

      }

      AnsiString certificateFile = certificate->GetCertificateFile();
      AnsiString privateKeyFile = certificate->GetPrivateKeyFile();


      try
      {
         context.use_certificate_file(certificateFile, boost::asio::ssl::context::pem);
      }
      catch (boost::system::system_error ec)
      {
         String asioError = ec.what();

         String errorMessage;
         errorMessage.Format(_T("Failed to load certificate file. Path: %s, Address: %s, Port: %i, Error: %s"), 
            String(certificateFile).c_str(), ip_address.c_str(), port, asioError.c_str());

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5113, "SslContextInitializer::InitServer", errorMessage);

         return false;
      }

      try
      {
         context.use_certificate_chain_file(certificateFile);
      }
      catch (boost::system::system_error ec)
      {
         String asioError = ec.what();

         String errorMessage;
         errorMessage.Format(_T("Failed to load certificate chain from certificate file. Path: %s, Address: %s, Port: %i, Error: %s"), 
            String(certificateFile), ip_address.c_str(), port, asioError.c_str());

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5113, "SslContextInitializer::InitServer", errorMessage);

         return false;
      }

      try
      {
         context.set_password_callback(std::bind(&SslContextInitializer::GetPassword_));
         context.use_private_key_file(privateKeyFile, boost::asio::ssl::context::pem);
      }
      catch (boost::system::system_error ec)
      {
         String asioError = ec.what();

         String errorMessage;
         errorMessage.Format(_T("Failed to load private key file. Path: %s, Address: %s, Port: %i, Error: %s"), 
            String(privateKeyFile), ip_address.c_str(), port, asioError.c_str());

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5113, "SslContextInitializer::InitServer", errorMessage);

         return false;
      }
      catch (...)
      {
         String errorMessage = "Error initializing SSL";
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5113, "SslContextInitializer::InitServer", errorMessage);
         return false;
      }

      SetCipherList_(context);

      return true;
   }

   bool
   SslContextInitializer::InitClient(boost::asio::ssl::context& context)
   { 
      boost::system::error_code errorCode;
      context.set_options(boost::asio::ssl::context::default_workarounds |
         boost::asio::ssl::context::no_sslv2);

      if (errorCode.value() != 0)
      {
         String errorMessage;
         errorMessage.Format(_T("Failed to set default workarounds."));

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5144, "SslContextInitializer::InitClient", errorMessage, errorCode);

         return false;
      }

      SetCipherList_(context);

      return true;
   }

   std::string 
   SslContextInitializer::GetPassword_()
   {
      ErrorManager::Instance()->ReportError(ErrorManager::High, 5143, "TCPServer::GetPassword()", "The private key file has a password. hMailServer does not support this.");
      return "";
   }

   void 
   SslContextInitializer::SetCipherList_(boost::asio::ssl::context& context)
   {
      AnsiString cipher_list = Configuration::Instance()->GetSslCipherList();

      cipher_list.Replace("\r", "");
      cipher_list.Replace("\n", "");
      cipher_list.Replace(" ", "");

      if (cipher_list.Trim().IsEmpty())
         return;

      // Asio does not expose cipher list. Access underlaying layer (OpenSSL) directly.
      SSL_CTX* ssl = context.native_handle();
      int result = SSL_CTX_set_cipher_list(ssl, cipher_list.c_str());

      if (result == 0)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5511,"SslContextInitializer::SetCipherList_", "Failed to set SSL ciphers");
     
      }
   }

}