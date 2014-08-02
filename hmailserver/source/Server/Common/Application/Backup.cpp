// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Backup.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Backup::Backup(void)
   {
      contains_ = 0;
      restore_options_ = 0;
   }

   Backup::~Backup(void)
   {

   }

   bool 
   Backup::GetContains(int iOption) const
   {
      return (contains_ & iOption) ? true : false;      
   }

   int 
   Backup::GetRestoreOption(int iRestoreOption) const
   {
      return (restore_options_ & iRestoreOption) ? true : false;      
   }

   void 
   Backup::SetRestoreOption(int iOption, bool bSet)
   {
      if (bSet)
         restore_options_ = restore_options_ | iOption;
      else
         restore_options_ = restore_options_ &~ iOption;

   }

}