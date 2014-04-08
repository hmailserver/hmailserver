// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   #define MAX_BUFF_SIZE       20000

   enum SessionType
   {
      STUnknown = 0,
      STSMTP = 1,
      STSMTPClient = 2,
      STPOP3 = 3,
      STPOP3Client = 4,
      STIMAP = 5,
      STListening = 6,
   };

}