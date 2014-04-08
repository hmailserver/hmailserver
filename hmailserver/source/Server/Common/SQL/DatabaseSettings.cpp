// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "DatabaseSettings.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DatabaseSettings::DatabaseSettings(const String &sDatabaseServer, const String &sDatabaseName, const String &sUsername, const String &sPassword,
                                     const String &sDatabaseDirectory, const String &sDatabaseServerFailoverPartner, HM::DatabaseSettings::SQLDBType dbType, long lDBPort) :
      m_sDatabaseServer(sDatabaseServer),
      m_sDatabaseName(sDatabaseName),
      m_sUsername(sUsername),
      m_sPassword(sPassword),
      m_sDatabaseDirectory(sDatabaseDirectory),
      m_eSQLDBType(dbType),
      m_DatabaseServerFailoverPartner(sDatabaseServerFailoverPartner),
      m_lDBPort(lDBPort)
   {

   }

   DatabaseSettings::~DatabaseSettings()
   {

   }

   String 
   DatabaseSettings::GetDefaultScript()
   {
      String sFolder = IniFileSettings::Instance()->GetDBScriptDirectory();
      
      String sFile;
      switch (m_eSQLDBType)
      {
      case TypeMSSQLServer:
      case TypeMSSQLCompactEdition:
         sFile = "CreateTablesMSSQL.sql";
         break;
      case TypeMYSQLServer:
         sFile = "CreateTablesMYSQL.sql";
         break;
      case TypePGServer:
         sFile = "CreateTablesPGSQL.sql";
         break;

      }

      String sFullPath = sFolder + "\\" + sFile;

      return sFullPath;

   }

   String 
   DatabaseSettings::GetDatabaseTypeName(HM::DatabaseSettings::SQLDBType type)
   {
      switch (type)
      {
      case TypeMYSQLServer:
         return "MySQL";
      case TypeMSSQLServer:
         return "MSSQL";
      case TypePGServer:
         return "PostgreSQL";
      case TypeMSSQLCompactEdition:
         return "MSSQL Compact";
      default:
         return "Unknown";
      }
   }

}