// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SURBLServer;

   class PersistentSURBLServer
   {
   public:
      PersistentSURBLServer(void);
      ~PersistentSURBLServer(void);
      
      static bool DeleteObject(shared_ptr<SURBLServer> pObject);
      static bool SaveObject(shared_ptr<SURBLServer> pObject);
      static bool SaveObject(shared_ptr<SURBLServer> pObject, String &errorMessage);
      static bool ReadObject(shared_ptr<SURBLServer> pObject, shared_ptr<DALRecordset> pRS);

   };
}