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
   SessionManager::SessionManager(void) :
      m_iNoOfIMAPConnections(0),
      m_iNoOfSMTPConnections(0),
      m_iNoOfPOP3Connections(0)
   {

   }

   SessionManager::~SessionManager(void)
   {
   }

   void 
   SessionManager::OnDisconnect(SessionType st)
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

   shared_ptr<ProtocolParser>
   SessionManager::CreateConnection(SessionType t, shared_ptr<SecurityRange> securityRange)
   {
      shared_ptr<ProtocolParser> pParser;

      // Check max per protocol
      int iMaxConnections = 0;
      switch (t)
      {
      case STSMTP:
         {
            iMaxConnections = Configuration::Instance()->GetSMTPConfiguration()->GetMaxSMTPConnections();
            if (iMaxConnections > 0 && m_iNoOfSMTPConnections >= iMaxConnections)
               return pParser;

            break;
         }
      case STPOP3:
         {
            iMaxConnections = Configuration::Instance()->GetPOP3Configuration()->GetMaxPOP3Connections();
            if (iMaxConnections > 0 && m_iNoOfPOP3Connections >= iMaxConnections)
               return pParser;

            break;
         }
      case STIMAP:
         {
            iMaxConnections = Configuration::Instance()->GetIMAPConfiguration()->GetMaxIMAPConnections();
            if (iMaxConnections > 0 && m_iNoOfIMAPConnections >= iMaxConnections)
               return pParser;
            break;
         }
      }


      // Check that client isn't blocked by IP range.
      bool bConnectionAllowed = false;
      switch (t)
      {
         case STSMTP:
            if (!securityRange->GetAllowSMTP())
               return pParser;
            break;
         case STPOP3:
            if (!securityRange->GetAllowPOP3())
               return pParser;
            break;
         case STIMAP:
            if (!securityRange->GetAllowIMAP())
               return pParser;
            break;
      }


      switch (t)
      {
      case STSMTP:
         {
            InterlockedIncrement(&m_iNoOfSMTPConnections);
            pParser = shared_ptr<SMTPConnection>(new SMTPConnection());
            break;
         }
      case STPOP3:
         {
            InterlockedIncrement(&m_iNoOfPOP3Connections);
            pParser = shared_ptr<POP3Connection>(new POP3Connection());
            break;
         }
      case STIMAP:
         {
            InterlockedIncrement(&m_iNoOfIMAPConnections);
            pParser = shared_ptr<IMAPConnection>(new IMAPConnection());
            break;
         }
      }

      return pParser;
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