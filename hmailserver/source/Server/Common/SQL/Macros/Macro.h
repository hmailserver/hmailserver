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

      MacroType GetType () const {return type_;}
      void SetType (MacroType type) {type_ = type;}

      String GetTableName () const {return table_name_;}
      void SetTableName (String tableName) {table_name_ = tableName;}

      String GetColumnName() const {return column_name_;}
      void SetColumnName(String columnName) {column_name_ = columnName;}

   private:

      MacroType type_;

      String table_name_;
      String column_name_;
   };
}
