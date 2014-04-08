// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "MySQLInterface.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   MySQLInterface::MySQLInterface() :
      m_hLibraryInstance(0),
      p_mysql_real_connect(0),
      p_mysql_close(0),
      p_mysql_init(0),
      p_mysql_error(0),
      p_mysql_query(0),
      p_mysql_store_result(0),
      p_mysql_free_result(0),
      p_mysql_insert_id(0),
      p_mysql_errno(0),
      p_mysql_num_rows(0),
      p_mysql_fetch_row(0),
      p_mysql_num_fields(0),
      p_mysql_fetch_field_direct(0),
      p_mysql_get_server_version(0)
   {

   }

   MySQLInterface::~MySQLInterface()
   {
      if (m_hLibraryInstance)
      {
         FreeLibrary(m_hLibraryInstance);
      }
   }

   String 
   MySQLInterface::_GetLibraryFileName()
   {
      LPTSTR szPath = (LPTSTR)alloca( 2048 );
      DWORD  dwPathLength;

      dwPathLength = GetModuleFileName(NULL, szPath, 2048 );
      szPath[ dwPathLength ] = 0; // --- nullterminated.

      String sPath(szPath);

      int iLastSlash = max(sPath.ReverseFind(_T("\\")), sPath.ReverseFind(_T("/")));
      String sRetVal = sPath.Mid(0, iLastSlash);
      sRetVal += "\\libmysql.dll";

      return sRetVal;
   }

   bool
   MySQLInterface::Load(String &sErrorMessage)
   {
      String sLibrary = _GetLibraryFileName();
      m_hLibraryInstance = LoadLibrary(sLibrary);

      if (!m_hLibraryInstance)
      {
         sErrorMessage = "Error:\r\n" 
                         "The MySQL client (libmysql.dll, 32 bit) could not be loaded.\r\n" 
                         "hMailServer needs this file to be able to connect to MySQL.\r\n"
                         "The MySQL client needs to be manually copied to the hMailServer Bin directory. The file is not included in the hMailServer installation.\r\n" 
                         "Path: " + sLibrary;

         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5094, "MySQLInterface::Load", sErrorMessage);

         return false;
      }

      p_mysql_real_connect = (hm_mysql_real_connect*)GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_real_connect" );
      p_mysql_close = (hm_mysql_close*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_close" );
      p_mysql_init = (hm_mysql_init*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_init" );
      p_mysql_error = (hm_mysql_error*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_error" );
      p_mysql_query = (hm_mysql_query*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_query" );
      p_mysql_store_result = (hm_mysql_store_result*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_store_result" );
      p_mysql_free_result = (hm_mysql_free_result*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_free_result" );
      p_mysql_insert_id = (hm_mysql_insert_id*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_insert_id" );
      p_mysql_errno = (hm_mysql_errno*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_errno" );
      p_mysql_num_rows = (hm_mysql_num_rows*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_num_rows" );
      p_mysql_fetch_row = (hm_mysql_fetch_row*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_fetch_row" );
      p_mysql_num_fields = (hm_mysql_num_fields*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_num_fields" );
      p_mysql_fetch_field_direct = (hm_mysql_fetch_field_direct*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_fetch_field_direct" );
      p_mysql_get_server_version = (hm_mysql_get_server_version*) GetProcAddress( (HMODULE)m_hLibraryInstance, "mysql_get_server_version" );

      return true;
   }

   bool
   MySQLInterface::IsLoaded()
   {
      if (m_hLibraryInstance > 0)
         return true;
      else
         return false;

   }
}
