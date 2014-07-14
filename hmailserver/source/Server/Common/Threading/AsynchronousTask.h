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
      AsynchronousTask(boost::function<void()> functionToRun, shared_ptr<T> parentHolder) :
         Task("AsynchronousTask"),
         _asynchronousFunction(functionToRun),
         _parentHolder(parentHolder)
      {

      }

      virtual void DoWork()
      {
         try
         {
            _asynchronousFunction();
         }
         catch (...)
         {
            // to be sure we release our pointer to the parent TCP connection below.
         }

         // Reset the shared_ptr to the parent object.
         _parentHolder.reset();
      }

   private:

      boost::function<void()> _asynchronousFunction;

      shared_ptr<T> _parentHolder;
   };
}