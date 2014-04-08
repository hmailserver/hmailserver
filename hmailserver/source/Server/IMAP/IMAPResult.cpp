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
      _result(result),
      _message(message)
   {
      
   }

   IMAPResult::IMAPResult() :
      _result(ResultOK),
      _message("")
   {

   }

   IMAPResult::IMAPResult(const IMAPResult &otherCopy)
   {
      _result = otherCopy.GetResult();
      _message = otherCopy.GetMessage();
   }

   IMAPResult& 
   IMAPResult::operator= (const IMAPResult&otherCopy)
   {
      _result = otherCopy.GetResult();
      _message = otherCopy.GetMessage();

      return *this;
   }


   IMAPResult::~IMAPResult()
   {
      
   }
}
