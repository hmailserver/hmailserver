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
      column_name_(name),
      type_(ParamTypeInt32)
   {
      ResetValues_();
      int_32value_ = value;
   }

   SQLParameter::SQLParameter(const String& name, const __int64 &value) :
      column_name_(name),
      type_(ParamTypeInt64)
   {
      ResetValues_();
      int_64value_ = value;
   }

   SQLParameter::SQLParameter(const String& name, const String &value) :
      column_name_(name),
      type_(ParamTypeString)
   {
      ResetValues_();
      string_value_ = value;
   }


   SQLParameter::SQLParameter(const String& name, unsigned int value) :
      column_name_(name),
      type_(ParamTypeUnsignedInt32)
   {
      ResetValues_();
      unsigned_int_32value_ = value;
   }

   void 
   SQLParameter::ResetValues_()
   {
      int_32value_ = 0;
      int_64value_ = 0;
      string_value_ = "";
      unsigned_int_32value_ = 0;
   }

   int 
   SQLParameter::GetType() const
   {
      return type_;
   }

   String
   SQLParameter::GetName() const
   {
      return column_name_;
   }

   int 
   SQLParameter::GetInt32Value() const
   {
      return int_32value_;
   }

   unsigned int 
   SQLParameter::GetUnsignedInt32Value() const
   {
      return unsigned_int_32value_;
   }

   __int64 
   SQLParameter::GetInt64Value() const
   {
      return int_64value_;
   }

   String 
   SQLParameter::GetStringValue() const
   {
      return string_value_;
   }
}
