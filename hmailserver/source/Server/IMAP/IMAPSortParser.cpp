// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\imapsortparser.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPSortParser::IMAPSortParser(void)
   {
   }

   IMAPSortParser::~IMAPSortParser(void)
   {
   }

   void 
   IMAPSortParser::Parse(const String &sExpression)
   {
      std::vector<String> vecSortCriterias = StringParser::SplitString(sExpression, " ");
      auto iter = vecSortCriterias.begin();
      while (iter != vecSortCriterias.end())
      {
         String sPart = (*iter);

         sPart.ToUpper();
         
         if (sPart == _T("REVERSE"))
         {
            iter++;
            if (iter != vecSortCriterias.end())
            {
               sPart = (*iter);
               sort_types_.push_back(std::make_pair(false, sPart));
            }
         }
         else
         {
            sort_types_.push_back(std::make_pair(true, sPart));
         }

         if (iter != vecSortCriterias.end())
            iter++;
      }
   }
}