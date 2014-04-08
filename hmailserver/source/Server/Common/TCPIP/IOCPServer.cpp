// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "IOCPServer.h"

#include "TCPServer.h"
#include "TCPConnection.h"

#include "LocalIPAddresses.h"

#include "../Threading/WorkQueue.h"
#include "../Threading/WorkQueueManager.h"
#include "../Util/ByteBuffer.h"
#include "../Util/ServerStatus.h"
#include "../BO/TCPIPPort.h"
#include "../BO/TCPIPPorts.h"

#include "IOCPQueueWorkerTask.h"
#include "SocketConstants.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IOCPServer::IOCPServer(void) :
      _dummy_context(_io_service, boost::asio::ssl::context::sslv23)
   {

   }

   IOCPServer::~IOCPServer(void)
   {
   }

   bool 
   IOCPServer::RegisterSessionType(SessionType st)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Registers a new connection type.
   //---------------------------------------------------------------------------()
   {
      m_setSessionTypes.insert(st);

      return true;
   }

   void
   IOCPServer::DoWork()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Creates the IO completion port, creates the worker threads, listen sockets etc.
   //---------------------------------------------------------------------------()
   {
      // Make sure information on which local ports are in use is reset.
      LocalIPAddresses::Instance()->LoadIPAddresses();

      // Create one socket for each IP address specified in the multi-homing settings.
      vector<shared_ptr<TCPIPPort> > vecTCPIPPorts = Configuration::Instance()->GetTCPIPPorts()->GetVector();

      vector<shared_ptr<TCPIPPort> >::iterator iterPort = vecTCPIPPorts.begin();
      vector<shared_ptr<TCPIPPort> >::iterator iterPortEnd = vecTCPIPPorts.end();

      vector<shared_ptr<TCPServer> > vecTCPServers;

      for (; iterPort != iterPortEnd; iterPort++)
      {
         shared_ptr<TCPIPPort> pPort = (*iterPort);
         IPAddress address = pPort->GetAddress();
         int iPort = pPort->GetPortNumber();
         SessionType st = pPort->GetProtocol();
         bool bUseSSL = pPort->GetUseSSL();

         shared_ptr<SSLCertificate> pSSLCertificate;

         if (bUseSSL)
         {
            shared_ptr<SSLCertificates> pSSLCertificates = Configuration::Instance()->GetSSLCertificates();
            pSSLCertificate = pSSLCertificates->GetItemByDBID(pPort->GetSSLCertificateID());
         }

         if (m_setSessionTypes.find(st) == m_setSessionTypes.end())
            continue;

         
         shared_ptr<TCPServer> pTCPServer = shared_ptr<TCPServer> (new TCPServer(_io_service, address, iPort, st, pSSLCertificate));

         pTCPServer->Run();

         vecTCPServers.push_back(pTCPServer);
      }
      
      const int iThreadCount = Configuration::Instance()->GetTCPIPThreads();

      if (iThreadCount <= 0)
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4325, "IOCPServer::DoWork()", "The number of TCP/IP threads has been set to zero.");

      int iQueueID = WorkQueueManager::Instance()->CreateWorkQueue(iThreadCount, "IOCPQueue", WorkQueue::eQTFixedSize);
      
      shared_ptr<WorkQueue> pWorkQueue = WorkQueueManager::Instance()->GetQueue("IOCPQueue");

      // Launch a thread that holds the IOCP objects

      for (int i = 0; i < iThreadCount; i++)
      {
         shared_ptr<IOCPQueueWorkerTask> pWorkerTask = shared_ptr<IOCPQueueWorkerTask>(new IOCPQueueWorkerTask(_io_service));
         WorkQueueManager::Instance()->AddTask(iQueueID, pWorkerTask);
      }	

      // Tell application object that we're now listening. 
      Application::Instance()->SetServerStartedEvent();

      m_evtClose.Wait();

      vector<shared_ptr<TCPServer> >::iterator iterServer = vecTCPServers.begin();
      vector<shared_ptr<TCPServer> >::iterator iterEnd = vecTCPServers.end();
      for (; iterServer != iterEnd; iterServer++)
      {
         (*iterServer)->StopAccept();
      }

      // Now the worker queues will get notifications that the outstanding
      // acceptex sockets are dropped.
      WorkQueueManager::Instance()->RemoveQueue("IOCPQueue");

   }

   shared_ptr<TCPConnection> 
   IOCPServer::CreateConnection(boost::asio::ssl::context& context)
   {
      TCPConnection::PrepareSSLContext(context);
      
      shared_ptr<TCPConnection> pNewConnection = shared_ptr<TCPConnection> (new TCPConnection(true, _io_service, context));

      return pNewConnection;
   }


   shared_ptr<TCPConnection> 
   IOCPServer::CreateConnection()
   {
      shared_ptr<TCPConnection> pNewConnection = shared_ptr<TCPConnection> (new TCPConnection(false, _io_service, _dummy_context));

      return pNewConnection;
   }

   void
   IOCPServer::StopWork()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Called when the server should be stopped.
   //---------------------------------------------------------------------------()
   {
      m_evtClose.Set();
   }

   boost::asio::io_service &
   IOCPServer::GetIOService()
   {
      return _io_service;
   }


}