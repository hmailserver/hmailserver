// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SSLCertificate;

   class PersistentSSLCertificate
   {
   public:
      PersistentSSLCertificate(void);
      ~PersistentSSLCertificate(void);
      
      static bool DeleteObject(shared_ptr<SSLCertificate> pObject);
      static bool SaveObject(shared_ptr<SSLCertificate> pObject);
      static bool SaveObject(shared_ptr<SSLCertificate> pObject, String &errorMessage);
      static bool ReadObject(shared_ptr<SSLCertificate> pObject, shared_ptr<DALRecordset> pRS);

   };
}