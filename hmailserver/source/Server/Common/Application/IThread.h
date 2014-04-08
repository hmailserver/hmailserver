#pragma once

class IThreadTask
{
public:
   IThreadTask()
   {
      m_hStopRequest = CreateEvent(0,TRUE, FALSE,0);
      m_hStopped = CreateEvent(0,TRUE, FALSE,0);
   };

   virtual ~IThreadTask()
   {
      CloseHandle(m_hStopRequest);
      CloseHandle(m_hStopped);
   }
   
   virtual void Stop()
   {
      SetEvent(m_hStopRequest);
   }

   void SetStoppedEvent()
   {
      SetEvent(m_hStopped);
   }

   void WaitUntilStopped()
   {
      // Wait 10 seconds for thread to stop.
      WaitForSingleObject(m_hStopped, 10000);
   }

protected:

   HANDLE m_hStopRequest;
   HANDLE m_hStopped;
   

private:

   
};