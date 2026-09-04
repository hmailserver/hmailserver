// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

namespace HM
{
   class ByteBuffer;

   class IOOperation
   {
   public:

      enum OperationType
      {
         BCTWrite,
         BCTRead,
         BCTShutdownSend,
         BCTDisconnect,
         BCTHandshake
      };

      IOOperation(OperationType type, std::shared_ptr<ByteBuffer> buffer);
      IOOperation(OperationType type, const AnsiString &string);
      ~IOOperation(void);

      OperationType GetType() {return type_; }
      std::shared_ptr<ByteBuffer> GetBuffer() {return buffer_; }
      AnsiString GetString() {return string_; }

      // Text to write to the protocol log once the operation has completed. Logging it when
      // the operation is enqueued would claim data was sent while it's still queued.
      void SetLogData(const AnsiString &log_data) {log_data_ = log_data; }
      AnsiString GetLogData() {return log_data_; }

   private:

      OperationType type_;
      AnsiString string_;
      AnsiString log_data_;
      std::shared_ptr<ByteBuffer> buffer_;

   };
}