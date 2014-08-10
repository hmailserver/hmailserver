// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

namespace HM
{
   class CertificateVerifier
   {
   public:

      /// The type of the function object's result.
      typedef bool result_type;

      /// Constructor.
      CertificateVerifier(const String &host_name);

      /// Perform certificate verification.
      bool operator()(bool preverified, boost::asio::ssl::verify_context& ctx) const;

   private:

      bool VerifyCertificate_( PCCERT_CONTEXT certificate, LPWSTR serverName, int &windows_error_code) const;

      // The host name to be checked.
      String host_name_;
   };
}