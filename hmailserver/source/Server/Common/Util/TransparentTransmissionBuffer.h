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

      void Append(const BYTE *pBuffer, int iBufferSize);
      bool Flush(bool bForce = false);
      
      bool Initialize(ProtocolParser *pProtocolParser);
      bool Initialize(const String &sFilename);

      void SetMaxSizeKB(int maxSize);
      
      shared_ptr<ByteBuffer> GetBuffer() 
      {
         return m_pBuffer; 
      }

      bool GetTransmissionEnded()
      {
         return m_bTransmissionEnded;
      }

      bool GetRequiresFlush();
      // There's enough data in the buffer to motivate
      // a flush to file/socket.

      bool GetLastSendEndedWithNewline()
      {
         return m_bLastSendEndedWithNewline;
      }

      bool _SaveToFile(shared_ptr<ByteBuffer> pBuffer);
      // Flushes the supplied buffer to file.

      int GetSize();

      bool GetCancelTransmission() {return _cancelTransmission;}
      String GetCancelMessage() {return _cancelMessage;}
   private:

      void _InsertTransmissionPeriod(shared_ptr<ByteBuffer> pIn);
      void _RemoveTransmissionPeriod(shared_ptr<ByteBuffer> pIn);

      shared_ptr<ByteBuffer> m_pBuffer;
      // The buffer containing the data to send/receive.
      
      bool m_bTransmissionEnded;
      // Have we found the end-of-transsmision sequence?
      
      bool m_bIsSending;
      // Are we sending data, or are we receiving data?

      bool m_bLastSendEndedWithNewline;
      
      // Output types
      File m_oFile;
      ProtocolParser *m_pProtocolParser;

      unsigned    int m_iDataSent;
      unsigned int m_iMaxSizeKB;

      bool _cancelTransmission;
      String _cancelMessage;


      enum Limits
      {
         MAX_LINE_LENGTH = 100000
      };
   };
}
