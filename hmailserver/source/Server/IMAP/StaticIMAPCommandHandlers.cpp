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
#include "IMAPCommandStartTls.h"

// IMAP QUOTA EXTENSION
#include "IMAPCommandGetQuota.h"
#include "IMAPCommandGetQuotaRoot.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   std::map<IMAPConnection::eIMAPCommandType, std::shared_ptr<IMAPCommand> > StaticIMAPCommandHandlers::mapCommandHandlers;

   StaticIMAPCommandHandlers::StaticIMAPCommandHandlers()
   {
      mapCommandHandlers[IMAPConnection::IMAP_LOGIN] = std::shared_ptr<IMAPCommandLOGIN>(new IMAPCommandLOGIN());
      mapCommandHandlers[IMAPConnection::IMAP_CHECK] = std::shared_ptr<IMAPCommandCHECK>(new IMAPCommandCHECK());
      mapCommandHandlers[IMAPConnection::IMAP_SELECT] = std::shared_ptr<IMAPCommandSELECT>(new IMAPCommandSELECT());
      mapCommandHandlers[IMAPConnection::IMAP_CLOSE] = std::shared_ptr<IMAPCommandCLOSE>(new IMAPCommandCLOSE());
      mapCommandHandlers[IMAPConnection::IMAP_CREATE] = std::shared_ptr<IMAPCommandCREATE>(new IMAPCommandCREATE());
      mapCommandHandlers[IMAPConnection::IMAP_DELETE] = std::shared_ptr<IMAPCommandDELETE>(new IMAPCommandDELETE());
      mapCommandHandlers[IMAPConnection::IMAP_EXAMINE] = std::shared_ptr<IMAPCommandEXAMINE>(new IMAPCommandEXAMINE());
      mapCommandHandlers[IMAPConnection::IMAP_EXPUNGE] = std::shared_ptr<IMAPCommandEXPUNGE>(new IMAPCommandEXPUNGE());
      mapCommandHandlers[IMAPConnection::IMAP_UNSUBSCRIBE] = std::shared_ptr<IMAPCommandUNSUBSCRIBE>(new IMAPCommandUNSUBSCRIBE());
      mapCommandHandlers[IMAPConnection::IMAP_SUBSCRIBE] = std::shared_ptr<IMAPCommandSUBSCRIBE>(new IMAPCommandSUBSCRIBE());
      mapCommandHandlers[IMAPConnection::IMAP_STATUS] = std::shared_ptr<IMAPCommandSTATUS>(new IMAPCommandSTATUS());
      mapCommandHandlers[IMAPConnection::IMAP_RENAME] = std::shared_ptr<IMAPCommandRENAME>(new IMAPCommandRENAME());
      mapCommandHandlers[IMAPConnection::IMAP_LIST] = std::shared_ptr<IMAPCommandLIST>(new IMAPCommandLIST());
      mapCommandHandlers[IMAPConnection::IMAP_LSUB] = std::shared_ptr<IMAPCommandLSUB>(new IMAPCommandLSUB());
      mapCommandHandlers[IMAPConnection::IMAP_COPY] = std::shared_ptr<IMAPCommandCOPY>(new IMAPCommandCOPY());
      mapCommandHandlers[IMAPConnection::IMAP_FETCH] = std::shared_ptr<IMAPCommandFETCH>(new IMAPCommandFETCH());
      mapCommandHandlers[IMAPConnection::IMAP_CAPABILITY] = std::shared_ptr<IMAPCommandCapability>(new IMAPCommandCapability());
      mapCommandHandlers[IMAPConnection::IMAP_STORE] = std::shared_ptr<IMAPCommandStore>(new IMAPCommandStore());
      mapCommandHandlers[IMAPConnection::IMAP_AUTHENTICATE] = std::shared_ptr<IMAPCommandAUTHENTICATE>(new IMAPCommandAUTHENTICATE());
      mapCommandHandlers[IMAPConnection::IMAP_NOOP] = std::shared_ptr<IMAPCommandNOOP>(new IMAPCommandNOOP());
      mapCommandHandlers[IMAPConnection::IMAP_LOGOUT] = std::shared_ptr<IMAPCommandLOGOUT>(new IMAPCommandLOGOUT());
      mapCommandHandlers[IMAPConnection::IMAP_UNKNOWN] = std::shared_ptr<IMAPCommandUNKNOWN>(new IMAPCommandUNKNOWN());
      mapCommandHandlers[IMAPConnection::IMAP_GETQUOTAROOT] = std::shared_ptr<IMAPCommandGetQuotaRoot>(new IMAPCommandGetQuotaRoot());
      mapCommandHandlers[IMAPConnection::IMAP_GETQUOTA] = std::shared_ptr<IMAPCommandGetQuota>(new IMAPCommandGetQuota());
      mapCommandHandlers[IMAPConnection::IMAP_NAMESPACE] = std::shared_ptr<IMAPCommandNamespace>(new IMAPCommandNamespace());
      mapCommandHandlers[IMAPConnection::IMAP_MYRIGHTS] = std::shared_ptr<IMAPCommandMyRights>(new IMAPCommandMyRights());
      mapCommandHandlers[IMAPConnection::IMAP_GETACL] = std::shared_ptr<IMAPCommandGetAcl>(new IMAPCommandGetAcl());
      mapCommandHandlers[IMAPConnection::IMAP_DELETEACL] = std::shared_ptr<IMAPCommandDeleteAcl>(new IMAPCommandDeleteAcl());
      mapCommandHandlers[IMAPConnection::IMAP_SETACL] = std::shared_ptr<IMAPCommandSetAcl>(new IMAPCommandSetAcl());
      mapCommandHandlers[IMAPConnection::IMAP_LISTRIGHTS] = std::shared_ptr<IMAPCommandListRights>(new IMAPCommandListRights());
      mapCommandHandlers[IMAPConnection::IMAP_STARTTLS] = std::shared_ptr<IMAPCommandStartTls>(new IMAPCommandStartTls());
   }



   // Tiny commands

   IMAPResult
   IMAPCommandUNKNOWN::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      pConnection->SendResponseString(pArgument->Tag(), "BAD", "Unknown or NULL command");

      return IMAPResult();
   }

   IMAPResult
   IMAPCommandNOOP::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      pConnection->SendAsciiData(pArgument->Tag() + " OK NOOP completed\r\n");   

      return IMAPResult();
   
   }

}
