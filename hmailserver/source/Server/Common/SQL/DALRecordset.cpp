// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "DALRecordset.h"
#include "DALConnection.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DALRecordset::DALRecordset()
   {

   }

   DALRecordset::~DALRecordset()
   {

   }


   bool
   DALRecordset::Open(std::shared_ptr<DALConnection> pConn, const SQLCommand &command)
   {
      String sErrMsg;

      SQLCommand commandCopy = command;

      if (!pConn->GetSupportsCommandParameters() && command.GetParameters().size() > 0)
      {
         SQLStatement statement;
         String nonParameterQueryString = statement.GenerateFromCommand(command);

         commandCopy.ClearParameters();
         commandCopy.SetQueryString(nonParameterQueryString);
      }

      int iNoOfTries = 6;
      for (int i = 1 ; i <= iNoOfTries; i++)
      {
         DALConnection::ExecutionResult result = TryOpen(pConn, commandCopy, sErrMsg);
         if (result == DALConnection::DALSuccess)
            return true;

         if (result != DALConnection::DALConnectionProblem)
            break;

         // There's been a connection problem. Try to reconnect.
         Sleep(1000);
         pConn->Reconnect(sErrMsg);
      }

      // We've reached the maximum number of 
      // reties without success. Time to give up.
      ErrorManager::Instance()->ReportError(ErrorManager::High, 5033, "DALRecordset::Open", sErrMsg);

      return false;
   }

   /// Should be called if a client is trying to access a record set value
   /// when we are EOF the record set. Will report an error to the error log
   void 
   DALRecordset::ReportEOFError_(const AnsiString &FieldName) const
   {
      String message;
      message.Format(_T("Trying to access data in recordset which was EOF. Column: %s."), String(FieldName).c_str());

      ErrorManager::Instance()->ReportError(ErrorManager::High, 4207, "DALRecordset::ReportEOFError_", message);
   }
}
