// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "SessionManager.h"

#include "../../SMTP/SMTPConnection.h"
#include "../../SMTP/SMTPClientConnection.h"
#include "../../SMTP/SMTPConfiguration.h"
#include "../../POP3/POP3Connection.h"
#include "../../POP3/POP3Configuration.h"
#include "../../IMAP/IMAPConnection.h"
#include "../../IMAP/IMAPConfiguration.h"

#include "../BO/SecurityRange.h"
#include "../BO/SSLCertificates.h"
#include "../BO/SSLCertificate.h"
#include "../BO/TCPIPPort.h"
#include "../BO/TCPIPPorts.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   /*
      We pre-create one connection per protocol, so we set the counters to -1 here.
   */
   SessionManager::SessionManager(void) :
      no_of_imapconnections_(-1),
      no_of_smtpconnections_(-1),
      no_of_pop3connections_(-1)
   {

   }

   SessionManager::~SessionManager(void)
   {
   }

   bool 
   SessionManager::GetAllow(SessionType session_type, shared_ptr<SecurityRange> security_range)
   {
      // Check that client isn't blocked by IP range.
      switch (session_type)
      {
      case STSMTP:
         if (!security_range->GetAllowSMTP())
            return false;
         break;
      case STPOP3:
         if (!security_range->GetAllowPOP3())
            return false;
         break;
      case STIMAP:
         if (!security_range->GetAllowIMAP())
            return false;
         break;
      }

      int current_connections = GetNumberOfConnections(session_type);

      // Check max per protocol
      int max_connections = 0;
      switch (session_type)
      {
      case STSMTP:
         {
            max_connections = Configuration::Instance()->GetSMTPConfiguration()->GetMaxSMTPConnections();
            if (max_connections > 0 && current_connections > max_connections)
               return false;

            break;
         }
      case STPOP3:
         {
            max_connections = Configuration::Instance()->GetPOP3Configuration()->GetMaxPOP3Connections();
            if (max_connections > 0 && current_connections > max_connections)
               return false;

            break;
         }
      case STIMAP:
         {
            max_connections = Configuration::Instance()->GetIMAPConfiguration()->GetMaxIMAPConnections();
            if (max_connections > 0 && current_connections > max_connections)
               return false;
            break;
         }
      }

      return true;
   }

   void
   SessionManager::OnCreate(SessionType t)
   {
      switch (t)
      {
      case STSMTP:
         {
            no_of_smtpconnections_++;
            break;
         }
      case STPOP3:
         {
            no_of_pop3connections_++;
            break;
         }
      case STIMAP:
         {
            no_of_imapconnections_++;
            break;
         }
      }
   }


   void 
   SessionManager::OnDestroy(SessionType st)
   {
      switch (st)
      {
      case STSMTP:
         no_of_smtpconnections_--;
         break;
      case STPOP3:
         no_of_pop3connections_--;
         break;
      case STIMAP:
         no_of_imapconnections_--;
         break;
      }
   }


   int
   SessionManager::GetNumberOfConnections(SessionType st)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the number of connections for a specific connection timeout
   //---------------------------------------------------------------------------()
   {
      int result = 0;

      switch (st)
      {
      case STSMTP:
         result = no_of_smtpconnections_;
         break;
      case STPOP3:
         result = no_of_pop3connections_;
         break;
      case STIMAP:
         result = no_of_imapconnections_;
         break;
      }

      if (result < 0)
      {
         // During start-up, the counter may not be initialized yet. (Defualt = -1)
         result = 0;
      }

      return result;
   }

   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the total number of connections.
   //---------------------------------------------------------------------------()
   int
   SessionManager::GetNumberOfConnections()
   {
      return GetNumberOfConnections(STSMTP) + 
             GetNumberOfConnections(STPOP3) +
             GetNumberOfConnections(STIMAP);
   }

}