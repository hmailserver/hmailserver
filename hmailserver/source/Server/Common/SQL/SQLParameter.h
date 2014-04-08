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
      
      void _ResetValues();

      String _columnName;
      int _int32Value;
      __int64 _int64Value;
      String _stringValue;
      Type _type;
      unsigned int _unsignedInt32Value;
     
   };
}
