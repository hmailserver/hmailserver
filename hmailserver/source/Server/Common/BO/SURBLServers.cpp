// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SURBLServers.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SURBLServers::SURBLServers()
   {
   }

   SURBLServers::~SURBLServers(void)
   {
   }


   void 
   SURBLServers::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads all SURBL servers from the database.
   //---------------------------------------------------------------------------()
   {
      String sSQL = String("select * from hm_surblservers order by surblid asc");
      DBLoad_(sSQL);
   }
}