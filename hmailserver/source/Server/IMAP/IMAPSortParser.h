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

      std::vector<pair<bool,String> > GetSortTypes () {return m_vecSortTypes; }
   private:

      // pair: ascending, criteria
      
      vector<pair<bool,String> > m_vecSortTypes;
   };
}