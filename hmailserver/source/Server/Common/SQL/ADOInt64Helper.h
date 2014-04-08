// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ADO64Helper 
   {
   public:
     ADO64Helper ();
     static void AddInt64Parameter(_CommandPtr &command, const String& parameterName, __int64 value);
      
   };
}
