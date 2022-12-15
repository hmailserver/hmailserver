// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class DNSRecord
   {
   public:

      DNSRecord(AnsiString value, int recordType, int preference);

      int GetPreference() { return preference_; }
      AnsiString GetValue() { return value_;  }
   private:
      
      AnsiString value_;
      int record_type_;
      int preference_;

   };


}
