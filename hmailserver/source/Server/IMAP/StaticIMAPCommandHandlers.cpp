// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "StaticIMAPCommandHandlers.h"

#include "IMAPCommandAuthenticate.h"
#include "IMAPCommandLogin.h"
#include "IMAPCommandCheck.h"
#include "IMAPCommandSelect.h"
#include "IMAPCommandClose.h"
#include "IMAPCommandCreate.h"
#include "IMAPCommandDelete.h"
#include "IMAPCommandExamine.h"
#include "IMAPCommandExpunge.h"
#include "IMAPCommandSubscribe.h"
#include "IMAPCommandUnsubscribe.h"
#include "IMAPCommandStatus.h"
#include "IMAPCommandRename.h"
#include "IMAPCommandList.h"
#include "IMAPCommandLsub.h"
#include "IMAPCommandCopy.h"
#include "IMAPCommandFetch.h"
#include "IMAPCommandCapability.h"
#include "IMAPCommandStore.h"
#include "IMAPCommandLogout.h"
#include "IMAPCommandNamespace.h"
#include "IMAPCommandMyRights.h"
#include "IMAPCommandGetAcl.h"
#include "IMAPCommandDeleteAcl.h"
#include "IMAPCommandSetAcl.h"
#include "IMAPCommandListRights.h"


// IMAP QUOTA EXTENSION
#include "IMAPCommandGetQuota.h"
#include "IMAPCommandGetQuotaRoot.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   std::map<IMAPConnection::eIMAPCommandType, shared_ptr<IMAPCommand> > StaticIMAPCommandHandlers::mapCommandHandlers;

   StaticIMAPCommandHandlers::StaticIMAPCommandHandlers()
   {

   }

   StaticIMAPCommandHandlers::~StaticIMAPCommandHandlers()
   {

   }

   void
   StaticIMAPCommandHandlers::Init()
   {
      mapCommandHandlers[IMAPConnection::IMAP_LOGIN] = shared_ptr<IMAPCommandLOGIN>(new IMAPCommandLOGIN());
      mapCommandHandlers[IMAPConnection::IMAP_CHECK] = shared_ptr<IMAPCommandCHECK>(new IMAPCommandCHECK());
      mapCommandHandlers[IMAPConnection::IMAP_SELECT] = shared_ptr<IMAPCommandSELECT>(new IMAPCommandSELECT());
      mapCommandHandlers[IMAPConnection::IMAP_CLOSE] = shared_ptr<IMAPCommandCLOSE>(new IMAPCommandCLOSE());
      mapCommandHandlers[IMAPConnection::IMAP_CREATE] = shared_ptr<IMAPCommandCREATE>(new IMAPCommandCREATE());
      mapCommandHandlers[IMAPConnection::IMAP_DELETE] = shared_ptr<IMAPCommandDELETE>(new IMAPCommandDELETE());
      mapCommandHandlers[IMAPConnection::IMAP_EXAMINE] = shared_ptr<IMAPCommandEXAMINE>(new IMAPCommandEXAMINE());
      mapCommandHandlers[IMAPConnection::IMAP_EXPUNGE] = shared_ptr<IMAPCommandEXPUNGE>(new IMAPCommandEXPUNGE());
      mapCommandHandlers[IMAPConnection::IMAP_UNSUBSCRIBE] = shared_ptr<IMAPCommandUNSUBSCRIBE>(new IMAPCommandUNSUBSCRIBE());
      mapCommandHandlers[IMAPConnection::IMAP_SUBSCRIBE] = shared_ptr<IMAPCommandSUBSCRIBE>(new IMAPCommandSUBSCRIBE());
      mapCommandHandlers[IMAPConnection::IMAP_STATUS] = shared_ptr<IMAPCommandSTATUS>(new IMAPCommandSTATUS());
      mapCommandHandlers[IMAPConnection::IMAP_RENAME] = shared_ptr<IMAPCommandRENAME>(new IMAPCommandRENAME());
      mapCommandHandlers[IMAPConnection::IMAP_LIST] = shared_ptr<IMAPCommandLIST>(new IMAPCommandLIST());
      mapCommandHandlers[IMAPConnection::IMAP_LSUB] = shared_ptr<IMAPCommandLSUB>(new IMAPCommandLSUB());
      mapCommandHandlers[IMAPConnection::IMAP_COPY] = shared_ptr<IMAPCommandCOPY>(new IMAPCommandCOPY());
      mapCommandHandlers[IMAPConnection::IMAP_FETCH] = shared_ptr<IMAPCommandFETCH>(new IMAPCommandFETCH());
      mapCommandHandlers[IMAPConnection::IMAP_CAPABILITY] = shared_ptr<IMAPCommandCapability>(new IMAPCommandCapability());
      mapCommandHandlers[IMAPConnection::IMAP_STORE] = shared_ptr<IMAPCommandStore>(new IMAPCommandStore());
      mapCommandHandlers[IMAPConnection::IMAP_AUTHENTICATE] = shared_ptr<IMAPCommandAUTHENTICATE>(new IMAPCommandAUTHENTICATE());
      mapCommandHandlers[IMAPConnection::IMAP_NOOP] = shared_ptr<IMAPCommandNOOP>(new IMAPCommandNOOP());
      mapCommandHandlers[IMAPConnection::IMAP_LOGOUT] = shared_ptr<IMAPCommandLOGOUT>(new IMAPCommandLOGOUT());
      mapCommandHandlers[IMAPConnection::IMAP_UNKNOWN] = shared_ptr<IMAPCommandUNKNOWN>(new IMAPCommandUNKNOWN());
      mapCommandHandlers[IMAPConnection::IMAP_GETQUOTAROOT] = shared_ptr<IMAPCommandGetQuotaRoot>(new IMAPCommandGetQuotaRoot());
      mapCommandHandlers[IMAPConnection::IMAP_GETQUOTA] = shared_ptr<IMAPCommandGetQuota>(new IMAPCommandGetQuota());
      mapCommandHandlers[IMAPConnection::IMAP_NAMESPACE] = shared_ptr<IMAPCommandNamespace>(new IMAPCommandNamespace());
      mapCommandHandlers[IMAPConnection::IMAP_MYRIGHTS] = shared_ptr<IMAPCommandMyRights>(new IMAPCommandMyRights());
      mapCommandHandlers[IMAPConnection::IMAP_GETACL] = shared_ptr<IMAPCommandGetAcl>(new IMAPCommandGetAcl());
      mapCommandHandlers[IMAPConnection::IMAP_DELETEACL] = shared_ptr<IMAPCommandDeleteAcl>(new IMAPCommandDeleteAcl());
      mapCommandHandlers[IMAPConnection::IMAP_SETACL] = shared_ptr<IMAPCommandSetAcl>(new IMAPCommandSetAcl());
      mapCommandHandlers[IMAPConnection::IMAP_LISTRIGHTS] = shared_ptr<IMAPCommandListRights>(new IMAPCommandListRights());
      
   }

   void
   StaticIMAPCommandHandlers::DeInit()
   {
      mapCommandHandlers.clear();
   }

   // Tiny commands

   IMAPResult
   IMAPCommandUNKNOWN::ExecuteCommand(shared_ptr<IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument)
   {
      pConnection->SendResponseString(pArgument->Tag(), "BAD", "Unknown or NULL command");

      return IMAPResult();
   }

   IMAPResult
   IMAPCommandNOOP::ExecuteCommand(shared_ptr<IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument)
   {
      pConnection->SendAsciiData(pArgument->Tag() + " OK NOOP completed\r\n");   

      return IMAPResult();
   
   }

}
