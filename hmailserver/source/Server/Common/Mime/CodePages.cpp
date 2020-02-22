// Copyright (c) 2007 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
//
// Contains mapping between character sets and code pages
// http://www.iana.org/assignments/character-sets
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/intl/unicode_81rn.asp

#include "stdafx.h"
#include "CodePages.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   CodePages::CodePages()
   {
      Initialize();
   }

   CodePages::~CodePages()
   {

   }

   void
   CodePages::Initialize()
   {
      // Complete this list.
      AddCodePage_("US-ASCII", 20127);
      
      AddCodePage_("BIG5", 950);
      AddCodePage_("csBig5", 950);

      AddCodePage_("iso-2022-jp", 50221);
      AddCodePage_("csISO2022JP", 50221);

      AddCodePage_("windows-1250", 1250);
      AddCodePage_("windows-1251", 1251);
      AddCodePage_("windows-1252", 1252);
      AddCodePage_("windows-1253", 1253);
      AddCodePage_("windows-1254", 1254);
      AddCodePage_("windows-1255", 1255);
      AddCodePage_("windows-1256", 1256);
      AddCodePage_("windows-1257", 1257);
      AddCodePage_("windows-1258", 1258);

      AddCodePage_("utf-8", CP_UTF8);
      AddCodePage_("utf-7", CP_UTF7);
   }

   void 
   CodePages::AddCodePage_(const AnsiString &sName, int iCodePage)
   {
      AnsiString sTmp = sName;
      sTmp.ToLower();

      code_pages_[sTmp] = iCodePage;
   }

   int 
   CodePages::GetCodePage(const AnsiString &sName) const
   {
      AnsiString lowerCaseCharSet = sName;
      lowerCaseCharSet.ToLower();

      std::map<AnsiString, int>::const_iterator iter = code_pages_.find(lowerCaseCharSet);

      if (iter == code_pages_.end())
         return 0;

      return (*iter).second;
   }
      
}
