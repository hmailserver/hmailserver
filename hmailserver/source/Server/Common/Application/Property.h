// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Cache/Cacheable.h"

namespace HM
{
   class Property : public Cacheable
   {
   public:
      Property(void);
      ~Property(void);

      Property(const String &sName, long lValue, const String &sValue, bool SaveCrypted = false);
   

      AnsiString GetName() const {return m_sName;}
      void SetName(const AnsiString &NewVal) {m_sName = NewVal;}

      int GetLongValue() const {return m_lValue; }
      void SetLongValue(long NewVal);

      String GetStringValue() const {return m_sValue; }
      void SetStringValue(const String &NewVal);

      bool GetBoolValue() const {return m_lValue ? true : false; }
      void SetBoolValue(bool NewVal);

      void SetIsCrypted() {m_bSaveCrypted = true; }
   private:

  
      bool _WriteBoolSetting(bool bValue);
      bool _WriteLongSetting(long lValue);
      bool _WriteStringSetting(const String &sValue);

      long m_lValue;
      String m_sValue;
      AnsiString m_sName;

      bool m_bSaveCrypted;

   };
}
