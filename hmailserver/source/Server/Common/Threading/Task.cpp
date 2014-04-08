// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21
#include "StdAfx.h"
#include ".\task.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Task::Task(void)
   {
      
   }

   Task::Task(const String &name) :
      _name (name)
   {

   }

   Task::~Task(void)
   {

   }

}