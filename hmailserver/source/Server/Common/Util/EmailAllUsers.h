// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class EmailAllUsers
   {
   public:
      EmailAllUsers(void);
      ~EmailAllUsers(void);

      bool Start(const String &sRecipientWildcard, const String &sFromAddress, const String &sFromName, const String &sSubject, const String &sBody);

   };
}