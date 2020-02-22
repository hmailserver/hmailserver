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
      
      static bool DeleteObject(std::shared_ptr<ServerMessage> pObject);
      static bool SaveObject(std::shared_ptr<ServerMessage> pObject);
      static bool ReadObject(std::shared_ptr<ServerMessage> pObject, std::shared_ptr<DALRecordset> pRS);

   };
}