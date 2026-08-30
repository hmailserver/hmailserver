// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "DMARCTxtLookup.h"

#include "../../TCPIP/DNSResolver.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool
   DMARCDnsTxtLookup::GetTXTRecords(const String &domain, std::vector<String> &records)
   {
      DNSResolver resolver;
      return resolver.GetTXTRecords(domain, records);
   }
}
