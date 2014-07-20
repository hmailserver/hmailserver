// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class TCPIPPort;
   enum PersistenceMode;

   class PersistentTCPIPPort
   {
   public:
      PersistentTCPIPPort(void);
      ~PersistentTCPIPPort(void);
      
      static bool DeleteObject(shared_ptr<TCPIPPort> pObject);
      static bool SaveObject(shared_ptr<TCPIPPort> pObject, String &errorMessage, PersistenceMode mode);
      static bool ReadObject(shared_ptr<TCPIPPort> pObject, shared_ptr<DALRecordset> pRS);

   };
}