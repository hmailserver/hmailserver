// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "ADOConnection.h"
#include "ADORecordset.h"
#include "SQLCommand.h"
#include "DatabaseSettings.h"
#include "Macros/MSSQLMacroExpander.h"
#include "ADOInt64Helper.h"
#include "../Util/Registry.h"

using namespace std;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ADOConnection::ADOConnection(std::shared_ptr<DatabaseSettings> pSettings) :
      DALConnection(pSettings)
   {
      HRESULT hr =cADOConnection.CreateInstance(__uuidof(Connection));
      
      cADOConnection->PutCursorLocation(adUseClient);

      if ( !SUCCEEDED( hr ) )
      {
         assert(0);
      }  

      bConnected = false;
   }


   ADOConnection::~ADOConnection()
   {
   
      try
      {
         cADOConnection = NULL;
      }
      catch(...)
      {
      
      } 
   
   
   }


   bool
   ADOConnection::Disconnect()
   {

      try
      {

         if (!bConnected)
            return false; // --- already disconnected.

         if (cADOConnection->State == 1)
            cADOConnection->Close();
      }
      catch ( ... )
      {
         assert(0);
      }

      bConnected = false;

      return true;
   }


   DALConnection::ConnectionResult
   ADOConnection::Connect(String &sErrorMessage)
   {
      String sUsername = database_settings_->GetUsername();
      String sPassword = database_settings_->GetPassword();
      String sServer = database_settings_->GetServer();
      String sDatabase = database_settings_->GetDatabaseName();
      String sServerFailoverPartner = database_settings_->GetDatabaseServerFailoverPartner();

      String sProvider = database_settings_->GetProvider();

      if (sProvider.IsEmpty())
      {
         if (sServerFailoverPartner.IsEmpty())
         {
            // If MSOLEDBSQL v18 or later is installed, prefer that one. It supports TLS 1.2.
            if (IsMSOLEDBSQL18OrLaterInstalled_())
            {
               sProvider = "MSOLEDBSQL";
            }
            else
            {
               sProvider = "sqloledb";
            }
         }
         else
         {
            sProvider = "SQLNCLI";
         }
      }

      if (bConnected)
      {
         assert(0); // --- Already connected!
         return Connected;
      }

      String sConnectionString = "Provider=" + sProvider + ";";
      sConnectionString.append("Server=" + sServer + ";");
      sConnectionString.append("Initial Catalog=" + sDatabase + ";");

      if (sUsername.IsEmpty())
      {
         // Windows authentication.
         sConnectionString.append(_T("Integrated Security=SSPI;"));
      }
      else
      {
         // User/password authentication
         sConnectionString.append(_T("User ID=" + sUsername +";"));
         sConnectionString.append(_T("Password=" + sPassword + ";"));
      }

      if (!sServerFailoverPartner.IsEmpty())
      {
         sConnectionString.append("FailoverPartner=" + sServerFailoverPartner + ";");
      }

      BSTR bsConnection = sConnectionString.AllocSysString();
      BSTR bsUser = sUsername.AllocSysString();
      BSTR bsPassword = sPassword.AllocSysString();
  
      try
      {
         HRESULT hr = cADOConnection->Open( bsConnection, bsUser, bsPassword, NULL );
         cADOConnection->PutCursorLocation(adUseClient);
      
         if ( !SUCCEEDED( hr ) )
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5027, "ADOConnection::Connect", "Open failed");
            return TemporaryFailure;
         }  

      }
      catch ( _com_error &err )
      {
         _bstr_t bstrSource( err.Source() );
         _bstr_t bstrDescription( err.Description() );
     
         LPCSTR lpcSource = bstrSource;
         String sErrSource = lpcSource;

         LPCSTR lpcDesc = bstrDescription;
         String sErrDesc = lpcDesc;

         sErrorMessage = "ADO: " + sErrDesc;
         
         String sMessage = "Error when connecting to database. " + sErrSource + " " + sErrDesc + " Check your database settings in hMailServer.ini.";

         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5028, "ADOConnection::Connect", sMessage);

         SysFreeString(bsConnection);
         SysFreeString(bsUser);
         SysFreeString(bsPassword);

         return TemporaryFailure;
      

      }
      catch( ... )
      {
         cADOConnection = 0;
     
         sErrorMessage = "ADO: Unknown database connection error";

         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5029, "ADOConnection::Connect", "An unknown error occurred when connecting to database. Check your database settings in hMailServer.ini.");

         SysFreeString(bsConnection);
         SysFreeString(bsUser);
         SysFreeString(bsPassword);

         return TemporaryFailure;

      }

      SysFreeString(bsConnection);
      SysFreeString(bsUser);
      SysFreeString(bsPassword);

      bConnected = true;

      // Change the format of dates. We always insert dates using the format yyyy-mm-dd.
      // We need to inform MSSQL about this.
      String sSetDateFormat = "SET DATEFORMAT ymd";
      String sErrorMsg;

      SQLCommand command1(sSetDateFormat);
      TryExecute(command1, sErrorMsg, 0, 0);

      // Set transaction isolation level to read uncommitted. This may give phantom
      // reads but that isn't a problem since we've implemented our own locking 
      // mechanism.
      String sIsolationLevel = "SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED";
      
      sErrorMsg = "";

      SQLCommand command2(sIsolationLevel);
      TryExecute(command2, sErrorMsg, 0, 0);

      return Connected;
   }

   _ConnectionPtr
   ADOConnection::GetConnection() const
   {
      return cADOConnection;
   }

   DALConnection::ExecutionResult
   ADOConnection::TryExecute(const SQLCommand &command, String &sErrorMessage, __int64 *iInsertID, int iIgnoreErrors)
   {
      if (!bConnected)
         return DALConnectionProblem; // --- already disconnected.

      String queryString = command.GetQueryString();

      try
      {
         // Execute the SQL statement.
         _CommandPtr adoCommand;
         adoCommand.CreateInstance(__uuidof(Command));
         InitializeCommandParameters(adoCommand, command, queryString);

         adoCommand->CommandType = adCmdText;
         adoCommand->CommandText = _bstr_t(queryString);
         adoCommand->ActiveConnection = cADOConnection;
         adoCommand->Execute(NULL, NULL, adCmdText);

         // Check what unique ID we got back (if we're interested).
         _RecordsetPtr pIdentityRS;
         BSTR bsIdentity;
         if (iInsertID > 0)
         {
            pIdentityRS.CreateInstance(__uuidof(Recordset));
            pIdentityRS->PutCursorLocation(adUseClient);
            pIdentityRS->PutRefActiveConnection(cADOConnection); 
            String sIdentitySQL = "SELECT @@IDENTITY AS IDENT";
            bsIdentity = sIdentitySQL.AllocSysString();

            HRESULT hr = pIdentityRS->Open( bsIdentity, vtMissing, adOpenKeyset, adLockOptimistic , adCmdText);
            pIdentityRS->PutRefActiveConnection(NULL); 
            
         }

         if (iInsertID)
         {
            *iInsertID = GetIdentityFromRS_(pIdentityRS);
            ::SysFreeString( bsIdentity );
         }
      }
      catch ( _com_error &err )
      {
         ExecutionResult dbErr = GetErrorType(_com_error::WCodeToHRESULT(err.WCode()));
         if (iIgnoreErrors > 0 && iIgnoreErrors & dbErr)
            return DALConnection::DALSuccess;

         if (queryString.Find(_T("[IGNORE-ERRORS]")) >= 0)
            return DALConnection::DALSuccess;

         _bstr_t bstrSource( err.Source() );
         _bstr_t bstrDescription( err.Description() );

         LPCSTR lpcSource = bstrSource;
         String sErrSource = lpcSource;

         LPCSTR lpcDesc = bstrDescription;
         String sErrDesc = lpcDesc;

         sErrorMessage = "Source: ADOConnection::Execute(), Code: HM10044, Description: Error while executing SQL statement: \n";
         sErrorMessage += queryString;
         sErrorMessage += "\n";
         sErrorMessage += sErrSource;
         sErrorMessage += "\n";
         sErrorMessage += sErrDesc;

         return dbErr;
      }
      catch ( ... )
      {

         sErrorMessage = "Source: ADOConnection::Execute(), Code: HM10045, Description: Unknown error when executing SQL statement: \n";
         sErrorMessage.append(queryString);

         return DALUnknown;
      }

      return DALSuccess;
   }

   bool
   ADOConnection::IsConnected() const
   { 
      return bConnected; 
   };

   __int64 
   ADOConnection::GetIdentityFromRS_(_RecordsetPtr pRS) const
   {
      try
      {
         if (pRS->GetRecordCount() == 0)
            return 0;
         
         _variant_t    vaField;
         FieldsPtr      pFields;
         FieldPtr       pField;
         HRESULT			hr;

         hr = pRS->get_Fields( &pFields );
         vaField = "IDENT";
         pFields->get_Item( vaField, &pField );

         vaField.Clear();
         if (pField)
         {
            pField->get_Value(&vaField);

            switch (vaField.vt)
            {
            case VT_NULL:
               return 0;
            case VT_I4:
               return vaField.intVal;
            default:
               if (vaField.decVal.sign == DECIMAL_NEG)
                  return -vaField.cyVal.int64;
               else
                  return vaField.cyVal.int64;
            }
         }

         return 0;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5030, "ADOConnection::GetIdentityFromRS_", "Error while determening @@IDENTITY");
      }

      return 0;
   }

   void 
   ADOConnection::OnConnected()
   {
      // Nothing to do here. Only used my MySQL.
   }

   DALConnection::ExecutionResult 
   ADOConnection::GetErrorType(int iErrorCode)
   {
      switch (iErrorCode)
      {
      case 0:
         return DALSuccess;
      case 0x80040E2FL: // DB_E_INTEGRITYVIOLATION:
         return DALErrorInSQL;
      case 0x80040200L:
         return DALConnectionProblem;
      default:
         return DALUnknown;
      }
   }

   bool 
   ADOConnection::BeginTransaction(String &sErrorMessage)
   {
      return TryExecute(SQLCommand("BEGIN TRANSACTION"), sErrorMessage, 0, 0) == DALSuccess;
   }

   bool 
   ADOConnection::CommitTransaction(String &sErrorMessage)
   {
      return TryExecute(SQLCommand("COMMIT TRANSACTION"), sErrorMessage, 0, 0) == DALSuccess;
   }

   bool 
   ADOConnection::RollbackTransaction(String &sErrorMessage)
   {
      return TryExecute(SQLCommand("ROLLBACK TRANSACTION"), sErrorMessage, 0, 0) == DALSuccess;
   }

   void 
   ADOConnection::SetTimeout(int seconds)
   {
      cADOConnection->PutCommandTimeout(seconds);
   }

   bool 
   ADOConnection::CheckServerVersion(String &errorMessage)
   {
      std::shared_ptr<ADOConnection> connection = shared_from_this();

      ADORecordset recordset;
      if (recordset.TryOpen(connection, SQLCommand("SELECT SERVERPROPERTY('productversion') as ProductVersion"), errorMessage) != DALConnection::DALSuccess)
         return false;

      String version = recordset.GetStringValue("ProductVersion");

      std::vector<String> versionVector = StringParser::SplitString(version, ".");
      int majorVersion = _ttoi(versionVector[0]);
      
      if (majorVersion < RequiredVersion)
      {
         errorMessage = "hMailServer requires Microsoft SQL Server 2000 or newer.";       
         return false;
      }

      return true;
   }

   std::shared_ptr<DALRecordset> 
   ADOConnection::CreateRecordset()
   {
      std::shared_ptr<ADORecordset> recordset = std::shared_ptr<ADORecordset>(new ADORecordset());
      return recordset;
   }

   void
   ADOConnection::EscapeString(String &sInput)
   {
      sInput.Replace(_T("'"), _T("''"));
   }

   std::shared_ptr<IMacroExpander> 
   ADOConnection::CreateMacroExpander()
   {
      std::shared_ptr<MSSQLMacroExpander> expander = std::shared_ptr<MSSQLMacroExpander>(new MSSQLMacroExpander());
      return expander;
   }

   void
   ADOConnection::InitializeCommandParameters(_CommandPtr &adoCommand, const SQLCommand &sqlCommand, String &queryString) const
   {
      for(const SQLParameter &parameter : sqlCommand.GetParameters())
      {
         String parameterName = parameter.GetName();

         if (parameter.GetType() == SQLParameter::ParamTypeInt32)
         {
            VARIANT integerType;
            integerType.vt = VT_I4;
            integerType.intVal = parameter.GetInt32Value();

            adoCommand->Parameters->Append(adoCommand->CreateParameter(_bstr_t(),adInteger,adParamInput, 4, integerType));
         }
         else if (parameter.GetType() == SQLParameter::ParamTypeInt64)
         {
            // Windows 2000 backwards compatibility:
            ADO64Helper::AddInt64Parameter(adoCommand, parameterName, parameter.GetInt64Value());
         }
         else if (parameter.GetType() == SQLParameter::ParamTypeUnsignedInt32)
         {
            VARIANT integerType;
            integerType.vt = VT_UI4;
            integerType.intVal = parameter.GetUnsignedInt32Value();

            adoCommand->Parameters->Append(adoCommand->CreateParameter(_bstr_t(),adInteger,adParamInput, 4, integerType));
         }
         else if (parameter.GetType() == SQLParameter::ParamTypeString)
         {
            const String &value = parameter.GetStringValue();

            VARIANT stringType;
            stringType.vt = VT_BSTR;
            stringType.bstrVal  = _bstr_t(value);

            int length = 0;

            if (value.GetLength() < 4000)
               length = 4000;
            else
               length = value.GetLength();   

            if (!value.IsEmpty())
               adoCommand->Parameters->Append(adoCommand->CreateParameter(_bstr_t(), adVarWChar, adParamInput, length, stringType));
            else
               adoCommand->Parameters->Append(adoCommand->CreateParameter(_bstr_t(), adVarWChar, adParamInput, 1, stringType));
         }

         queryString.Replace(parameterName, _T("?"));
      }
   }

   bool 
   ADOConnection::IsMSOLEDBSQL18OrLaterInstalled_() const
   {
      Registry registry;
      String value;
      if (!registry.GetStringValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\MSOLEDBSQL", "InstalledVersion", value))
         return false;

      auto versionInfo = StringParser::SplitString(value, ".");

      if (versionInfo.size() != 4)
      {
         // Unsupported format
         return false;
      }

      auto majorVersionStr = versionInfo[0];
      if (!StringParser::IsNumeric(majorVersionStr))
      {
         // Unsupported format
         return false;
      }

      int majorVersion = 0;
      AnsiString str = majorVersionStr;
      if (!StringParser::TryParseInt(str, majorVersion))
      {
         // Unsupported format
         return false;
      }

      return majorVersion >= 18;
   }
}

