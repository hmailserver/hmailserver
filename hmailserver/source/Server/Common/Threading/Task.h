// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

namespace HM
{
   class Task
   {
   public:
      Task(const String &name);
      ~Task(void);

      void Run();

      String GetName() const {return name_;  }

      Event& GetIsStartedEvent() {return is_started_;}

   protected:

      void SetIsStarted() {is_started_.Set();}
      virtual void DoWork() = 0;

   private:
      
      Event is_started_;

      String name_;
   };
}