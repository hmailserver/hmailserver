// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class IncomingRelay;
   enum PersistenceMode;

   class PersistentIncomingRelay
   {
   public:
	   PersistentIncomingRelay();
	   virtual ~PersistentIncomingRelay();

      static bool DeleteObject(std::shared_ptr<IncomingRelay> pSR);
      static bool SaveObject(std::shared_ptr<IncomingRelay> pSR);
      static bool SaveObject(std::shared_ptr<IncomingRelay> pSR, String &errorMessage, PersistenceMode mode);

      static bool ReadObject(std::shared_ptr<IncomingRelay> pSR, std::shared_ptr<DALRecordset> pRS);
      static bool ReadObject(std::shared_ptr<IncomingRelay> pSR, const SQLCommand &command);
      static bool ReadObject(std::shared_ptr<IncomingRelay> pSR, int ObjectID);
   };

}
