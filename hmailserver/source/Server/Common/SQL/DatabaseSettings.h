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

	   DatabaseSettings(const String &sDatabaseServer, const String &sDatabaseName, const String &sUsername, const String &sPassword,
                       const String &sDatabaseDirectory, const String &sDatabaseServerFailoverPartner, HM::DatabaseSettings::SQLDBType dbType, long lDBPort);
	   virtual ~DatabaseSettings();

      String GetServer() {return m_sDatabaseServer; }
      String GetDatabaseName() {return m_sDatabaseName;}
      String GetUsername() {return m_sUsername;}
      String GetPassword() {return m_sPassword;}
      String GetDatabaseDirectory() {return m_sDatabaseDirectory;}
      HM::DatabaseSettings::SQLDBType GetType() {return m_eSQLDBType; }
      int GetPort() {return m_lDBPort; }
      String GetDatabaseServerFailoverPartner() const { return m_DatabaseServerFailoverPartner; }
   
      String GetDefaultScript();

      static String GetDatabaseTypeName(HM::DatabaseSettings::SQLDBType type);

   private:

      String m_sDatabaseServer;
      String m_sDatabaseName;
      String m_sUsername;
      String m_sPassword;
      String m_sDatabaseDirectory;
      String m_DatabaseServerFailoverPartner;
      HM::DatabaseSettings::SQLDBType m_eSQLDBType;
      long m_lDBPort;


   };

}
