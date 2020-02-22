// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM   
{
   enum PersistenceMode
   {
      PersistenceModeNormal = 0,
      PersistenceModeRestore = 1,
      PersistenceModeRename = 2,
   };
   
}