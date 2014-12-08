// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "ADORecordset.h"
#include "ADOConnection.h"
#include "SQLCommand.h"
#include "SQLParameter.h"
#include "../Util/VariantDateTime.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ADORecordset::ADORecordset()
   {
      cur_row_ = 0;
   }

   ADORecordset::~ADORecordset()
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
   ADORecordset::TryOpen(std::shared_ptr<DALConnection> pDALConn, const SQLCommand &command, String &sErrorMessage)
   {  
      cur_row_ = 0;

      std::shared_ptr<ADOConnection> pConn = std::static_pointer_cast<ADOConnection>(pDALConn);
      _ConnectionPtr pADOConnection = pConn->GetConnection();

      // Do we have a connection towards the database?
      if (!pConn->IsConnected())
         return DALConnection::DALConnectionProblem;

      String queryString = command.GetQueryString();
      
      try
      {
         _CommandPtr adoCommand;
         adoCommand.CreateInstance(__uuidof(Command));

         pConn->InitializeCommandParameters(adoCommand, command, queryString);

         adoCommand->CommandType = adCmdText;
         adoCommand->CommandText = _bstr_t(queryString);
         adoCommand->ActiveConnection = pADOConnection;
         
         cADORecordset = adoCommand->Execute(NULL, NULL, adCmdText);
         cADORecordset->PutRefActiveConnection(NULL); 
         adoCommand->ActiveConnection = NULL;

         return DALConnection::DALSuccess;
      }
      catch ( _com_error &err)
      {        
         DALConnection::ExecutionResult dbErr = ADOConnection::GetErrorType(_com_error::WCodeToHRESULT(err.WCode()));

         _bstr_t bstrSource( err.Source() );
         _bstr_t bstrDescription( err.Description() );

         LPCSTR lpcSource = bstrSource;
         String sErrSource = lpcSource;

         LPCSTR lpcDesc = bstrDescription;
         String sErrDesc = lpcDesc;

         sErrorMessage = "Source: ADOConnection::Execute, Code: HM10046, Description: Unknown error when executing SQL statement. \n";
         sErrorMessage += queryString;
         sErrorMessage += "\n";
         sErrorMessage += sErrSource;
         sErrorMessage += "\n";
         sErrorMessage += sErrDesc;

         return dbErr;
      }

      return DALConnection::DALSuccess;
   }

   

   bool
   ADORecordset::Close_()
   {
      if (cADORecordset == NULL)
         return false;

      try
      {
         HRESULT hr = cADORecordset->Close();
      }
      catch (...)
      {
         assert(0);
         return false;
      }
   
      return true;
   }

   long
   ADORecordset::RecordCount() const
   {
      try
      {
         return (long) cADORecordset->GetRecordCount();
      }
      catch ( _com_error &err)
      {
      
         _bstr_t bstrSource( err.Source() );
         _bstr_t bstrDescription( err.Description() );
      
         LPCSTR lpcSource = bstrSource;
         String sErrSource = lpcSource;

         LPCSTR lpcDesc = bstrDescription;
         String sErrDesc = lpcDesc;

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5031, "ADOConnection::RecordCount", "Error while determining the number of rows in a recordset. ADO-Source: " + sErrSource + ". ADO-Description: " + sErrDesc);


         return -1;
      }
   }

   bool
   ADORecordset::IsEOF() const
   {
      if (cur_row_ >= RecordCount())
         return true;
      else
         return false;
   }

   bool
   ADORecordset::MoveNext()
   {
      cur_row_++;
      cADORecordset->MoveNext();
      return true;
   }


   String
   ADORecordset::GetStringValue(const AnsiString &FieldName) const
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
         hr = cADORecordset->get_Fields( &pFields );
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
   ADORecordset::GetLongValue(const AnsiString &FieldName) const
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

	   hr = cADORecordset->get_Fields( &pFields );
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
   ADORecordset::GetInt64Value(const AnsiString &FieldName) const
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

	   hr = cADORecordset->get_Fields( &pFields );
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
         {
            if (vaField.decVal.sign == DECIMAL_NEG)
               return -vaField.cyVal.int64;
            else
               return vaField.cyVal.int64;
         }
         
      }
   }

   double
   ADORecordset::GetDoubleValue(const AnsiString &FieldName) const
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

	   hr = cADORecordset->get_Fields( &pFields );
	   vaField = FieldName;
	   pFields->get_Item( vaField, &pField );

      vaField.Clear();
      pField->get_Value(&vaField);

      if (vaField.vt == VT_NULL)
         return 0;

      return vaField.dblVal;
   }

   std::vector<AnsiString> 
   ADORecordset::GetColumnNames() const
   {
      std::vector<AnsiString> result;

      FieldsPtr      pFields;
      FieldPtr       pField;
      HRESULT			hr;

      hr = cADORecordset->get_Fields( &pFields );

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
   ADORecordset::GetIsNull(const AnsiString &FieldName) const
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

      hr = cADORecordset->get_Fields( &pFields );
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
