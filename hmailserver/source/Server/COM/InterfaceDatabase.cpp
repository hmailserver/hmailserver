// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceDatabase.h"

#include "COMError.h"

#include "../Common/SQL/SQLCEConnection.h"
#include "../Common/SQL/DatabaseSettings.h"
#include "../Common/SQL/DALConnectionFactory.h"
#include "../Common/SQL/DALConnection.h"
#include "../Common/SQL/SQLScriptParser.h"
#include "../Common/SQL/SQLScriptRunner.h"
#include "../Common/SQL/Prerequisites/PrerequisiteList.h"

#include "../Common/Util/PasswordGenerator.h"


STDMETHODIMP 
InterfaceDatabase::InterfaceSupportsErrorInfo(REFIID riid)
{
   static const IID* arr[] = 
   {
      &IID_IInterfaceDatabase,
   };

   for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
   {
      if (InlineIsEqualGUID(*arr[i],riid))
         return S_OK;
   }
   return S_FALSE;   
}

InterfaceDatabase::InterfaceDatabase()
{

}

InterfaceDatabase::~InterfaceDatabase()
{
   if (m_pConn)
   {
      RollbackTransaction();
   }
}
   
bool 
InterfaceDatabase::LoadSettings()
{
   m_pConfig = HM::Configuration::Instance();
   m_pDBManager = HM::Application::Instance()->GetDBManager();
   m_pIniFileSettings = HM::IniFileSettings::Instance();
   return true;
}

STDMETHODIMP InterfaceDatabase::get_RequiresUpgrade(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      HRESULT hRes = _EnsureDatabaseConnectivity();
      if (hRes != S_OK)
         return hRes;
      
      bool bUpgradeRequired = m_pConfig->GetRequiredDBVersion() > m_pDBManager->GetCurrentDatabaseVersion();
   
      *pVal = bUpgradeRequired ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::get_IsConnected(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      shared_ptr<HM::DatabaseConnectionManager> pDBManager = HM::Application::Instance()->GetDBManager();
      if (pDBManager)
         *pVal = m_pDBManager->GetIsConnected() ? VARIANT_TRUE : VARIANT_FALSE;
      else
         *pVal = VARIANT_FALSE;
   
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::get_DatabaseExists(VARIANT_BOOL *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      *pVal = m_pIniFileSettings->GetDatabaseSettingsExists() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::get_RequiredVersion(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      /*
         Server administration privileges should not be required for this function. Reason is that
         user should be able to determine whether a database upgrade is needed without entering his
         password. Because we don't want the user to enter his password during every installation
         unless there's changes in the database.
      */
   
      *pVal = m_pConfig->GetRequiredDBVersion();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::get_CurrentVersion(long *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      /*
         Server administration privileges should not be required for this function. Reason is that
         user should be able to determine whether a database upgrade is needed without entering his
         password. Because we don't want the user to enter his password during every installation
         unless there's changes in the database.
      */
   
      HRESULT hRes = _EnsureDatabaseConnectivity();
      if (hRes != S_OK)
         return hRes;
   
      *pVal = m_pDBManager->GetCurrentDatabaseVersion();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::get_DatabaseType(eDBtype *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      HM::DatabaseSettings::SQLDBType DBType = m_pIniFileSettings->GetDatabaseType();
      *pVal = (eDBtype) DBType;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::get_ServerName(BSTR *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      HM::String sServerName = m_pIniFileSettings->GetDatabaseServer();
   
      *pVal = sServerName.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::get_DatabaseName(BSTR *pVal)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      HM::String sServerName = m_pIniFileSettings->GetDatabaseName();
   
      *pVal = sServerName.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::BeginTransaction()
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      HRESULT hRes = _EnsureDatabaseConnectivity();
      if (hRes != S_OK)
         return hRes;
   
      HM::String sErrorMessage;
      m_pConn = m_pDBManager->BeginTransaction(sErrorMessage);
   
      if (!m_pConn)
      {
         return COMError::GenerateError(sErrorMessage);
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::CommitTransaction()
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      if (!m_pConn)
         return COMError::GenerateError("No transaction started");
   
      shared_ptr<HM::DALConnection> pTempConn = m_pConn;
      m_pConn.reset();
   
      HM::String sErrorMessage;
      if (!m_pDBManager->CommitTransaction(pTempConn, sErrorMessage))
      {
         return COMError::GenerateError(sErrorMessage);
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::RollbackTransaction()
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      if (!m_pConn)
         return COMError::GenerateError("No transaction started");
   
      shared_ptr<HM::DALConnection> pTempConn = m_pConn;
      m_pConn.reset();
   
      HM::String sErrorMessage;
      if (!m_pDBManager->RollbackTransaction(pTempConn, sErrorMessage))
      {
         return COMError::GenerateError(sErrorMessage);
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::ExecuteSQL(BSTR sSQLStatement)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      return ExecuteSQLWithReturn(sSQLStatement, 0);
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::ExecuteSQLWithReturn(BSTR sSQLStatement, long *uniqueID)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      if (uniqueID)
         *uniqueID = 0;
   
      HRESULT hRes = _EnsureDatabaseConnectivity();
      if (hRes != S_OK)
         return hRes;
   
      HM::String sStatement = sSQLStatement;
   
      HM::String sErrorMessage;
      if (m_pConn)
      {
         // Execute in the transaction we've started.
         __int64 value = 0;
         if (m_pConn->TryExecute(HM::SQLCommand(sStatement), sErrorMessage, &value, 0) == HM::DALConnection::DALSuccess)
         {
            if (uniqueID)
               *uniqueID = (long) value;
   
               return S_OK;
         }
      }
      else
      {
         __int64 value = 0;
   
         if (m_pDBManager->Execute(HM::SQLCommand(sStatement), &value, 0, sErrorMessage))
         {
            if (uniqueID)
               *uniqueID = (long) value;
   
            return S_OK;
         }
   
      }
   
      return COMError::GenerateError("Execution of SQL statement failed. Error: " + sErrorMessage);
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::ExecuteSQLScript(BSTR sFilename)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      HRESULT hRes = _EnsureDatabaseConnectivity();
      if (hRes != S_OK)
         return hRes;
   
      HM::String sErrorMessage;
      if (m_pConn)
      {
         HM::SQLScriptRunner scriptRunner;
         if (!scriptRunner.ExecuteScript(m_pConn, sFilename, sErrorMessage))
         {
            return COMError::GenerateError("Execution of SQL statements failed. Error: " + sErrorMessage);
         }
      }
      else
      {
         if (!m_pDBManager->ExecuteScript(sFilename, sErrorMessage))
         {
            return COMError::GenerateError("Execution of SQL statements failed. Error: " + sErrorMessage);
         }
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::UtilGetFileNameByMessageID(hyper lMessageID, BSTR *Output)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      HRESULT hRes = _EnsureDatabaseConnectivity();
      if (hRes != S_OK)
         return hRes;
   
      HM::String sSQL;
      sSQL.Format(_T("select messagefilename from hm_messages where messageid = %d"), lMessageID);
   
      shared_ptr<HM::DALRecordset> pRS = m_pDBManager->OpenRecordset(HM::SQLCommand(sSQL));
      if (!pRS)
         return S_OK;
   
      HM::String sRetVal = "";
      if (!pRS->IsEOF())
         sRetVal = pRS->GetStringValue("messagefilename");
   
   
      *Output = sRetVal.AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::CreateInternalDatabase()
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

   	if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      // Make sure we have the latest settings.
      m_pIniFileSettings->LoadSettings();
   
      HM::String sDirectory = m_pIniFileSettings->GetDatabaseDirectory();
      HM::String sDatabaseName = "hMailServer";
      HM::String sPassword = HM::PasswordGenerator::Generate();
   
      HM::String sErrorMessage;
      if (!HM::SQLCEConnection::CreateDatabase(sDirectory, sDatabaseName, sPassword, sErrorMessage))
         return COMError::GenerateError(sErrorMessage);
   
      HM::String sEmpty;
   
      // Create a settings object which we use to connect to the server.
      shared_ptr<HM::DatabaseSettings> pSettings = shared_ptr<HM::DatabaseSettings>(
         new HM::DatabaseSettings(sEmpty, sDatabaseName, sEmpty, sPassword, sDirectory, sEmpty, HM::DatabaseSettings::TypeMSSQLCompactEdition, 0));
   
      // Connect to the new database
      shared_ptr<HM::DALConnection> pConn = HM::DALConnectionFactory::CreateConnection(pSettings);
   
      if (pConn->Connect(sErrorMessage) != HM::DALConnection::Connected)
      {
         return COMError::GenerateError(sErrorMessage);
      }
   
      // Create the tables
      HM::SQLScriptRunner scriptRunner;
      if (!scriptRunner.ExecuteScript(pConn, pSettings->GetDefaultScript(), sErrorMessage))
      {
         return COMError::GenerateError(sErrorMessage);
      }
   
      m_pIniFileSettings->SetDatabaseDirectory(sDirectory);
      m_pIniFileSettings->SetDatabaseType(HM::DatabaseSettings::TypeMSSQLCompactEdition);
      m_pIniFileSettings->SetUsername("");
      m_pIniFileSettings->SetPassword(sPassword);
      m_pIniFileSettings->SetDatabasePort(0);
      m_pIniFileSettings->SetDatabaseServer("");
      m_pIniFileSettings->SetDatabaseName(sDatabaseName);
      m_pIniFileSettings->SetIsInternalDatabase(true);
   
      return S_OK;   
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::CreateExternalDatabase(eDBtype ServerType, BSTR ServerName, long lPort, BSTR DatabaseName, BSTR Username, BSTR Password)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      const HM::String sEmpty;
      const HM::String sServerName = ServerName;
      const HM::String sDatabaseName = DatabaseName;
      const HM::String sUsername = Username;
      const HM::String sPassword = Password;
   
      if (sDatabaseName.Find(_T(" ")) >= 0)
         return COMError::GenerateError("The database name may not contain spaces.");
   
      // Create a settings object for the connection ...
      shared_ptr<HM::DatabaseSettings> pSettings = shared_ptr<HM::DatabaseSettings>(
         new HM::DatabaseSettings(sServerName, sEmpty, sUsername, sPassword, sEmpty, sEmpty,(HM::DatabaseSettings::SQLDBType) ServerType, lPort));
   
      // Connect to the database serve   
      shared_ptr<HM::DALConnection> pConn = HM::DALConnectionFactory::CreateConnection(pSettings);
   
      HM::String sErrorMessage;
      if (pConn->Connect(sErrorMessage) != HM::DALConnection::Connected)
         return COMError::GenerateError(sErrorMessage);
   
      // Create the database
      HM::String sCreateDB = HM::SQLStatement::GetCreateDatabase(pSettings, sDatabaseName);
   
      if (!pConn->Execute(HM::SQLCommand(sCreateDB), sErrorMessage, 0, 0))
         return COMError::GenerateError(sErrorMessage);
   
      // Disconnect again.
      pConn->Disconnect();
   
      // Create a new settings object where we specify the database name as well.
      pSettings = shared_ptr<HM::DatabaseSettings>(
         new HM::DatabaseSettings(sServerName, sDatabaseName, sUsername, sPassword, sEmpty, sEmpty,(HM::DatabaseSettings::SQLDBType) ServerType, lPort));
   
      // Reconnect to the new database.
      pConn = HM::DALConnectionFactory::CreateConnection(pSettings);
      if (pConn->Connect(sErrorMessage) != HM::DALConnection::Connected)
         return COMError::GenerateError(sErrorMessage);
   
      // Run the scripts
      HM::SQLScriptRunner scriptRunner;
      if (!scriptRunner.ExecuteScript(pConn, pSettings->GetDefaultScript(), sErrorMessage))
         return COMError::GenerateError(sErrorMessage);
   
      m_pIniFileSettings->SetDatabaseDirectory("");
      m_pIniFileSettings->SetDatabaseType((HM::DatabaseSettings::SQLDBType) ServerType);
      m_pIniFileSettings->SetUsername(sUsername);
      m_pIniFileSettings->SetPassword(sPassword);
      m_pIniFileSettings->SetDatabasePort(lPort);
      m_pIniFileSettings->SetDatabaseServer(sServerName);
      m_pIniFileSettings->SetDatabaseName(sDatabaseName);
      m_pIniFileSettings->SetIsInternalDatabase(false);
   
      return S_OK;   
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceDatabase::SetDefaultDatabase(eDBtype ServerType, BSTR ServerName, long lPort, BSTR DatabaseName, BSTR Username, BSTR Password)
{
   try
   {
      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      HM::String sEmpty;
      HM::String sServerName = ServerName;
      HM::String sDatabaseName = DatabaseName;
      HM::String sUsername = Username;
      HM::String sPassword = Password;
   
      if (sDatabaseName.Find(_T(" ")) >= 0)
         return COMError::GenerateError("The database name may not contain spaces.");
   
      // Create a settings object for the connection ...
      shared_ptr<HM::DatabaseSettings> pSettings = shared_ptr<HM::DatabaseSettings>(
         new HM::DatabaseSettings(sServerName, sEmpty, sUsername, sPassword, sEmpty, sEmpty,(HM::DatabaseSettings::SQLDBType) ServerType, lPort));
   
      // Connect to the database server.
      shared_ptr<HM::DALConnection> pConn = HM::DALConnectionFactory::CreateConnection(pSettings);
   
      HM::String sErrorMessage;
      if (pConn->Connect(sErrorMessage) != HM::DALConnection::Connected)
         return COMError::GenerateError(sErrorMessage);
   
      // Disconnect again.
      pConn->Disconnect();
   
      m_pIniFileSettings->SetDatabaseDirectory("");
      m_pIniFileSettings->SetDatabaseType((HM::DatabaseSettings::SQLDBType) ServerType);
      m_pIniFileSettings->SetUsername(sUsername);
      m_pIniFileSettings->SetPassword(sPassword);
      m_pIniFileSettings->SetDatabasePort(lPort);
      m_pIniFileSettings->SetDatabaseServer(sServerName);
      m_pIniFileSettings->SetDatabaseName(sDatabaseName);
      m_pIniFileSettings->SetIsInternalDatabase(ServerType == hDBTypeMSSQLCE);
   
      return S_OK;   
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}   
   
HRESULT
InterfaceDatabase::_EnsureDatabaseConnectivity()
{
   m_pDBManager = HM::Application::Instance()->GetDBManager();
   if (!m_pDBManager)
   {
      return COMError::GenerateError("The connection to the database is not available. Please check the hMailServer error log for details.");
   }

   if (!m_pDBManager->GetIsConnected())
   {
      HM::String sError;
      sError.Format(_T("The connection to the database is not available.\r\n%s"), HM::Application::Instance()->GetLastErrorMessage());

      return COMError::GenerateError(sError);
   }

   return S_OK;
}

STDMETHODIMP InterfaceDatabase::EnsurePrerequisites(long DBVersion)
{
   try
   {
      if (!m_pConfig)
         return GetAccessDenied();

      if (!GetIsServerAdmin())
         return GetAccessDenied();
   
      HM::String sErrorMessage;
   
      if (m_pConn)
      {
         HM::PrerequisiteList prerequisites;
         if (!prerequisites.Ensure(m_pConn, DBVersion, sErrorMessage))
         {
            return COMError::GenerateError("Execution of SQL statements failed. Error: " + sErrorMessage);
         }
      }
      else
      {
         if (!m_pDBManager->EnsuresPrerequisites(DBVersion, sErrorMessage))
         {
            return COMError::GenerateError("Execution of SQL statements failed. Error: " + sErrorMessage);
         }
      }
     
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

