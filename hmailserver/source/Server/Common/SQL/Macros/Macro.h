// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Macro
   {
   public:
      Macro();

      enum MacroType
      {
         Unknown,
         DropColumnKeys,
         SQLCEUPDATE_MESSAGES_SET_FOLDER_INBOX,
         SQLCEUPDATE_FOLDERS_SET_CURRENT_UID,
         SQLCEUPDATE_FOLDERS_SET_NEW_PARENTFOLDERID_WHERE_ZERO,
         SQLCE_UPDATE_IMAP_HIERARCHY_DELIMITER,
      };

      MacroType GetType () const {return _type;}
      void SetType (MacroType type) {_type = type;}

      String GetTableName () const {return _tableName;}
      void SetTableName (String tableName) {_tableName = tableName;}

      String GetColumnName() const {return _columnName;}
      void SetColumnName(String columnName) {_columnName = columnName;}

   private:

      MacroType _type;

      String _tableName;
      String _columnName;
   };
}
