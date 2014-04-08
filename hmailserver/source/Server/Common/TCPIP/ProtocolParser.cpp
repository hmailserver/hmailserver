// Copyright (c) 2008 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include <stdafx.h>

#include "ProtocolParser.h"
#include "TCPConnection.h"

#include "../Util/ByteBuffer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   
   ProtocolParser::ProtocolParser()
   {
      _parentConnection = 0;
      _sessionID = 0;
      _timeout = 0;
   }

   ProtocolParser::~ProtocolParser()
   {
      LOG_DEBUG("Ending session " + StringParser::IntToString(_sessionID));
   }

   void 
   ProtocolParser::Initialize(TCPConnection* parentConnection)
   {
      _parentConnection = parentConnection;

      _sessionID = Application::Instance()->GetUniqueID();

      LOG_DEBUG("Creating session " + StringParser::IntToString(_sessionID));
   }



   void 
   ProtocolParser::SetTimeout(int seconds)
   {
      _timeout = seconds;
   }

   int
   ProtocolParser::GetTimeout()
   {
      return _timeout;
   }

   bool
   ProtocolParser::SendData(const AnsiString &sAnsiString)
   {
      _parentConnection->PostWrite(sAnsiString);

      return true;
   }

   bool
   ProtocolParser::SendData(shared_ptr<ByteBuffer> pByteBuffer)
   {
      _parentConnection->PostWrite(pByteBuffer);

      return true;
   }

   void 
   ProtocolParser::PostDisconnect()
   {
      _parentConnection->PostDisconnect();
   }

   int 
   ProtocolParser::GetSessionID() const
   {
      return _sessionID;
   }

   const IPAddress
   ProtocolParser::GetIPAddress() const
   {
      return _parentConnection->GetRemoteEndpointAddress();
   }

   unsigned long 
   ProtocolParser::GetPort() const
   {
      return _parentConnection->GetRemoteEndpointPort();
   }

   unsigned long 
   ProtocolParser::GetLocalPort() const
   {
      return _parentConnection->GetLocalEndpointPort();
   }

   void 
   ProtocolParser::SetReceiveBinary(bool binary)
   {
      _parentConnection->SetReceiveBinary(binary);
   }

   AnsiString 
   ProtocolParser::GetIPAddressString() const
   {
      return GetIPAddress().ToString();
   }

   void
   ProtocolParser::PostReceive()
   {
      _parentConnection->PostRead(GetCommandSeparator());
   }

   void
   ProtocolParser::PostBufferReceive()
   {
      _parentConnection->PostRead("");
   }

   void
   ProtocolParser::PostShutdown(int what)
   {
      _parentConnection->PostShutdown((TCPConnection::ShutdownOption) what);
   }

   void 
   ProtocolParser::UpdateLogoutTimer()
   {
      _parentConnection->UpdateLogoutTimer();
   }

   shared_ptr<SecurityRange>
   ProtocolParser::GetSecurityRange() const
   {
      return _parentConnection->GetSecurityRange();
   }

   int 
   ProtocolParser::GetBufferSize()
   {
      return _parentConnection->GetBufferSize();
   }

   shared_ptr<TCPConnection> 
   ProtocolParser::GetTCPConnectionTemporaryPointer()
   {
      return _parentConnection->GetSharedFromThis();
   }
}
