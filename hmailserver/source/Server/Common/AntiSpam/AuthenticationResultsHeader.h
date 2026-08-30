// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SenderAuthentication;
   class MessageData;

   // Writes the Authentication-Results header described in RFC 8601.
   class AuthenticationResultsHeader
   {
   public:

      // Replaces any Authentication-Results field claiming to have been added by us.
      static void Apply(std::shared_ptr<MessageData> messageData, std::shared_ptr<SenderAuthentication> senderAuthentication);

      static String BuildValue(std::shared_ptr<SenderAuthentication> senderAuthentication, const String &authservId);

      // The authentication server identifier, which is the first token of the value.
      static AnsiString GetAuthservId(const AnsiString &fieldValue);

   private:

      static void RemoveOwnFields_(std::shared_ptr<MessageData> messageData, const String &authservId);
   };
}
