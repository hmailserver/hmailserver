// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "TestConnection.h"

#include "TestConnectionResult.h"

#include "../Application/TimeoutCalculator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   TestConnection::TestConnection(ConnectionSecurity connection_security,
      boost::asio::io_service& io_service, 
      boost::asio::ssl::context& context,
      std::shared_ptr<Event> disconnected,
      AnsiString expected_remote_hostname,
      std::shared_ptr<TestConnectionResult> connection_result) :

      TCPConnection(connection_security, io_service, context, disconnected, expected_remote_hostname),
      
      connection_result_(connection_result)
   {
      SetTimeout(10);
      SetAllowConnectToSelf(true);
   }

   TestConnection::~TestConnection()
   {
      
   }

   void 
   TestConnection::OnCouldNotConnect(const AnsiString &error_description)
   {
      connection_result_->SetErrorMessage(error_description);
   }

   void
   TestConnection::OnConnected()
   {
      connection_result_->SetConnectedSuccesfully(true);
   }

   void
   TestConnection::OnHandshakeCompleted()
   {
      connection_result_->SetHandshakeCompletedSuccesfully(true);
   }

}

