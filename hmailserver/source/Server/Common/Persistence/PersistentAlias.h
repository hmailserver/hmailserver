// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Alias;

   class PersistentAlias
   {
   public:
	   PersistentAlias();
	   virtual ~PersistentAlias();

      static bool DeleteObject(shared_ptr<Alias> pAlias);
      static bool SaveObject(shared_ptr<Alias> pAlias);
      static bool SaveObject(shared_ptr<Alias> pAlias, String &sErrorMessage);

      static bool ReadObject(shared_ptr<Alias> pAlias, shared_ptr<DALRecordset> pRS);
      static bool ReadObject(shared_ptr<Alias> pAlias, const String & sAddress);
      static bool ReadObject(shared_ptr<Alias> pAlias, const SQLCommand &command);
      static bool ReadObject(shared_ptr<Alias> pAlias, __int64 iID);

   };
}
