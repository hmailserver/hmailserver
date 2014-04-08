// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "./RuleResult.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{ 
   RuleResult::RuleResult(void) :
      m_iSendUsingRoute(0),
      m_bDeleteEmail(false)
   {
   }

   RuleResult::~RuleResult(void)
   {
   }

}