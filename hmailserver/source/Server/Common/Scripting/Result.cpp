// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\result.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif


namespace HM
{
   Result::Result(void) :
      parameter_(0),
      value_(0)
   {
      
   }

   Result::~Result(void)
   {
   }

   long
   Result::GetValue() const 
   {
      return value_;
   }

   int
   Result::GetParameter() const 
   {
      return parameter_;
   }

   String
   Result::GetMessage() const
   {
      return message_;
   }
}