#pragma once


#define STDCALL __stdcall

typedef void hm_MYSQL;
typedef void hm_MYSQL_RES;
typedef char** hm_MYSQL_ROW;

struct hm_st_mysql_field {
   char *name;                 /* Name of column */
   char *org_name;             /* Original column name, if an alias */ 
   char *table;                /* Table of column if column was a field */
   char *org_table;            /* Org table name, if table was an alias */
   char *db;                   /* Database for table */
   char *catalog;	      /* Catalog for table */
   char *def;                  /* Default value (set by mysql_list_fields) */
};

typedef hm_MYSQL* STDCALL hm_mysql_real_connect(hm_MYSQL *,const char *,const char *,const char *,const char *, unsigned int,const char *,unsigned long);
typedef void STDCALL hm_mysql_close(hm_MYSQL *);
typedef hm_MYSQL* STDCALL hm_mysql_init(hm_MYSQL *);
typedef const char * STDCALL hm_mysql_error(hm_MYSQL *);
typedef int STDCALL hm_mysql_query(hm_MYSQL *mysql, const char *q);
typedef hm_MYSQL_RES * STDCALL hm_mysql_store_result(hm_MYSQL *mysql);
typedef void STDCALL hm_mysql_free_result(hm_MYSQL_RES *result);
typedef unsigned long long STDCALL hm_mysql_insert_id(hm_MYSQL *mysql);
typedef unsigned int STDCALL hm_mysql_errno(hm_MYSQL *mysql);
typedef unsigned long long STDCALL hm_mysql_num_rows(hm_MYSQL_RES *res);
typedef hm_MYSQL_ROW STDCALL hm_mysql_fetch_row(hm_MYSQL_RES *result);
typedef unsigned int STDCALL hm_mysql_num_fields(hm_MYSQL_RES *res);
typedef hm_st_mysql_field *STDCALL hm_mysql_fetch_field_direct(hm_MYSQL_RES *res, unsigned int fieldnr);
typedef unsigned long STDCALL hm_mysql_get_server_version(hm_MYSQL *mysql);

namespace HM
{
   class MySQLInterface : public Singleton<MySQLInterface>
   {
   public:
	   MySQLInterface();
	   virtual ~MySQLInterface();

      bool Load(String &sErrorMessage);
      bool IsLoaded();
   
      hm_mysql_real_connect *p_mysql_real_connect;
      hm_mysql_close *p_mysql_close;
      hm_mysql_init *p_mysql_init;
      hm_mysql_error *p_mysql_error;
      hm_mysql_query *p_mysql_query;
      hm_mysql_store_result *p_mysql_store_result;
      hm_mysql_free_result *p_mysql_free_result;
      hm_mysql_insert_id *p_mysql_insert_id;
      hm_mysql_errno *p_mysql_errno;
      hm_mysql_num_rows *p_mysql_num_rows;
      hm_mysql_fetch_row *p_mysql_fetch_row;
      hm_mysql_num_fields *p_mysql_num_fields;
      hm_mysql_fetch_field_direct *p_mysql_fetch_field_direct;
      hm_mysql_get_server_version *p_mysql_get_server_version;
   private:

      String GetLibraryFileName_();

      HINSTANCE library_instance_;
   };
}
