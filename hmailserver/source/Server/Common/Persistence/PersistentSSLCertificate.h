// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SSLCertificate;
   enum  PersistenceMode;

   class PersistentSSLCertificate
   {
   public:
      PersistentSSLCertificate(void);
      ~PersistentSSLCertificate(void);
      
      static bool DeleteObject(shared_ptr<SSLCertificate> pObject);
      static bool SaveObject(shared_ptr<SSLCertificate> pObject);
      static bool SaveObject(shared_ptr<SSLCertificate> pObject, String &errorMessage,  PersistenceMode mode);
      static bool ReadObject(shared_ptr<SSLCertificate> pObject, shared_ptr<DALRecordset> pRS);

   };
}