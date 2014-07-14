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

      static bool DeleteObject(shared_ptr<IncomingRelay> pSR);
      static bool SaveObject(shared_ptr<IncomingRelay> pSR);
      static bool SaveObject(shared_ptr<IncomingRelay> pSR, String &errorMessage, PersistenceMode mode);

      static bool ReadObject(shared_ptr<IncomingRelay> pSR, shared_ptr<DALRecordset> pRS);
      static bool ReadObject(shared_ptr<IncomingRelay> pSR, const SQLCommand &command);
      static bool ReadObject(shared_ptr<IncomingRelay> pSR, int ObjectID);
   };

}
