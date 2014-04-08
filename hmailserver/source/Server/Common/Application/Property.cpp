// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "property.h"
#include "../Util/Crypt.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Property::Property(void)
   {
      m_lValue = 0;
      m_sValue = "";
      m_bSaveCrypted = false;
   }

   Property::Property(const String & sName, long lValue, const String & sValue, bool SaveCrypted)
   {
      m_sName = sName;
      m_lValue = lValue;
      m_sValue = sValue;
      
      m_bSaveCrypted = SaveCrypted;
   }

   Property::~Property(void)
   {

   }

   void
   Property::SetLongValue(long NewVal)
   {
      m_lValue = NewVal;
      _WriteLongSetting(NewVal);
   }

   void
   Property::SetStringValue(const String &NewVal)
   {
      m_sValue = NewVal;
      _WriteStringSetting(NewVal);
   }

   void
   Property::SetBoolValue(bool NewVal)
   {
      m_lValue = NewVal ? 1 : 0;
      _WriteLongSetting(m_lValue);
   }

   bool
   Property::_WriteBoolSetting(bool bValue)
   {
      int iValue = bValue ? 1 : 0;

      SQLCommand command("update hm_settings set settinginteger = @SETTINGINTEGER where settingname = @SETTINGNAME");
      command.AddParameter("@SETTINGINTEGER", iValue);
      command.AddParameter("@SETTINGNAME", m_sName);

      return Application::Instance()->GetDBManager()->Execute(command);
   }


   bool
   Property::_WriteLongSetting(long lValue)
   {
      SQLCommand command("update hm_settings set settinginteger = @SETTINGINTEGER where settingname = @SETTINGNAME");
      command.AddParameter("@SETTINGINTEGER", lValue);
      command.AddParameter("@SETTINGNAME", m_sName);

      return Application::Instance()->GetDBManager()->Execute(command);
   }

   bool
   Property::_WriteStringSetting(const String & sValue)
   {
      String sSQL;


      String sTemp;
      if (m_bSaveCrypted)
         sTemp = Crypt::Instance()->EnCrypt(sValue, Crypt::ETBlowFish);
      else
         sTemp = sValue;

      SQLStatement statement(SQLStatement::STUpdate, "hm_settings");
      statement.AddColumn("settingstring", sTemp);
      statement.SetWhereClause(Formatter::Format("settingname = '{0}'", m_sName));

      return Application::Instance()->GetDBManager()->Execute(statement);
   }
}