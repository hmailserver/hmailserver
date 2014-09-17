// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "DALConnection.h"
#include "DatabaseSettings.h"
#include "../Cache/CacheContainer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DALConnection::DALConnection(std::shared_ptr<DatabaseSettings> pDatabaseSettings) :
      try_count_(6),
      database_settings_(pDatabaseSettings)
   {

   }

   DALConnection::~DALConnection()
   {

   }

   bool 
   DALConnection::Execute(const SQLCommand &command, String &sErrorMessage, __int64 *iInsertID, int iIgnoreErrors) 
   {
      SQLCommand commandCopy = command;

      if (!GetSupportsCommandParameters() && command.GetParameters().size() > 0)
      {
         SQLStatement statement;
         String nonParameterQueryString = statement.GenerateFromCommand(command);

         commandCopy.ClearParameters();
         commandCopy.SetQueryString(nonParameterQueryString);
      }

      int iNoOfTries = 6;
      for (int i = 1 ; i <= try_count_; i++)
      {
         if (i == 2 || i == 4)
         {
            // Try to reconnect
            Reconnect(sErrorMessage);
         }

         DALConnection::ExecutionResult result = TryExecute(commandCopy, sErrorMessage, iInsertID, iIgnoreErrors);
         if (result == DALConnection::DALSuccess)
         {
            sErrorMessage = "";           
            return true;
         }

         if (result != DALConnection::DALConnectionProblem)
            break;

         // There's been a connection problem. Try to reconnect.
         Sleep(1000);
         Reconnect(sErrorMessage);
      }

      // We've reached the maximum number of 
      // reties without success. Time to give up.
      ErrorManager::Instance()->ReportError(ErrorManager::High, 5032, "DALConnection::Execute", sErrorMessage);

      return false;
   }

   bool 
   DALConnection::Reconnect(String &sErrorMessage)
   {
#ifndef _ERROR_LOGGING_IN_MESSAGE_BOXES
      LOG_APPLICATION("DALConnection - Reconnecting to the database");
#endif

      // Disconnect first...
      Disconnect();
      
      // ... and now try to reconnect.
      if (Connect(sErrorMessage) != Connected)
         return false;   

      CacheContainer::Instance()->Clear();

      return true;
   }



}