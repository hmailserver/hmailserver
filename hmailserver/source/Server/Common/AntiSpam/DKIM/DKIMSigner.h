// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class MimeHeader;

   class DKIMSigner
   {
   public:
      DKIMSigner();

      void Sign(std::shared_ptr<Message> message);

   private:
      // Converts the message to a 7-bit content-transfer-encoding, if it needs it and if it
      // is ours to rewrite. Returns true if the file on disk changed, in which case the
      // header has to be read again before the message is signed.
      static bool ConvertToSevenBit_(std::shared_ptr<Message> message, const String &fileName, MimeHeader &mimeHeader);
   };

}