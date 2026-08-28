// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "MySQLInterface.h"

#include "../Util/FileUtilities.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   MySQLInterface::MySQLInterface() :
      library_instance_(0),
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
      p_mysql_get_server_version(0),
      p_mysql_options(0),
      is_mariadb_(false)
   {

   }

   MySQLInterface::~MySQLInterface()
   {
      try
      {
         if (library_instance_)
         {
            FreeLibrary(library_instance_);
         }
      }
      catch (...)
      {

      }
   }

   String
   MySQLInterface::GetLibraryDirectory_()
   {
      LPTSTR szPath = (LPTSTR)alloca( 2048 );
      DWORD  dwPathLength;

      dwPathLength = GetModuleFileName(NULL, szPath, 2048 );
      szPath[ dwPathLength ] = 0; // --- nullterminated.

      String sPath(szPath);

      int iLastSlash = std::max(sPath.ReverseFind(_T("\\")), sPath.ReverseFind(_T("/")));
      return sPath.Mid(0, iLastSlash);
   }

   String
   MySQLInterface::GetLibraryFileName_(bool &isMariaDB)
   {
      String sDirectory = GetLibraryDirectory_();

      // Prefer the MariaDB Connector/C client (libmariadb.dll), which is
      // installed into the hMailServer Bin directory. It exposes the same
      // mysql_* C API as libmysql.dll, and uses the OpenSSL DLLs installed
      // next to it for TLS. Fall back to libmysql.dll otherwise, so that
      // installations where it has been replaced keep working unchanged.
      String sMariaDB = sDirectory + "\\libmariadb.dll";
      if (FileUtilities::Exists(sMariaDB))
      {
         isMariaDB = true;
         return sMariaDB;
      }

      isMariaDB = false;
      return sDirectory + "\\libmysql.dll";
   }

   bool
   MySQLInterface::Load(String &sErrorMessage)
   {
      String sLibrary = GetLibraryFileName_(is_mariadb_);
      library_instance_ = LoadLibrary(sLibrary);

      if (!library_instance_)
      {
         String versionArchitecture = Application::Instance()->GetVersionArchitecture();

         sErrorMessage = Formatter::Format("Error:\r\n"
               "The MySQL client ({0}) could not be loaded.\r\n"
               "hMailServer needs this file to be able to connect to MySQL.\r\n"
               "This file is installed into the hMailServer Bin directory. If it has been removed, reinstall hMailServer to restore it.\r\n"
               "A replacement can also be downloaded from https://mariadb.com/downloads/connectors/ (libmariadb.dll) or https://dev.mysql.com/downloads/connector/c/ (libmysql.dll).\r\n"
               "Path: {1}", versionArchitecture, sLibrary);

         ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5094, "MySQLInterface::Load", sErrorMessage);

         return false;
      }

      p_mysql_real_connect = (hm_mysql_real_connect*)GetProcAddress( (HMODULE)library_instance_, "mysql_real_connect" );
      p_mysql_close = (hm_mysql_close*) GetProcAddress( (HMODULE)library_instance_, "mysql_close" );
      p_mysql_init = (hm_mysql_init*) GetProcAddress( (HMODULE)library_instance_, "mysql_init" );
      p_mysql_error = (hm_mysql_error*) GetProcAddress( (HMODULE)library_instance_, "mysql_error" );
      p_mysql_query = (hm_mysql_query*) GetProcAddress( (HMODULE)library_instance_, "mysql_query" );
      p_mysql_store_result = (hm_mysql_store_result*) GetProcAddress( (HMODULE)library_instance_, "mysql_store_result" );
      p_mysql_free_result = (hm_mysql_free_result*) GetProcAddress( (HMODULE)library_instance_, "mysql_free_result" );
      p_mysql_insert_id = (hm_mysql_insert_id*) GetProcAddress( (HMODULE)library_instance_, "mysql_insert_id" );
      p_mysql_errno = (hm_mysql_errno*) GetProcAddress( (HMODULE)library_instance_, "mysql_errno" );
      p_mysql_num_rows = (hm_mysql_num_rows*) GetProcAddress( (HMODULE)library_instance_, "mysql_num_rows" );
      p_mysql_fetch_row = (hm_mysql_fetch_row*) GetProcAddress( (HMODULE)library_instance_, "mysql_fetch_row" );
      p_mysql_num_fields = (hm_mysql_num_fields*) GetProcAddress( (HMODULE)library_instance_, "mysql_num_fields" );
      p_mysql_fetch_field_direct = (hm_mysql_fetch_field_direct*) GetProcAddress( (HMODULE)library_instance_, "mysql_fetch_field_direct" );
      p_mysql_get_server_version = (hm_mysql_get_server_version*) GetProcAddress( (HMODULE)library_instance_, "mysql_get_server_version" );
      p_mysql_options = (hm_mysql_options*) GetProcAddress( (HMODULE)library_instance_, "mysql_options" );

      return true;
   }

   bool
   MySQLInterface::IsLoaded()
   {
      if (library_instance_ > 0)
         return true;
      else
         return false;

   }
}
