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

      virtual void DoWork() = 0;

      String GetName() const {return _name;  }

      Event& GetIsStartedEvent() {return _isStarted;}

   protected:

      void SetIsStarted() {_isStarted.Set();}

   private:
      
      Event _isStarted;

      String _name;
   };
}