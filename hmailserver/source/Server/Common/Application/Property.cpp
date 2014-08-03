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
      long_value_ = 0;
      string_value_ = "";
      save_crypted_ = false;
   }

   Property::Property(const String & sName, long lValue, const String & sValue, bool SaveCrypted)
   {
      name_ = sName;
      long_value_ = lValue;
      string_value_ = sValue;
      
      save_crypted_ = SaveCrypted;
   }

   Property::~Property(void)
   {

   }

   void
   Property::SetLongValue(long NewVal)
   {
      long_value_ = NewVal;
      WriteLongSetting_(NewVal);
   }

   void
   Property::SetStringValue(const String &NewVal)
   {
      string_value_ = NewVal;
      WriteStringSetting_(NewVal);
   }

   void
   Property::SetBoolValue(bool NewVal)
   {
      long_value_ = NewVal ? 1 : 0;
      WriteLongSetting_(long_value_);
   }

   bool
   Property::WriteBoolSetting_(bool bValue)
   {
      int iValue = bValue ? 1 : 0;

      SQLCommand command("update hm_settings set settinginteger = @SETTINGINTEGER where settingname = @SETTINGNAME");
      command.AddParameter("@SETTINGINTEGER", iValue);
      command.AddParameter("@SETTINGNAME", name_);

      return Application::Instance()->GetDBManager()->Execute(command);
   }


   bool
   Property::WriteLongSetting_(long lValue)
   {
      SQLCommand command("update hm_settings set settinginteger = @SETTINGINTEGER where settingname = @SETTINGNAME");
      command.AddParameter("@SETTINGINTEGER", lValue);
      command.AddParameter("@SETTINGNAME", name_);

      return Application::Instance()->GetDBManager()->Execute(command);
   }

   bool
   Property::WriteStringSetting_(const String & sValue)
   {
      String sSQL;


      String sTemp;
      if (save_crypted_)
         sTemp = Crypt::Instance()->EnCrypt(sValue, Crypt::ETBlowFish);
      else
         sTemp = sValue;

      SQLStatement statement(SQLStatement::STUpdate, "hm_settings");
      statement.AddColumn("settingstring", sTemp);
      statement.SetWhereClause(Formatter::Format("settingname = '{0}'", name_));

      return Application::Instance()->GetDBManager()->Execute(statement);
   }
}