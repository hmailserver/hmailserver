// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "GUIDCreator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   GUIDCreator::GUIDCreator()
   {

   }

   GUIDCreator::~GUIDCreator()
   {

   }

   String
   GUIDCreator::GetGUID()
   {
      GUID uuid;
      CoCreateGuid(&uuid);

      wchar_t szGUID[39];
      StringFromGUID2 (uuid, szGUID, 39);

      return szGUID;
   }
}
