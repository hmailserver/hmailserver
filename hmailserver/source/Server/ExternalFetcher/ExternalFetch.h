// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace boost
{
   namespace system
   {
      class error_code;
   }
}
namespace HM
{
   class FetchAccount;
   class ClientInfo;

   class ExternalFetch
   {
   public:
      ExternalFetch(void);
      ~ExternalFetch(void);

      void Start(shared_ptr<FetchAccount> pFA);

   private:

      void _ReportError(boost::system::error_code &errorCode);
   };
}