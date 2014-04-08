// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

#include "../Util/Event.h"

namespace HM
{
   class Task;
   class Thread;

   class WorkQueue
   {
   public:
      
      enum QueueType
      {
         eQTPreLoad = 0, // Pre-create a couple of threads
         eQTFixedSize = 1, // Pre-create a fixed number of threads
         eQTRandom = 2 // Don't pre-create. Create if we need threads.
      };

      WorkQueue(unsigned int iMaxSimultaneous, QueueType qtType, const String &sName);
      ~WorkQueue(void);

      void SetMaxSimultaneous(int iMaxSimultaneous);

      void AddTask(shared_ptr<Task> pTask);
      void Start();
      void ThreadFunc();
      
      bool OnThreadReady(Thread *pThread);
      void OnThreadExited(Thread *pThread);

      void Stop(bool bPreKillWarning);
      bool GetRunningThreadsExists();

      int GetQueueSize();

      void Pause();
      void Continue();
      void Clear();
      bool GetIsPaused() {return m_bPause; }

      const String &GetName() const;

      HANDLE GetQueueThreadHandle() const {return m_hThread; }
      
      shared_ptr<Task> GetTaskByName(const String &name, bool &isQueued);
      void StopTask(const String &name);

   private:

      queue<shared_ptr<Task> > m_qPendingTasks;
      CriticalSection m_csPendingTasks;

      map<HANDLE, shared_ptr<Thread> > m_mapThreads;
      CriticalSection m_csThreads;

      unsigned int m_iMaxSimultaneous;
      unsigned int m_iBaseLineThreadCount;

      Event m_hCheckForTask;
      Event m_hStopQueue;
      HANDLE m_hThread;

      String m_sQueueName;

      bool m_bPause;

      QueueType m_qtType;

   };
}