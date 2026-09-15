// Copyright (c) 2026 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
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
      enum ReverseOutcome
      {
         NotAnSrsAddress = 0,
         Reversed = 1,
         ReversalFailed = 2,
      };

      static bool GetIsEnabled();

      static void ApplyToForwardedMessage(std::shared_ptr<Message> message, const String &forwardingAccount,
                                          const String &forwardingDomain, const String &targetAddress);
      // Sets the envelope sender a forwarded message is to be sent with. SRS is used where
      // it applies; where it deliberately does not, the older ini-file setting
      // RewriteEnvelopeFromWhenForwarding still does if it is switched on. forwardingAccount
      // is the account forwarding the message, and is empty when a global rule forwards it:
      // the server is then doing the forwarding, and the ini-file setting has no account
      // address to put in the sender.

      static String CreateForwardingSender(const String &originalSender, const String &forwardingDomain, const String &targetAddress);
      // The envelope sender a message forwarded to targetAddress should be sent with, as an
      // address in forwardingDomain. Returns an empty string when the sender should be left
      // as it is - because SRS is switched off, because the message is not leaving the
      // server, or because we are the sender's own mail server and it already lists us as a
      // permitted sender.

      static ReverseOutcome TryReverse(const String &recipientAddress, String &originalSender, String &errorMessage);
      // Recovers the sender a rewritten address was created for. NotAnSrsAddress means the
      // address is an ordinary one, which the rest of the server is to make sense of;
      // ReversalFailed that it looks like one we have handed out, but was not created by
      // this server or has expired.

   private:
      static std::shared_ptr<SRS> Create_();
      static bool IsLocalDomain_(const String &address);
      static bool LeavesThisServer_(const String &address, int recursionLevel);
   };
}
