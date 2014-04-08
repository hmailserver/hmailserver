// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "SQLCommand.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   SQLCommand::SQLCommand()
   {

   }

   SQLCommand::SQLCommand(const String &queryString) :
      _queryString(queryString)
   {
      
   }

   void
   SQLCommand::AddParameter(const AnsiString &name, const int &value)
   {
      SQLParameter parameter(name, value);

      _parameters.push_back(parameter);
   }

   void
   SQLCommand::AddParameter(const AnsiString &name, const __int64 &value)
   {
      SQLParameter parameter(name, value);

      _parameters.push_back(parameter);
   }


   void
   SQLCommand::AddParameter(const AnsiString &name, const String& value)
   {
      SQLParameter parameter(name, value);

      _parameters.push_back(parameter);
   }

   void
   SQLCommand::AddParameter(const AnsiString &name, unsigned int value)
   {
      SQLParameter parameter(name, value);

      _parameters.push_back(parameter);
   }


   String 
   SQLCommand::GetQueryString() const
   {
      return _queryString;
   }

   void
   SQLCommand::SetQueryString(const String &value)
   {
      _queryString = value;
   }


   const list<SQLParameter> &
   SQLCommand::GetParameters() const
   {
      return _parameters;
   }

   void 
   SQLCommand::ClearParameters()
   {
      _parameters.clear();
   }
}
