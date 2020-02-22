// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Task.h"

namespace HM
{
   template <class T>
   class AsynchronousTask : public Task
   {
   public:
      AsynchronousTask(std::function<void()> functionToRun, std::shared_ptr<T> parentHolder) :
         Task("AsynchronousTask"),
         asynchronousFunction_(functionToRun),
         parentHolder_(parentHolder)
      {

      }

      virtual void DoWork()
      {
         try
         {
            asynchronousFunction_();
         }
         catch (...)
         {
            // to be sure we release our pointer to the parent TCP connection below.
         }

         // Reset the shared_ptr to the parent object.
         parentHolder_.reset();
      }

   private:

      std::function<void()> asynchronousFunction_;

      std::shared_ptr<T> parentHolder_;
   };
}