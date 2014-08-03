// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include <set>

namespace HM
{

   class POP3Sessions : public Singleton<POP3Sessions>
   {
   public:
	   POP3Sessions();
	   virtual ~POP3Sessions();

      bool IsLocked(__int64 iAccount);
      bool Lock(__int64 iAccount);
      void Unlock(__int64 iAccount);
   
   private:

      std::set<__int64> locked_accounts_;

      boost::recursive_mutex mutex_;
   };

}
