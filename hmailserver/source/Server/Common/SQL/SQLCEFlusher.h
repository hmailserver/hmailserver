// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include <boost/thread.hpp>

namespace HM
{
   class DatabaseSettings;
   class SQLCEConnection;

   // Forces SQL Server Compact to write committed changes to disk once a second.
   // Its own flush may never happen (KB2979868), so changes are lost if the server stops abnormally.
   class SQLCEFlusher
   {
   public:
      SQLCEFlusher(std::shared_ptr<DatabaseSettings> settings);
      ~SQLCEFlusher();

      bool Start();
      void Stop();

   private:

      void WorkerFunc_();

      std::shared_ptr<SQLCEConnection> connection_;
      boost::thread worker_thread_;
   };
}
