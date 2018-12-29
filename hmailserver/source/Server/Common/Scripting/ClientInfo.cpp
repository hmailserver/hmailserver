/* Copyright (c) Martin Knafve / The hMailServer Community Developers (HCD) hMailServer.com */

#include <stdafx.h>
#include "ClientInfo.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ClientInfo::ClientInfo() :
      port_(0)
   {

   }

   ClientInfo::~ClientInfo()
   {

   }


}
