// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Registry
   {
   public:
      Registry(void);
      ~Registry(void);

      bool GetStringValue(HKEY hive, String key, String valueName, String &value);

   private:

   };
}