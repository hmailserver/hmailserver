// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "FileInfo.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   FileInfo::FileInfo(const String &name, const DateTime &created) :
      name_(name),
      created_(created)

   {
      
   }

   FileInfo::FileInfo()
   {

   }

   

}