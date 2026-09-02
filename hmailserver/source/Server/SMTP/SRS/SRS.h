// Copyright (c) 2026 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // Sender Rewriting Scheme, as described on http://www.openspf.org/SRS.
   //
   // Forwarding a message with its envelope sender left alone makes the message fail the
   // SPF check at the next server: we are not a permitted sender for the domain the
   // message originally came from. Replacing the sender with the forwarding account
   // instead passes SPF, but throws away the return path, so a bounce ends up with the
   // person who set up the forwarding rather than with whoever sent the message.
   //
   // SRS solves both by rewriting the sender into an address in one of our own domains
   // which the original sender can be recovered from:
   //
   //    SRS0=<hash>=<timestamp>=<domain>=<local part>@<our domain>
   //
   // A bounce comes back to that address, we decode it, and pass the bounce on to the
   // original sender. The hash is what keeps the address from turning us into an open
   // relay: without the server's secret, an address that decodes to an arbitrary
   // recipient cannot be constructed. The timestamp limits how long an address we have
   // handed out stays usable.
   //
   // A message whose sender has already been rewritten - by us or by another forwarder -
   // gets the second form when it is forwarded again:
   //
   //    SRS1=<hash>=<first hop>=<the part of the address following SRS0>@<our domain>
   //
   // This keeps the first hop in the address rather than wrapping one rewritten address
   // inside another, so an address does not grow with every hop it makes. Reversing it
   // yields the SRS0 address at the first hop, which that server can reverse in turn.
   //
   // The class holds no configuration of its own; SenderRewriteScheme reads that from the
   // server configuration and constructs one of these.
   class SRS
   {
   public:
      enum ReverseResult
      {
         ResultSuccess = 0,
         ResultNotAnSrsAddress = 1,
         ResultMalformed = 2,
         ResultInvalidHash = 3,
         ResultExpired = 4,
         ResultNotConfigured = 5,
      };

      SRS(const AnsiString &secret, int maxAgeDays, int hashLength);
      ~SRS();

      String Forward(const String &sender, const String &forwardingDomain) const;
      String Forward(const String &sender, const String &forwardingDomain, time_t now) const;
      // Rewrites sender into an address in forwardingDomain. Returns an empty string if
      // the address could not be created, which includes the case where it would end up
      // longer than an address may be.

      ReverseResult Reverse(const String &address, String &originalAddress) const;
      ReverseResult Reverse(const String &address, time_t now, String &originalAddress) const;
      // Recovers the address a rewritten address was created from. originalAddress is
      // only assigned when the result is ResultSuccess.

      static bool IsSrsAddress(const String &address);
      // Whether the address looks like one Forward creates. Says nothing about whether it
      // is one of ours - only Reverse can tell that.

      static AnsiString GenerateSecret();
      static String GetResultDescription(ReverseResult result);

      static const int DefaultMaxAgeDays;
      static const int DefaultHashLength;
      static const int MinHashLength;
      static const int MaxHashLength;
      static const int MinMaxAgeDays;
      static const int MaxMaxAgeDays;
      static const int MaxAddressLength;
      static const int SecretLength;

   private:

      String BuildSrs0_(const String &senderLocalPart, const String &senderDomain, time_t now) const;
      String BuildSrs1_(const String &firstHop, const String &srs0Payload) const;

      String CreateHash_(const String &data) const;
      bool ValidateHash_(const String &data, const String &hash) const;

      static bool SplitAddress_(const String &address, String &localPart, String &domain);
      static bool ParseTag_(const String &localPart, int &version, String &payload);
      static bool SplitSrs0Payload_(const String &payload, String &hash, String &timestamp, String &domain, String &localPart);
      static bool SplitSrs1Payload_(const String &payload, String &hash, String &firstHop, String &srs0Payload);
      static AnsiString CreateTimestamp_(time_t now);
      static bool ToUTF8_(const String &input, AnsiString &output);
      static bool ValidateTimestamp_(const String &timestamp, time_t now, int maxAgeDays);

      AnsiString secret_;
      int max_age_days_;
      int hash_length_;
   };
}
