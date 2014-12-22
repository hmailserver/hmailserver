// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class TestConnectionResult
   {
   public:
      TestConnectionResult() :
         connected_succesfully_(false),
         handshake_completed_(false)
      {

      }
      void SetConnectedSuccesfully(bool connected_succesfully) { connected_succesfully_ = connected_succesfully; }
      void SetHandshakeCompletedSuccesfully(bool handshake_completed) { handshake_completed_ = handshake_completed; }
      void SetErrorMessage(String error_message) { error_message_ = error_message; }

      bool GetConnectedSuccesfully() { return connected_succesfully_; }
      bool GetHandshakeCompletedSuccesfully() { return handshake_completed_; }
      String GetErrorMessage() { return error_message_; }

   private:

      bool connected_succesfully_;
      bool handshake_completed_;
      String error_message_;
   };
}
