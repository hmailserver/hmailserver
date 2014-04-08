// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Event
   {
   public:
      Event(void);
      Event(const Event& p);
      ~Event(void);
   
      void Wait();
      void Set();
      bool IsSet();
      void Reset();

      HANDLE GetHandle() const;

   private:

      bool _closeWhenWaited;
      HANDLE m_hEvent;
   };
}