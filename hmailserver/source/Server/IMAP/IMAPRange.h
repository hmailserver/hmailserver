// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class IMAPRange
   {
   public:
      IMAPRange();

      void SetRange(unsigned int start, unsigned int end);

      unsigned int GetStart() { return start_; }
      unsigned int GetEnd() { return end_; }

   private:

      unsigned int start_;
      unsigned int end_;
   };

}
