// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPRangeParser.h"
#include "IMAPRange.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   std::vector<IMAPRange>
   IMAPRangeParser::Parse(const String &rangeSpecifier)
   {
      std::vector<IMAPRange> result;

      std::vector<String> ranges = StringParser::SplitString(rangeSpecifier, ",");

      for (String range : ranges)
      {
         long colonPos = range.Find(_T(":"));

         unsigned long rangeStart = 0;
         unsigned long rangeEnd = ULONG_MAX;

         if (colonPos >= 0)
         {
            String rangeStartText = range.Mid(0, colonPos);
            String rangeEndText = range.Mid(colonPos + 1);

            rangeStart = _ttoi(rangeStartText);

            if (rangeEndText != _T("*"))
               rangeEnd = _ttoi(rangeEndText);
         }
         else
         {
            rangeStart = rangeEnd = _ttoi(range);
         }

         IMAPRange range;
         range.SetRange(rangeStart, rangeEnd);

         result.push_back(range);
      }

      return result;
   }
}
