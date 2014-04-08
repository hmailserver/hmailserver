// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ServerMessage;

   class PersistentServerMessage
   {
   public:
      PersistentServerMessage(void);
      ~PersistentServerMessage(void);
      
      static bool DeleteObject(shared_ptr<ServerMessage> pObject);
      static bool SaveObject(shared_ptr<ServerMessage> pObject);
      static bool ReadObject(shared_ptr<ServerMessage> pObject, shared_ptr<DALRecordset> pRS);

   };
}