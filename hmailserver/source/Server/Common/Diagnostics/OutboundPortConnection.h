#pragma once

#include "../TCPIP/ProtocolParser.h"
#include "../Util/TransparentTransmissionBuffer.h"
#include "../BO/Message.h"

namespace HM
{
   class ByteBuffer;
   class MessageRecipient;
  
   class OutboundPortConnection : public ProtocolParser  
   {
   public:
	   OutboundPortConnection();
	   virtual ~OutboundPortConnection();

      void OnCouldNotConnect(const AnsiString &sErrorDescription);

      virtual void ParseData(const AnsiString &Request);

   protected:

      virtual void OnConnected();
      virtual AnsiString GetCommandSeparator() const;

   private:

   };
}
