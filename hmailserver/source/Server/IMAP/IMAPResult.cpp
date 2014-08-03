// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPResult.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   IMAPResult::IMAPResult(Result result, const String &message) :
      result_(result),
      message_(message)
   {
      
   }

   IMAPResult::IMAPResult() :
      result_(ResultOK),
      message_("")
   {

   }

   IMAPResult::IMAPResult(const IMAPResult &otherCopy)
   {
      result_ = otherCopy.GetResult();
      message_ = otherCopy.GetMessage();
   }

   IMAPResult& 
   IMAPResult::operator= (const IMAPResult&otherCopy)
   {
      result_ = otherCopy.GetResult();
      message_ = otherCopy.GetMessage();

      return *this;
   }


   IMAPResult::~IMAPResult()
   {
      
   }
}
