// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

namespace HM
{
   class Task
   {
   public:
      Task(void);
      Task(const String &name);
      ~Task(void);

      virtual void DoWork() = 0;

      virtual void StopWork() = 0;

      String GetName() const {return _name;  }
   protected:

      
   private:

      String _name;
   };
}