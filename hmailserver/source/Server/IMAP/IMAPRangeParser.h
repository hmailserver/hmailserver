// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class IMAPRange;

   class IMAPRangeParser
   {
   public:
      std::vector<IMAPRange> IMAPRangeParser::Parse(const String& rangeSpecifier);
   };

}
