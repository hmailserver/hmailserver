// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Groups.h"

#include "../Persistence/PersistentGroup.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Groups::Groups()
   {
      
   }

   Groups::~Groups(void)
   {
   }

   void 
   Groups::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads all groups form the database
   //---------------------------------------------------------------------------()
   {
      String sSQL = "select * from hm_groups order by groupname asc";

      DBLoad_(sSQL);
   }
}