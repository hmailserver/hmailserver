// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "SQLParameter.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SQLParameter::SQLParameter(const String& name, const int &value) :
      _columnName(name),
      _type(ParamTypeInt32)
   {
      _ResetValues();
      _int32Value = value;
   }

   SQLParameter::SQLParameter(const String& name, const __int64 &value) :
      _columnName(name),
      _type(ParamTypeInt64)
   {
      _ResetValues();
      _int64Value = value;
   }

   SQLParameter::SQLParameter(const String& name, const String &value) :
      _columnName(name),
      _type(ParamTypeString)
   {
      _ResetValues();
      _stringValue = value;
   }


   SQLParameter::SQLParameter(const String& name, unsigned int value) :
      _columnName(name),
      _type(ParamTypeUnsignedInt32)
   {
      _ResetValues();
      _unsignedInt32Value = value;
   }

   void 
   SQLParameter::_ResetValues()
   {
      _int32Value = 0;
      _int64Value = 0;
      _stringValue = "";
      _unsignedInt32Value = 0;
   }

   int 
   SQLParameter::GetType() const
   {
      return _type;
   }

   String
   SQLParameter::GetName() const
   {
      return _columnName;
   }

   int 
   SQLParameter::GetInt32Value() const
   {
      return _int32Value;
   }

   unsigned int 
   SQLParameter::GetUnsignedInt32Value() const
   {
      return _unsignedInt32Value;
   }

   __int64 
   SQLParameter::GetInt64Value() const
   {
      return _int64Value;
   }

   String 
   SQLParameter::GetStringValue() const
   {
      return _stringValue;
   }
}
