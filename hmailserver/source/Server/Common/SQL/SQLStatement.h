// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class DatabaseSettings;
   class DateTime;
   class SQLCommand;

   class  SQLStatement  
   {
   public:

      enum eColType
      {
         ColTypeUnknown = 0,
         ColTypeValue = 1,
         ColTypeString = 2,
         ColTypeRaw = 4
      };

      enum eStatementType
      {
         STUndefined = 0,
         STInsert = 1,
         STUpdate = 2,
         STDelete = 3,
         STSelect = 4
      };


      struct Column
      {
         String sName;
         
         String sString;
         __int64 iInt;

         eColType iType;
      };

	   SQLStatement();
      SQLStatement(eStatementType iType, const String &tableName);
	   virtual ~SQLStatement();

      void AddColumn(const String & sName, const String & sValue);
      void AddColumn(const String & sName, const String & sValue, int iMaxLength);

      void AddColumn(const String & sName, long lValue);
      void AddColumnNULL(const String & sName);
      void AddColumnInt64(const String & sName, __int64 lValue);
      void AddColumnDate(const String & sName, const DateTime &dtValue);
      void AddColumn(const String & sName);
      void AddColumnCommand(const String &column, const String &command);

      void AddWhereClauseColumn(const String &sName, const String &sValue);
      void AddWhereClauseColumn(const String &sName, const AnsiString &sValue);

      void SetAdditionalSQL(const String &additionalSQL) {m_sAdditionalSQL = additionalSQL; }

      void SetTable(const String & sName) { m_sTable = sName; }
      void SetWhereClause(const String & sWhere) { m_sWhere = sWhere; }
      void SetTopRows(int rows);

      void SetStatementType(eStatementType iType) { m_Type = iType; }
      void SetIdentityColumn(const String &sIdentityColumn) {m_sIdentityColumn = sIdentityColumn; }

      SQLCommand GetCommand() const;
      int GetNoOfCols() const;

      String GenerateFromCommand(const SQLCommand &command);

      static String GetCurrentTimestamp();
      static String GetCurrentTimestampPlusMinutes(int iMinutes);
      static String GetCreateDatabase(shared_ptr<DatabaseSettings> pSettings, const String &sDatabaseName);
      static String GetLeftFunction(const String &sParamName, int iLength);
      static String GetTopRows(const String &tableName, int rows);

      static String Escape(const String &input);
      
      static String ConvertWildcardToLike(String input);
      static String ConvertLikeToWildcard(String input);

   private:

      eStatementType m_Type;

      String m_sIdentityColumn;
      String m_sWhere;
      String m_sTable;
      String m_sAdditionalSQL;

      int _topRows;

      std::vector<Column> vecColumns;
      std::vector<Column> m_vecWhereClauseColumns;
   };
}
