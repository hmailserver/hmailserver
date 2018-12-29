/* Copyright (c) Martin Knafve / The hMailServer Community Developers (HCD) hMailServer.com */

#include <stdafx.h>
#include "DNSBlackLists.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DNSBlackLists::DNSBlackLists()
   {
   }

   DNSBlackLists::~DNSBlackLists(void)
   {
   }


   void 
   DNSBlackLists::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reloads all routes from the database.
   //---------------------------------------------------------------------------()
   {

      String sSQL = "select * from hm_dnsbl order by sblid asc";
      DBLoad_(sSQL);
   }
}