// Copyright (c) 2008 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2008-08-12

#include "StdAfx.h"
#include "IOOperation.h"
#include "../Util/ByteBuffer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IOOperation::IOOperation(OperationType type, shared_ptr<ByteBuffer> buffer) :
      _type(type),
      _buffer(buffer)
   {
      
   }

   IOOperation::IOOperation(OperationType type, const AnsiString &string) :
      _type(type),
      _string(string)
   {
      
   }

   IOOperation::~IOOperation(void)
   {

   }

   
}