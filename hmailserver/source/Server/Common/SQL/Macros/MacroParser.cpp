// Copyright (c) 2009 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "MacroParser.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MacroParser::MacroParser(const String &macroString) :
      macro_string_(macroString)
   {
      
   }


   MacroParser::~MacroParser()
   {
   
   }

   Macro 
   MacroParser::Parse()
   {
      if (macro_string_.StartsWith(_T("HM_DROP_COLUMN_OBJECTS")))
      {
         int pos = macro_string_.Find(_T(" "));

         String columnSpecifier = macro_string_.Mid(pos);

         int separator = columnSpecifier.Find(_T("."));

         String tableName = columnSpecifier.Mid(0, separator);
         String columnName = columnSpecifier.Mid(separator+1);

         tableName.Trim();
         columnName.Trim();

         Macro macro;
         macro.SetType(Macro::DropColumnKeys);
         macro.SetTableName(tableName);
         macro.SetColumnName(columnName);

         return macro;
      }
      else if (macro_string_ == _T("UPDATE_MESSAGES_SET_FOLDER_INBOX"))
      {
         Macro macro;
         macro.SetType(Macro::SQLCEUPDATE_MESSAGES_SET_FOLDER_INBOX);
         return macro;
      }
      else if (macro_string_ == _T("UPDATE_FOLDERS_SET_CURRENT_UID"))
      {
         Macro macro;
         macro.SetType(Macro::SQLCEUPDATE_FOLDERS_SET_CURRENT_UID);
         return macro;
      }
      else if (macro_string_ == _T("UPDATE_FOLDERS_SET_NEW_PARENTFOLDERID_WHERE_ZERO"))
      {
         Macro macro;
         macro.SetType(Macro::SQLCEUPDATE_FOLDERS_SET_NEW_PARENTFOLDERID_WHERE_ZERO);
         
         return macro;
      }
      else if (macro_string_ == _T("SQLCE_UPDATE_IMAP_HIERARCHY_DELIMITER"))
      {
         Macro macro;
         macro.SetType(Macro::SQLCE_UPDATE_IMAP_HIERARCHY_DELIMITER);

         return macro;
      }
      
      Macro unknownMacro;
      return unknownMacro;
   }
}
