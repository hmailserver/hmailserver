// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#include "StdAfx.h"

#include "WorkQueue.h"
#include "Task.h"



#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#endif

namespace HM
{


   WorkQueue::WorkQueue(unsigned int iMaxSimultaneous, const String &sQueueName) :
      m_sQueueName (sQueueName),
      m_iMaxSimultaneous(0),
      work_( io_service_)
   {
      SetMaxSimultaneous(iMaxSimultaneous);

      LOG_DEBUG(Formatter::Format("Creating work queue {0}", m_sQueueName));
   }

   void 
   WorkQueue::SetMaxSimultaneous(int iMaxSimultaneous)
   {
      m_iMaxSimultaneous = iMaxSimultaneous;

      // Hard code limit to 100. Everything over this won't be good for stability.
      if (m_iMaxSimultaneous > 100)
         m_iMaxSimultaneous = 100;
   }

   WorkQueue::~WorkQueue(void)
   {
     LOG_DEBUG(Formatter::Format("Destructing work queue {0}", m_sQueueName));
   }

   void 
   WorkQueue::AddTask(shared_ptr<Task> pTask)
   {
      LOG_DEBUG(Formatter::Format("Adding task {0} to work queue {1}", pTask->GetName(), m_sQueueName));

      // Post a wrapped task into the queue.
      boost::function<void ()> func = boost::bind(&WorkQueue::ExecuteTask, this, pTask);
      io_service_.post( func );
   }

   void 
   WorkQueue::ExecuteTask(shared_ptr<Task> pTask)
   {
      {
         boost::lock_guard<boost::recursive_mutex> guard(runningTasksMutex_);
         runningTasks_.insert(pTask);
      }

      LOG_DEBUG(Formatter::Format("Executing task {0} in work queue {1}", pTask->GetName(), m_sQueueName));

      pTask->DoWork();

      {
         boost::lock_guard<boost::recursive_mutex> guard(runningTasksMutex_);
         set<shared_ptr<Task>>::iterator iter = runningTasks_.find(pTask);
         runningTasks_.erase(pTask);
      }
   }


   void 
   WorkQueue::Start()
   {
      LOG_DEBUG(Formatter::Format("Starting work queue {0}", m_sQueueName));

      //io_service_.reset();

      for ( std::size_t i = 0; i < m_iMaxSimultaneous; ++i )
      {
         shared_ptr<boost::thread> thread = shared_ptr<boost::thread>
            (new boost::thread(boost::bind( &WorkQueue::IoServiceRunWorker, this )));

         workerThreads_.insert(thread);
      }

      LOG_DEBUG(Formatter::Format("Started work queue {0}", m_sQueueName));
   }

   void 
   WorkQueue::IoServiceRunWorker()
   {
      LOG_DEBUG(Formatter::Format("Running worker in work queue {0}", m_sQueueName));
      io_service_.run();
      LOG_DEBUG(Formatter::Format("Worker exited in work queue {0}", m_sQueueName));
   }

   void 
   WorkQueue::Stop()
   {
      LOG_DEBUG(Formatter::Format("Stopping working queue {0}.", m_sQueueName));

      // Prevent new tasks from being started.
      io_service_.stop();

      LOG_DEBUG(Formatter::Format("Interupt and join threads in working queue {0}", m_sQueueName));

      set<shared_ptr<boost::thread>> completedThreads;

      int attemptCount = 10000 / 250; // 10 seconds, 250 ms between each

      for (int i = 0; i < attemptCount; i++)
      {
         boost_foreach (shared_ptr<boost::thread> thread, workerThreads_)
         {
            thread->interrupt();
         }

         boost_foreach (shared_ptr<boost::thread> thread, workerThreads_)
         {
            if (thread->timed_join(boost::posix_time::milliseconds(1)))
            {
               completedThreads.insert(thread);
            }
         }

         boost_foreach (shared_ptr<boost::thread> thread, completedThreads)
         {
            set<shared_ptr<boost::thread>>::iterator iter = workerThreads_.find(thread);

            workerThreads_.erase(iter);
         }

         completedThreads.clear();

         if (workerThreads_.size() == 0)
         {
            LOG_DEBUG(Formatter::Format("All threads are joined in queue {0}.", m_sQueueName));
            return;
         }

         

         boost::lock_guard<boost::recursive_mutex> guard(runningTasksMutex_);
         set<shared_ptr<Task>>::iterator iter = runningTasks_.begin();
         if (iter != runningTasks_.end())
         {
            LOG_DEBUG(Formatter::Format("Still {0} remaining threads in queue {1}. First task: {2}", workerThreads_.size(), m_sQueueName, (*iter)->GetName()));
         }
         else
         {
            LOG_DEBUG(Formatter::Format("Still {0} remaining threads in queue {1}. First task: <Unknown>", workerThreads_.size(), m_sQueueName));

         }

         Sleep(250);
      }

      LOG_DEBUG(Formatter::Format("Given up waiting for threads to join in queue {0}.", m_sQueueName));
   }

   const String&
   WorkQueue::GetName() const
   //---------------------------------------------------------------------------
   // DESCRIPTION:
   // Returns the name of this queue.
   //---------------------------------------------------------------------------
   {
      return m_sQueueName;
   }
}
