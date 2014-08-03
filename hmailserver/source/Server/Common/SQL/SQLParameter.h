// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class  SQLParameter
   {
   public:
      SQLParameter(const String& name, const int &value);
      SQLParameter(const String& name, const __int64 &value);
      SQLParameter(const String& name, const String &value);
      SQLParameter(const String& name, unsigned int value);
      
      enum Type
      {
         ParamTypeInt32 = 1,
         ParamTypeInt64 = 2,
         ParamTypeString = 3,
         ParamTypeUnsignedInt32 = 4
      };

      int GetType() const;
      int GetInt32Value() const;
      unsigned int GetUnsignedInt32Value() const;
      __int64 GetInt64Value() const;
      String GetStringValue() const;

      String GetName() const;
      
   private:
      
      void ResetValues_();

      String column_name_;
      int int_32value_;
      __int64 int_64value_;
      String string_value_;
      Type type_;
      unsigned int unsigned_int_32value_;
     
   };
}
