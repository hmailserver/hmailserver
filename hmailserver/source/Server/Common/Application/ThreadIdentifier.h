   // ThreadIdentifier.h: interface for the ThreadIdentifier class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class IThreadTask;

namespace HM
{
   class ThreadIdentifier  
   {
   public:
	   ThreadIdentifier();
	   virtual ~ThreadIdentifier();

      HANDLE GetThreadHandle() const;
      void SetThreadHandle(HANDLE h);

      int GetThreadID() const;
      
      void SetThread(shared_ptr<IThreadTask> pThread);
      shared_ptr<IThreadTask> GetThread() const;

      void Run();

      void WaitForStart();
      void Close();
      void Stop();
      

   private:
      HANDLE m_hThreadHandle;
      int m_iThreadID;
      shared_ptr<IThreadTask> m_pThread;
      HANDLE m_StartEvent;
   };

   struct ThreadIdentifierContainer
   {
      shared_ptr<ThreadIdentifier> pThreadIdentifier;
   };
}
