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
      m_iContains = 0;
      m_iRestoreOptions = 0;
   }

   Backup::~Backup(void)
   {

   }

   bool 
   Backup::GetContains(int iOption) const
   {
      return (m_iContains & iOption) ? true : false;      
   }

   int 
   Backup::GetRestoreOption(int iRestoreOption) const
   {
      return (m_iRestoreOptions & iRestoreOption) ? true : false;      
   }

   void 
   Backup::SetRestoreOption(int iOption, bool bSet)
   {
      if (bSet)
         m_iRestoreOptions = m_iRestoreOptions | iOption;
      else
         m_iRestoreOptions = m_iRestoreOptions &~ iOption;

   }

}