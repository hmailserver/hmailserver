// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\SingletonCreator.h"

#include "..\Mime\CodePages.h"

#include "..\Util\ServerStatus.h"
#include "..\Util\Crypt.h"
#include "..\Util\Languages.h"
#include "..\Util\TLD.h"
#include "..\Application\ObjectCache.h"
#include "..\Application\IniFileSettings.h"
#include "..\Application\ScriptingHost\ScriptServer.h"
#include "..\Application\Reinitializator.h"
#include "..\TCPIP\LocalIPAddresses.h"
#include "..\..\IMAP\StaticIMAPCommandHandlers.h"
#include "..\..\IMAP\IMAPFolderContainer.h"
#include "..\..\POP3\POP3Sessions.h"
#include "..\..\SMTP\SMTPVacationMessageCreator.h"
#include "..\..\SMTP\SPF\SPF.h"
#include "..\AntiSpam\SpamProtection.h"
#include "..\Cache\CacheContainer.h"
#include "..\Cache\MessageCache.h"
#include "..\Threading\WorkQueueManager.h"
#include "..\Application\SessionManager.h"
#include "..\SQL\MySQLInterface.h"
#include "..\Cache\AccountSizeCache.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SingletonCreator::SingletonCreator(void)
   {
   }

   SingletonCreator::~SingletonCreator(void)
   {

   }

   void 
   SingletonCreator::Create()
   {
      
      ErrorManager::CreateInstance();
      WorkQueueManager::CreateInstance();

      Application::CreateInstance();

      ObjectCache::CreateInstance();
      ServerStatus::CreateInstance();
      
      StaticIMAPCommandHandlers::CreateInstance();
      StaticIMAPCommandHandlers::Init();

      POP3Sessions::CreateInstance();
      LocalIPAddresses::CreateInstance();

      Configuration::CreateInstance();
      Crypt::CreateInstance();

      SMTPVacationMessageCreator::CreateInstance();
      ScriptServer::CreateInstance();
      SPF::CreateInstance();

      Reinitializator::CreateInstance();

      Languages::CreateInstance();

      SessionManager::CreateInstance();

      CacheContainer::CreateInstance();
      MessageCache::CreateInstance();
      IniFileSettings::CreateInstance();

      TLD::CreateInstance();
      CodePages::CreateInstance();

      MySQLInterface::CreateInstance();
      SpamProtection::CreateInstance();

      AccountSizeCache::CreateInstance();
      IMAPFolderContainer::CreateInstance();
   }

   void 
   SingletonCreator::Delete()
   {
      IMAPFolderContainer::DeleteInstance();
      AccountSizeCache::DeleteInstance();

      SpamProtection::DeleteInstance();

      MySQLInterface::DeleteInstance();

      CodePages::DeleteInstance();
      TLD::DeleteInstance();

      IniFileSettings::DeleteInstance();
      MessageCache::DeleteInstance();
      CacheContainer::DeleteInstance();

      SessionManager::DeleteInstance();

      Languages::DeleteInstance();

      Reinitializator::DeleteInstance();
      

      SPF::DeleteInstance();
      ScriptServer::DeleteInstance();
      SMTPVacationMessageCreator::DeleteInstance();
      
      Crypt::DeleteInstance();
      Configuration::DeleteInstance();

      LocalIPAddresses::DeleteInstance();
      POP3Sessions::DeleteInstance();
      
      StaticIMAPCommandHandlers::DeInit();
      StaticIMAPCommandHandlers::DeleteInstance();

      ServerStatus::DeleteInstance();
      ObjectCache::DeleteInstance();

      Application::DeleteInstance();
      WorkQueueManager::DeleteInstance();

      ErrorManager::DeleteInstance();


   }


}