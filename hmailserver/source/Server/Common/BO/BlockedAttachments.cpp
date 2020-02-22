// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "BlockedAttachments.h"

#include "../Persistence/PersistentBlockedAttachment.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   BlockedAttachments::BlockedAttachments()
   {
   }

   BlockedAttachments::~BlockedAttachments(void)
   {
   }


   void 
   BlockedAttachments::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads all SURBL servers from the database.
   //---------------------------------------------------------------------------()
   {
      String sSQL = _T("select * from hm_blocked_attachments order by bawildcard asc");
      DBLoad_(sSQL);
   }

}