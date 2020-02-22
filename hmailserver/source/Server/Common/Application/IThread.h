#pragma once

class IThreadTask
{
public:
   IThreadTask()
   {
      stop_request_ = CreateEvent(0,TRUE, FALSE,0);
      stopped_ = CreateEvent(0,TRUE, FALSE,0);
   };

   virtual ~IThreadTask()
   {
      CloseHandle(stop_request_);
      CloseHandle(stopped_);
   }
   
   virtual void Stop()
   {
      SetEvent(stop_request_);
   }

   void SetStoppedEvent()
   {
      SetEvent(stopped_);
   }

   void WaitUntilStopped()
   {
      // Wait 10 seconds for thread to stop.
      WaitForSingleObject(stopped_, 10000);
   }

protected:

   HANDLE stop_request_;
   HANDLE stopped_;
   

private:

   
};