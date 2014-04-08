// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "ServerMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ServerMessage::ServerMessage(void)
   {

   }

   ServerMessage::~ServerMessage(void)
   {
   }

   bool 
   ServerMessage::XMLStore(XNode *pParentNode)
   {

      return true;
   }

   bool 
   ServerMessage::XMLLoad(XNode *pNode)
   {

      return true;
   }
}