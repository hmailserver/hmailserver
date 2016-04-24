// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "GreyListingWhiteAddresses.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   GreyListingWhiteAddresses::GreyListingWhiteAddresses()
   {
   }

   GreyListingWhiteAddresses::~GreyListingWhiteAddresses(void)
   {
   }


   void 
   GreyListingWhiteAddresses::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads all SURBL servers from the database.
   //---------------------------------------------------------------------------()
   {
      String sSQL = "select * from hm_greylisting_whiteaddresses order by whiteipaddress asc";
      DBLoad_(sSQL);
   }


}