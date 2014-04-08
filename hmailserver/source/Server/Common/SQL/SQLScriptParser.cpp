// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "SQLScriptParser.h"
#include "DatabaseSettings.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SQLScriptParser::SQLScriptParser(shared_ptr<DatabaseSettings> pSettings, const String &sFile) :
      m_pSettings(pSettings),
      m_sFile(sFile)
   {

   }

   SQLScriptParser::~SQLScriptParser()
   {

   }

   bool 
   SQLScriptParser::Parse(String &sErrorMessage)
   {
      String sContents = FileUtilities::ReadCompleteTextFile(m_sFile);
      

      if (sContents.GetLength() == 0)
      {
         sErrorMessage = "Unable to read from file " + m_sFile;
         return false;
      }
   
      String sCommandSeparator;
      switch (m_pSettings->GetType())
      {
      case HM::DatabaseSettings::TypeMSSQLCompactEdition:
      case HM::DatabaseSettings::TypeMSSQLServer:
         sCommandSeparator = "\r\n\r\n";
         break;
      case HM::DatabaseSettings::TypeMYSQLServer:
         sCommandSeparator = "\r\n\r\n";
         break;
      case HM::DatabaseSettings::TypePGServer:
         sCommandSeparator = ";\r\n\r\n";
         break;
      }

      vector<String> vecCommands = StringParser::SplitString(sContents, sCommandSeparator);
      vector<String>::iterator iter = vecCommands.begin();
      vector<String>::iterator iterEnd = vecCommands.end();

      for (; iter != iterEnd; iter++)
      {
         String sCommand = (*iter);

         if (_PreprocessLine(sCommand))
         {
            m_vecCommands.push_back(sCommand);
         }
      }
      

      return true;
   }

   bool 
   SQLScriptParser::_PreprocessLine(String &sLine)
   {
      // Do some basic preprocessing...
      while (sLine.Left(2).Compare(_T("\r\n")) == 0)
         sLine = sLine.Mid(2);
      
      while (sLine.Left(1).Compare(_T(" ")) == 0)
         sLine = sLine.Mid(1);
     
      while (sLine.Left(1).Compare(_T("\t")) == 0)
         sLine = sLine.Mid(1);


      String sTempLine = sLine;

      if (m_pSettings->GetType() == HM::DatabaseSettings::TypeMSSQLCompactEdition)
      {
         if (sTempLine.ToLower().Left(3).Compare(_T("if ")) == 0)
         {
            return false;
         }
         else if (sLine.FindNoCase(_T(" CLUSTERED ")) >= 0)
         {
            sLine.ReplaceNoCase(_T(" CLUSTERED "), _T(" "));
         }
         else if (sLine.FindNoCase(_T("CREATE PROC")) >= 0)
         {
            // Procedures not supported by SQL CE
            return false;
         }

         sLine.Replace(_T("\t"), _T(" "));
         sLine.Replace(_T(" varchar"), _T(" nvarchar"));

      }

      if (sLine.IsEmpty())
         return false;


      return true;
   }

}