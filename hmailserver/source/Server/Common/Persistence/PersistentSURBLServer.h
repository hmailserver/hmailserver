// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SURBLServer;
   enum PersistenceMode;

   class PersistentSURBLServer
   {
   public:
      PersistentSURBLServer(void);
      ~PersistentSURBLServer(void);
      
      static bool DeleteObject(std::shared_ptr<SURBLServer> pObject);
      static bool SaveObject(std::shared_ptr<SURBLServer> pObject);
      static bool SaveObject(std::shared_ptr<SURBLServer> pObject, String &errorMessage, PersistenceMode mode);
      static bool ReadObject(std::shared_ptr<SURBLServer> pObject, std::shared_ptr<DALRecordset> pRS);

   };
}