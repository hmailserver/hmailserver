// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCopyResult.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   IMAPCopyResult::IMAPCopyResult(Result result, const String &message) :
      IMAPResult(result, message)
   {
      
   }

   IMAPCopyResult::IMAPCopyResult() :
      IMAPResult(ResultOK, "")
   {

   }

   IMAPCopyResult::~IMAPCopyResult()
   {
      
   }

   void
   IMAPCopyResult::SetUid(unsigned int sourceUid, unsigned int targetUid)
   {
      sourceUid_ = sourceUid;
      targetUid_ = targetUid;
   }
}
