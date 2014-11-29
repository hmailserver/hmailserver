#include "stdafx.h"
#include "SQLCERecordset.h"
#include "SQLCEConnection.h"
#include "../Util/VariantDateTime.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SQLCERecordset::SQLCERecordset()
   {
	   /*HRESULT hr = cSQLCERecordset.CreateInstance(__uuidof(Recordset));
      if (!SUCCEEDED(hr))
      {
         assert(0);
      }
*/
      cur_row_ = 0;
   }

   SQLCERecordset::~SQLCERecordset()
   {
      try
      {
         Close_();
      }
      catch (...)
      {

      }
   }

   DALConnection::ExecutionResult
   SQLCERecordset::TryOpen(std::shared_ptr<DALConnection> pDALConn, const SQLCommand &command, String &sErrorMessage)
   {  
      cur_row_ = 0;

      String sSQL = command.GetQueryString();

      std::shared_ptr<SQLCEConnection> pConn = std::static_pointer_cast<SQLCEConnection>(pDALConn);

      _ConnectionPtr pADOConnection = pConn->GetConnection();

      if (!pConn->IsConnected())
         return DALConnection::DALConnectionProblem; // --- disconnected.

      if (pADOConnection == NULL)
         return DALConnection::DALConnectionProblem; // connection to server lost?

      String queryString = command.GetQueryString();
   
      try
      {
         _CommandPtr adoCommand;
         adoCommand.CreateInstance(__uuidof(Command));

         pConn->InitializeCommandParameters(adoCommand, command, queryString);

         adoCommand->CommandType = adCmdText;
         adoCommand->CommandText = _bstr_t(queryString);
         adoCommand->ActiveConnection = pADOConnection;
         adoCommand->NamedParameters = true;

         cSQLCERecordset = adoCommand->Execute(NULL, NULL, adCmdText);
         adoCommand->ActiveConnection = NULL;

         return DALConnection::DALSuccess;
      }
      catch ( _com_error &err)
      {        
         _bstr_t bstrSource( err.Source() );
         _bstr_t bstrDescription( err.Description() );

         LPCSTR lpcSource = bstrSource;
         String sErrSource = lpcSource;

         LPCSTR lpcDesc = bstrDescription;
         String sErrDesc = lpcDesc;

         sErrorMessage = "Source: SQLCEConnection::Execute, Code: HM10046, Description: Unknown error when executing SQL statement. \n";
         sErrorMessage += sSQL;
         sErrorMessage += "\n";
         sErrorMessage += sErrSource;
         sErrorMessage += "\n";
         sErrorMessage += sErrDesc;

         return DALConnection::DALUnknown;
      }
      catch (...)
      {
         return DALConnection::DALUnknown;
      }

      return DALConnection::DALSuccess;
   }

   bool
   SQLCERecordset::Close_()
   {
      try
      {
         HRESULT hr = cSQLCERecordset->Close();
      }
      catch (...)
      {
         return false;
      }
   
      return true;
   }

   long
   SQLCERecordset::RecordCount() const
   {
      return 0; // This method is not supported!
   }

   bool
   SQLCERecordset::IsEOF() const
   {
      if (cSQLCERecordset->GetadoEOF() == VARIANT_TRUE)
         return true;
      else
         return false;
   }

   bool
   SQLCERecordset::MoveNext()
   {
      cur_row_++;
      cSQLCERecordset->MoveNext();
      return true;
   }


   String
   SQLCERecordset::GetStringValue(const AnsiString &FieldName) const
   {
      if (IsEOF())
      {
         ReportEOFError_(FieldName);
         return "";
      }

      _variant_t    vaField;
	   FieldsPtr      pFields;
	   FieldPtr       pField;
      HRESULT			hr;

      try
      {
         hr = cSQLCERecordset->get_Fields( &pFields );
	      vaField = FieldName;
	      pFields->get_Item( vaField, &pField );

         vaField.Clear();
         if (pField)
         {
            pField->get_Value(&vaField);
         }
      }
      catch ( _com_error &err)
      {
         _bstr_t bstrSource( err.Source() );
         _bstr_t bstrDescription( err.Description() );
         assert(0);
         return "";
      }

      String sValue;
      if (vaField.vt == VT_DATE)
      {
         DateTime vtDate = vaField.date;
         sValue.Format(_T("%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d"), 
               vtDate.GetYear(), vtDate.GetMonth(), vtDate.GetDay(), vtDate.GetHour(), vtDate.GetMinute(), vtDate.GetSecond());;
      }
      else if (vaField.vt == VT_BSTR)
      {
         sValue = vaField.bstrVal;
      }

      return sValue;
   }

   long
   SQLCERecordset::GetLongValue(const AnsiString &FieldName) const
   {
      if (IsEOF())
      {
         ReportEOFError_(FieldName);
         return 0;
      }

      _variant_t    vaField;
	   FieldsPtr      pFields;
	   FieldPtr       pField;
      HRESULT			hr;

	   hr = cSQLCERecordset->get_Fields( &pFields );
	   vaField = FieldName;
	   pFields->get_Item( vaField, &pField );

      vaField.Clear();
      if (pField)
      {
         pField->get_Value(&vaField);

         if (vaField.vt == VT_NULL)
            return 0;
      }
      
      return vaField.lVal;
   }

   __int64
   SQLCERecordset::GetInt64Value(const AnsiString &FieldName) const
   {
      if (IsEOF())
      {
         ReportEOFError_(FieldName);
         return 0;
      }

      _variant_t     vaField;
	   FieldsPtr      pFields;
	   FieldPtr       pField;
      HRESULT			hr;

	   hr = cSQLCERecordset->get_Fields( &pFields );
	   vaField = FieldName;
	   pFields->get_Item( vaField, &pField );

      vaField.Clear();
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

   double
   SQLCERecordset::GetDoubleValue(const AnsiString &FieldName) const
   {
      if (IsEOF())
      {
         ReportEOFError_(FieldName);
         return 0;
      }

      _variant_t     vaField;
	   FieldsPtr      pFields;
	   FieldPtr       pField;
      HRESULT			hr;

	   hr = cSQLCERecordset->get_Fields( &pFields );
	   vaField = FieldName;
	   pFields->get_Item( vaField, &pField );

      vaField.Clear();
      pField->get_Value(&vaField);

      if (vaField.vt == VT_NULL)
         return 0;

      return vaField.dblVal;
   }

   std::vector<AnsiString> 
   SQLCERecordset::GetColumnNames() const
   {
      std::vector<AnsiString> result;

      FieldsPtr      pFields;
      FieldPtr       pField;
      HRESULT			hr;

      hr = cSQLCERecordset->get_Fields( &pFields );

      for (int i = 0; i < pFields->get_Count(0); i++)
      {
         _variant_t vaField = i;
         pFields->get_Item( vaField, &pField );

         BSTR s;
         pField->get_Name(&s);

         AnsiString fieldName = s;

         result.push_back(fieldName);
      }

      return result;

   }


   bool 
   SQLCERecordset::GetIsNull(const AnsiString &FieldName) const
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns true if the column contains NULL
   //---------------------------------------------------------------------------()
   {
      if (IsEOF())
      {
         ReportEOFError_(FieldName);
         return false;
      }

      _variant_t     vaField;
      FieldsPtr      pFields;
      FieldPtr       pField;
      HRESULT			hr;

      hr = cSQLCERecordset->get_Fields( &pFields );
      vaField = FieldName;
      pFields->get_Item( vaField, &pField );

      vaField.Clear();
      pField->get_Value(&vaField);

      if (vaField.vt == VT_NULL)
         return true;
      else
         return false;
   }
}
