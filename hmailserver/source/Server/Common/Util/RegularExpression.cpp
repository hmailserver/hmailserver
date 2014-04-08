// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\regularexpression.h"

#include <Boost/Regex.hpp>
using namespace boost;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   RegularExpression::RegularExpression(void)
   {
   }

   RegularExpression::~RegularExpression(void)
   {
   }

   bool 
   RegularExpression::TestExactMatch(const String &sExpression, const String &sValue)
   {
      try
      {
         wregex expression(sExpression); 
         
         if(regex_match(sValue, expression)) 
            return true;
      }
      catch (std::runtime_error &) // regex_match will throw runtime_error if regexp is too complex.
      {
         return false;
      }

      return false;
   }

   void
   RegularExpressionTester::Test()
   {
      String sValue = "Hits by Solcon spam-filter: 4.5 points by spam04";
      String sExpression = "^Hits by Solcon spam-filter: [0-5]{1}\\.[0-9]{1} points by spam[0-9]+$";

      if (!RegularExpression::TestExactMatch(sExpression, sValue))
         throw 0;

      sValue = "T";
      sExpression = ".";

      if (!RegularExpression::TestExactMatch(sExpression, sValue))
         throw 0;

      sValue = "\r";
      sExpression = ".";

      if (!RegularExpression::TestExactMatch(sExpression, sValue))
         throw 0;

      sValue = "\n";
      sExpression = ".";

      if (!RegularExpression::TestExactMatch(sExpression, sValue))
         throw 0;

      sValue = "TTTTT";
      sExpression = ".";

      if (RegularExpression::TestExactMatch(sExpression, sValue))
         throw 0;

      sValue = "TTTTT";
      sExpression = ".*";

      if (!RegularExpression::TestExactMatch(sExpression, sValue))
         throw 0;

      sValue = "TTTTTAPATTT";
      sExpression = "T*APAT*";

      if (!RegularExpression::TestExactMatch(sExpression, sValue))
         throw 0;

      sValue = "Hits by Solcon spam-filter: 4.5 points by spam04\r\nBlaBla\r\nBlaBla\r\n";
      sExpression = ".*Hits by Solcon spam-filter: [0-5]{1}\\.[0-9]{1} points by spam[0-9]+.*";

      if (!RegularExpression::TestExactMatch(sExpression, sValue))
         throw 0;
   }
}