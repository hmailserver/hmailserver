// Copyright (c) 2026 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SRS;

   // Applies the Sender Rewriting Scheme to the server it is running on: decides when a
   // forwarded message's sender should be rewritten, and turns a rewritten address which
   // comes back to us into the address it was created from.
   //
   // The SRS class itself knows nothing about the server's configuration; this is where
   // the two meet.
   class SenderRewriteScheme
   {
   public:
      static bool GetIsEnabled();

      static String CreateForwardingSender(const String &originalSender, const String &forwardingAddress, const String &targetAddress);
      // The envelope sender a message forwarded from forwardingAddress to targetAddress
      // should be sent with. Returns an empty string when the sender should be left as it
      // is - because SRS is switched off, because the message is not leaving the server,
      // or because we are the sender's own mail server and it already lists us as a
      // permitted sender.

      static bool IsSrsRecipient(const String &recipientAddress);
      // Whether the address looks like one we have handed out. Only true when SRS is
      // enabled: with it switched off, such an address is an ordinary address which may
      // well belong to an account.

      static bool TryReverse(const String &recipientAddress, String &originalSender, String &errorMessage);
      // Recovers the sender a rewritten address was created for. Fails for an address
      // which was not created by this server, or which has expired.

   private:
      static std::shared_ptr<SRS> Create_();
      static bool IsLocalDomain_(const String &address);
   };
}
