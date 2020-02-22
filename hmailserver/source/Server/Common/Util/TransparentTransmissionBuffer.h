// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-10-05

// Purpose:
//
// To act as a layer between the SMTP/POP3 components
// and the receiving socket/FILE. This class replaces
// the . with .. in the TCP/IP communication in the 
// SMTP/POP3 protocol. It also checks for the \r\n.\r\n
// sequence in the data to detect end-of-message.
// 

#pragma once

#include "File.h"

#include <boost/function.hpp>
#include "../TCPIP/TCPConnection.h"

namespace HM
{
   class ByteBuffer;
   class SocketConnection;
   class IProtocolParser;

   class TransparentTransmissionBuffer
   {
   public:
      TransparentTransmissionBuffer(bool bSending);
      ~TransparentTransmissionBuffer(void);

      void Append(const BYTE *pBuffer, size_t iBufferSize);
      bool Flush(bool bForce = false);
      
      bool Initialize(std::weak_ptr<TCPConnection> pTcpConnection);
      bool Initialize(const String &sFilename);

      void SetMaxSizeKB(size_t maxSize);
      
      std::shared_ptr<ByteBuffer> GetBuffer() 
      {
         return buffer_; 
      }

      bool GetTransmissionEnded()
      {
         return transmission_ended_;
      }

      bool GetRequiresFlush();
      // There's enough data in the buffer to motivate
      // a flush to file/socket.

      bool GetLastSendEndedWithNewline()
      {
         return last_send_ended_with_newline_;
      }

      bool SaveToFile_(std::shared_ptr<ByteBuffer> pBuffer);
      // Flushes the supplied buffer to file.

      size_t GetSize();

      bool GetCancelTransmission() {return cancel_transmission_;}
      String GetCancelMessage() {return cancel_message_;}
   private:

      void InsertTransmissionPeriod_(std::shared_ptr<ByteBuffer> pIn);
      void RemoveTransmissionPeriod_(std::shared_ptr<ByteBuffer> pIn);

      std::shared_ptr<ByteBuffer> buffer_;
      // The buffer containing the data to send/receive.
      
      bool transmission_ended_;
      // Have we found the end-of-transsmision sequence?
      
      bool is_sending_;
      // Are we sending data, or are we receiving data?

      bool last_send_ended_with_newline_;
      
      // Output types
      File file_;
      std::weak_ptr<TCPConnection> tcp_connection_;

      size_t data_sent_;
      size_t max_size_kb_;

      bool cancel_transmission_;
      String cancel_message_;


      enum Limits
      {
         MAX_LINE_LENGTH = 100000
      };
   };
}
