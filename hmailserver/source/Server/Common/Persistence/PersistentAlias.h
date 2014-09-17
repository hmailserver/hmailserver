// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Alias;
   enum PersistenceMode;

   class PersistentAlias
   {
   public:
	   PersistentAlias();
	   virtual ~PersistentAlias();

      static bool DeleteObject(std::shared_ptr<Alias> pAlias);
      static bool SaveObject(std::shared_ptr<Alias> pAlias);
      static bool SaveObject(std::shared_ptr<Alias> pAlias, String &sErrorMessage, PersistenceMode mode);

      static bool ReadObject(std::shared_ptr<Alias> pAlias, std::shared_ptr<DALRecordset> pRS);
      static bool ReadObject(std::shared_ptr<Alias> pAlias, const String & sAddress);
      static bool ReadObject(std::shared_ptr<Alias> pAlias, const SQLCommand &command);
      static bool ReadObject(std::shared_ptr<Alias> pAlias, __int64 iID);

   };
}
