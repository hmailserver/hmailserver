// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SSLCertificates.h"

#include "../Persistence/PersistentSSLCertificate.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SSLCertificates::SSLCertificates()
   {
   }

   SSLCertificates::~SSLCertificates(void)
   {
   }


   void
   SSLCertificates::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads all SURBL servers from the database.
   //---------------------------------------------------------------------------()
   {
      String sSQL = "select * from hm_sslcertificates order by sslcertificatename asc";
      DBLoad_(sSQL);
   }

}