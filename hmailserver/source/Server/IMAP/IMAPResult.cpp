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
#ifdef _DEBUG
      if (message.EndsWith(_T("\r\n")))
      {
         // The message passed to IMAPResult should not have a trailing newline. This will be appended.
         // before the message is sent to the client.
         assert(0);
      }
#endif
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
