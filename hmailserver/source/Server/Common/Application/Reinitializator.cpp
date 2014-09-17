// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// This class is a bit whacky. The single purpose of it is to
// reintialize the entire server. Sometimes, for example when
// doing a restore, this needs to be done asynchroniously.

#include "StdAfx.h"
#include ".\reinitializator.h"

#include <boost/thread/thread.hpp>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   Reinitializator::Reinitializator(void) :
      is_running_(false)
   {
   }

   Reinitializator::~Reinitializator(void)
   {

   }

   void 
   Reinitializator::ReInitialize()
   {
      if (is_running_)
      {
         return;
      }

      std::function<void ()> func = std::bind( &Reinitializator::WorkerFunc, this );

      worker_thread_ = boost::thread(func);
   }

   void
   Reinitializator::WorkerFunc()
   {
      is_running_ = true;

      Application::Instance()->Reinitialize();

      is_running_ = false;

   }
}