// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "Event.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Event::Event(void) :
      is_set_(false)
   {

   }

   Event::~Event(void)
   {
      
   }

   void 
   Event::Wait()
   {
      boost::mutex::scoped_lock lock(mutex_);

      if (!is_set_)
         set_condition_.wait(lock, boost::bind(&Event::is_set_, this));

      is_set_ = false;
   }

   bool 
   Event::WaitFor(boost::chrono::milliseconds milliseconds)
   {
      boost::mutex::scoped_lock lock(mutex_);

      if (is_set_)
      {
         is_set_ = false;
         return true;
      }
      else
      {
         // result will be false if there's a timeout.
         bool result = set_condition_.wait_for(lock, milliseconds, boost::bind(&Event::is_set_, this));
         is_set_ = false;
         return result;
      }
     
   }

   void 
   Event::Set()
   {  
      boost::mutex::scoped_lock lock(mutex_);
      is_set_ = true;
      set_condition_.notify_one();
   }


}