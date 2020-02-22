// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class IMAPSortParser
   {
   public:
      IMAPSortParser(void);
      ~IMAPSortParser(void);

      void Parse(const String &sExpression);

      std::vector<std::pair<bool,String> > GetSortTypes () {return sort_types_; }
   private:

      // pair: ascending, criteria
      
      std::vector<std::pair<bool,String> > sort_types_;
   };
}