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
      m_iNoOfIMAPConnections(-1),
      m_iNoOfSMTPConnections(-1),
      m_iNoOfPOP3Connections(-1)
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
            if (max_connections > 0 && current_connections >= max_connections)
               return false;

            break;
         }
      case STPOP3:
         {
            max_connections = Configuration::Instance()->GetPOP3Configuration()->GetMaxPOP3Connections();
            if (max_connections > 0 && current_connections >= max_connections)
               return false;

            break;
         }
      case STIMAP:
         {
            max_connections = Configuration::Instance()->GetIMAPConfiguration()->GetMaxIMAPConnections();
            if (max_connections > 0 && current_connections >= max_connections)
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
            InterlockedIncrement(&m_iNoOfSMTPConnections);
            break;
         }
      case STPOP3:
         {
            InterlockedIncrement(&m_iNoOfPOP3Connections);
            break;
         }
      case STIMAP:
         {
            InterlockedIncrement(&m_iNoOfIMAPConnections);
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
         InterlockedDecrement(&m_iNoOfSMTPConnections);
         break;
      case STPOP3:
         InterlockedDecrement(&m_iNoOfPOP3Connections);
         break;
      case STIMAP:
         InterlockedDecrement(&m_iNoOfIMAPConnections);
         break;
      }
   }


   long
   SessionManager::GetNumberOfConnections(SessionType st)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the number of connections for a specific connection timeout
   //---------------------------------------------------------------------------()
   {
      switch (st)
      {
      case STSMTP:
         return m_iNoOfSMTPConnections;
         break;
      case STPOP3:
         return m_iNoOfPOP3Connections;
         break;
      case STIMAP:
         return m_iNoOfIMAPConnections;
         break;
      }

      return -1;
   }

   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the total number of connections.
   //---------------------------------------------------------------------------()
   unsigned long
   SessionManager::GetNumberOfConnections()
   {
      long smtpConnectins = 0;
      long pop3Connections = 0;
      long imapConnections = 0;

      InterlockedExchange(&smtpConnectins, m_iNoOfSMTPConnections);
      InterlockedExchange(&pop3Connections, m_iNoOfPOP3Connections);
      InterlockedExchange(&imapConnections, m_iNoOfIMAPConnections);

      return smtpConnectins + pop3Connections + imapConnections;
   }

}