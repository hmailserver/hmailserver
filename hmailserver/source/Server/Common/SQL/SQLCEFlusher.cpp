// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SQLCEFlusher.h"
#include "SQLCEConnection.h"
#include "SQLCommand.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SQLCEFlusher::SQLCEFlusher(std::shared_ptr<DatabaseSettings> settings) :
      connection_(std::make_shared<SQLCEConnection>(settings))
   {
   }

   SQLCEFlusher::~SQLCEFlusher()
   {
      Stop();
   }

   bool
   SQLCEFlusher::Start()
   {
      String errorMessage;
      if (connection_->Connect(errorMessage) != DALConnection::Connected)
         return false;

      // Commits on this connection flush everything committed so far, by all connections.
      if (!connection_->EnableFlushOnCommit())
      {
         connection_->Disconnect();
         return false;
      }

      worker_thread_ = boost::thread(std::bind(&SQLCEFlusher::WorkerFunc_, this));
      return true;
   }

   void
   SQLCEFlusher::Stop()
   {
      if (worker_thread_.joinable())
      {
         worker_thread_.interrupt();
         worker_thread_.join();
      }

      connection_->Disconnect();
   }

   void
   SQLCEFlusher::WorkerFunc_()
   {
      SQLCommand command("UPDATE hm_dbversion SET value = value");
      bool errorReported = false;

      try
      {
         while (true)
         {
            boost::this_thread::sleep_for(boost::chrono::seconds(1));

            String errorMessage;
            if (connection_->TryExecute(command, errorMessage) != DALConnection::DALSuccess && !errorReported)
            {
               ErrorManager::Instance()->ReportError(ErrorManager::High, 5705, "SQLCEFlusher::WorkerFunc_", errorMessage);
               errorReported = true;
            }
         }
      }
      catch (const boost::thread_interrupted&)
      {
      }
   }
}
