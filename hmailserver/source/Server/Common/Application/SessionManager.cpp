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
      no_of_imapconnections_(0),
      no_of_smtpconnections_(0),
      no_of_pop3connections_(0)
   {

   }

   SessionManager::~SessionManager(void)
   {
   }

   bool 
   SessionManager::CreateSession(SessionType session_type, std::shared_ptr<SecurityRange> security_range)
   {
      switch (session_type)
      {
      case STSMTP:
         if (security_range == nullptr || !security_range->GetAllowSMTP())
            return false;
         break;
      case STPOP3:
         if (security_range == nullptr || !security_range->GetAllowPOP3())
            return false;
         break;
      case STIMAP:
         if (security_range == nullptr || !security_range->GetAllowIMAP())
            return false;
         break;
      }
     
      // Check max per protocol
      int max_connections = 0;
      switch (session_type)
      {
      case STSMTP:
         {
            int connection_count = no_of_smtpconnections_.fetch_add(1) + 1;

            max_connections = Configuration::Instance()->GetSMTPConfiguration()->GetMaxSMTPConnections();
            if (max_connections > 0 && connection_count > max_connections)
            {
               no_of_smtpconnections_--;
               return false;
            }

            break;
         }
      case STPOP3:
         {
            int connection_count = no_of_pop3connections_.fetch_add(1) + 1;

            max_connections = Configuration::Instance()->GetPOP3Configuration()->GetMaxPOP3Connections();
            if (max_connections > 0 && connection_count > max_connections)
            {
               no_of_pop3connections_--;
               return false;
            }

            break;
         }
      case STIMAP:
         {
            int connection_count = no_of_imapconnections_.fetch_add(1) + 1;

            max_connections = Configuration::Instance()->GetIMAPConfiguration()->GetMaxIMAPConnections();
            if (max_connections > 0 && connection_count > max_connections)
            {
               no_of_imapconnections_--;
               return false;
            }

            break;
         }
      }

      return true;
   }

   void 
   SessionManager::OnSessionEnded(SessionType st)
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

#ifdef DEBUG
      switch (st)
      {
      case STSMTP:
         if (no_of_smtpconnections_ < 0)
            throw std::logic_error("Negative session count.");
         break;
      case STPOP3:
         if (no_of_pop3connections_ < 0)
            throw std::logic_error("Negative session count.");
         break;
      case STIMAP:
         if (no_of_imapconnections_ < 0)
            throw std::logic_error("Negative session count.");
         break;
      }
#endif

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