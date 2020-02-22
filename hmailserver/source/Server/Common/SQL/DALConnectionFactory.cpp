// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "DALConnectionFactory.h"
#include "ADOConnection.h"
#include "MySQLConnection.h"
#include "PGConnection.h"
#include "SQLCEConnection.h"
#include "DatabaseSettings.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   DALConnectionFactory::DALConnectionFactory()
   {

   }

   DALConnectionFactory::~DALConnectionFactory()
   {

   }

   std::shared_ptr<DALConnection>
   DALConnectionFactory::CreateConnection(std::shared_ptr<DatabaseSettings> pSettings)
   {
      std::shared_ptr<DALConnection> pConn;
      
      HM::DatabaseSettings::SQLDBType t = pSettings->GetType();

     switch (t)
      {
      case HM::DatabaseSettings::TypeMSSQLServer:
         pConn = std::shared_ptr<ADOConnection>(new ADOConnection(pSettings));
         break;
      case HM::DatabaseSettings::TypeMYSQLServer:
         pConn = std::shared_ptr<MySQLConnection>(new MySQLConnection(pSettings));
         break;
      case HM::DatabaseSettings::TypePGServer:
         pConn = std::shared_ptr<PGConnection>(new PGConnection(pSettings));
         break;
      case HM::DatabaseSettings::TypeMSSQLCompactEdition:
         pConn = std::shared_ptr<SQLCEConnection>(new SQLCEConnection(pSettings));
         break;
      }
   
      return pConn;
   }
}