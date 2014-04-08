// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "PasswordRemover.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   PasswordRemover::PasswordRemover()
   {
      
   }

   PasswordRemover::~PasswordRemover()
   {

   }

	void
	PasswordRemover::Remove(PRType prt, String &sClientCommand)
	{
      if (prt == PRIMAP)
      {
		   // Starts the second word.
         int iCommandStart = sClientCommand.Find(_T(" "));
         if (iCommandStart == -1)
            return;
      
         iCommandStart++;

         if (sClientCommand.Mid(iCommandStart, 5).CompareNoCase(_T("LOGIN")) != 0)
            return;
         
         // Find end of login name.
         int iUsernameEnd = sClientCommand.Find(_T(" "), iCommandStart + 6);
         if (iUsernameEnd <= 0)
            return;

         sClientCommand = sClientCommand.Mid(0, iUsernameEnd);
         sClientCommand += " ***";
      }
      else if (PRIMAP)
      {
         // Remove from POP3 ...
         if (sClientCommand.Mid(0, 4).CompareNoCase(_T("PASS")) != 0)
            return;

         // Remove the password.
         sClientCommand = sClientCommand.Mid(0, 4) + " ***";
      }

	}
}
