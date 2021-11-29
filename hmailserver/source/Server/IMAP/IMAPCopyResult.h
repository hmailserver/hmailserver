// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPResult.h"

namespace HM
{

   class IMAPCopyResult : public IMAPResult
   {
   public:
      IMAPCopyResult();
      IMAPCopyResult(Result result, const String& message);
      IMAPCopyResult(const IMAPResult& otherCopy);
      IMAPCopyResult& operator= (const IMAPResult& f);

      virtual ~IMAPCopyResult();

      void SetUid(unsigned int sourceUid, unsigned int targetUid);

   private:

      unsigned int sourceUid_;
      unsigned int targetUid_;
   };

}
