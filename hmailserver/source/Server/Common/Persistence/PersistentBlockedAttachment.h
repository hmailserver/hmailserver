// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class BlockedAttachment;

   class PersistentBlockedAttachment
   {
   public:
      PersistentBlockedAttachment(void);
      ~PersistentBlockedAttachment(void);
      
      static bool DeleteObject(shared_ptr<BlockedAttachment> pObject);
      static bool SaveObject(shared_ptr<BlockedAttachment> pObject);
      static bool SaveObject(shared_ptr<BlockedAttachment> pObject, String &errorMessage);
      static bool ReadObject(shared_ptr<BlockedAttachment> pObject, shared_ptr<DALRecordset> pRS);

   };
}