// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

#include "../Util/Event.h"

namespace HM
{
   class Task;
   class WorkQueue;

   class Thread
   {
   public:
      Thread(WorkQueue *pParentQueue);
      ~Thread(void);

      enum State
      {
         Ready = 0,
         Running = 1,
      };

      void ThreadFunc();
      void AssignTask(shared_ptr<Task> pTask);
      void CloseThread();

      State GetState() {return m_eThreadState; }
      void SetStateReady();

      HANDLE GetHandle() {return m_hThreadHandle;}

      void Start();
      void Stop(bool bBeKind);

      shared_ptr<Task> GetCurrentTask() {return m_pTask; }

   private:

      void _SetPriority();
      
      void _TerminateThread();

      HANDLE m_hThreadHandle;

      shared_ptr<Task> m_pTask;

      Event m_evtNewTask;
      Event m_evtStopRequest;

      State m_eThreadState;

      WorkQueue *m_pParentQueue;
   };

}