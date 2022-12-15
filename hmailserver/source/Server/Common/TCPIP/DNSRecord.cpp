// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "DNSRecord.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DNSRecord::DNSRecord(AnsiString value, int recordType, int preference)
   {
      value_ = value;
      record_type_ = recordType;
      preference_ = preference;
   }
}
