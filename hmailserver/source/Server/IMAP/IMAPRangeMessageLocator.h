// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPRange.h"

namespace HM
{
   class IMAPMessageInRange;
   class Messages;

   class IMAPRangeMessageLocator
   {
   public:
      IMAPRangeMessageLocator();

      std::vector<IMAPMessageInRange> GetMessageInRanges(std::shared_ptr<Messages> messages, bool isUid, std::vector<IMAPRange> ranges);
   private:

   };

}
