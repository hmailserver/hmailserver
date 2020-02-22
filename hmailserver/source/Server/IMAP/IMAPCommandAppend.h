// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"
#include "../Common/Util/ByteBuffer.h"

namespace HM
{
   class ByteBuffer;
   class IMAPFolder;
   class Domain;

   class IMAPCommandAppend : public IMAPCommand  
   {
   public:
	   IMAPCommandAppend();
	   virtual ~IMAPCommandAppend();

      virtual IMAPResult ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);
      void ParseBinary(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<ByteBuffer> pBuf);
      

   private:

      void Finish_(std::shared_ptr<IMAPConnection> pConnection);
      bool TruncateBuffer_(const std::shared_ptr<IMAPConnection> pConnection );
      bool WriteData_(const std::shared_ptr<IMAPConnection> pConnection, const BYTE *pBuf, size_t WriteLen);
      void KillCurrentMessage_();
      
      int GetMaxMessageSize_(std::shared_ptr<const Domain> pDomain);

      String current_tag_;
      String flags_to_set_;
      String create_time_to_set_;
      size_t bytes_left_to_receive_;

      String message_file_name_;

      ByteBuffer append_buffer_;
      std::shared_ptr<IMAPFolder> destination_folder_;
      std::shared_ptr<Message> current_message_;

      
   };

}
