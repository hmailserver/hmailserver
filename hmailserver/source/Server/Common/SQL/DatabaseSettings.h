// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class DatabaseSettings  
   {
   public:
   
      enum SQLDBType
      {
         TypeUnknown      = 0,
         TypeMYSQLServer  = 1,
         TypeMSSQLServer  = 2,
         TypePGServer = 3,
         TypeMSSQLCompactEdition = 4
      };

	   DatabaseSettings(const String &sDatabaseProvider, const String &sDatabaseServer, const String &sDatabaseName, const String &sUsername, const String &sPassword,
			const String &sDatabaseDirectory, const String &sDatabaseServerFailoverPartner, HM::DatabaseSettings::SQLDBType dbType, long lDBPort);
	   virtual ~DatabaseSettings();

      String GetProvider() { return database_provider_; }
      String GetServer() {return database_server_; }
      String GetDatabaseName() {return database_name_;}
      String GetUsername() {return username_;}
      String GetPassword() {return password_;}
      String GetDatabaseDirectory() {return database_directory_;}
      HM::DatabaseSettings::SQLDBType GetType() {return sqldbtype_; }
      int GetPort() {return dbport_; }
      String GetDatabaseServerFailoverPartner() const { return database_server_failover_partner_; }
   
      String GetDefaultScript();
	  
      static String GetDatabaseTypeName(HM::DatabaseSettings::SQLDBType type);

   private:

      String database_provider_;
      String database_server_;
      String database_name_;
      String username_;
      String password_;
      String database_directory_;
      String database_server_failover_partner_;
      HM::DatabaseSettings::SQLDBType sqldbtype_;
      long dbport_;
   };

}
