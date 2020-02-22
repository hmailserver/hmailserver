// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"
#include "SSLCertificate.h"

#include "../Persistence/PersistentSSLCertificate.h"

namespace HM
{
   class SSLCertificates : public Collection<SSLCertificate, PersistentSSLCertificate>
   {
   public:
      SSLCertificates();
      ~SSLCertificates(void);

      // Refreshes this collection from the database.
      void Refresh();

   protected:
      virtual String GetCollectionName() const {return "SSLCertificates"; }

   private:
     
   };
}