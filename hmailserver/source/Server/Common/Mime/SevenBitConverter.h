// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class MimeBody;

   // Re-encodes 8-bit message content into a 7-bit content-transfer-encoding.
   //
   // RFC 6376 section 5.3 asks a signer to convert a message to a 7-bit form before it
   // signs it. A body left as 8-bit is signed in that form, and the signature then breaks
   // at the first hop that downgrades the body to quoted-printable, since that rewrite
   // changes the very bytes the body hash was taken over. DKIMSigner runs it on a message
   // it is about to sign.
   //
   // Four things it leaves alone:
   //
   //   - message/rfc822 parts. RFC 2045 section 6.4 allows message/* only the identity
   //     encodings, so such a part cannot be re-encoded at all; the fix is to descend into
   //     the message it carries and convert that, and then re-serialize it. Left for a
   //     change of its own.
   //   - multipart/signed and multipart/encrypted. RFC 1847 has the signature taken over
   //     the exact bytes of the signed part, so re-encoding it would break the author's
   //     S/MIME or PGP signature in order to protect ours.
   //   - A message whose top-level headers carry no MIME-Version, Content-Type or
   //     Content-Transfer-Encoding, which is to say one that is not MIME at all.
   //     Quoted-printable there would reach the reader as the literal text "=C3=A4",
   //     since nothing marks the body as encoded; labelling it means adding MIME-Version
   //     and Content-Type, which is a decision of its own. This applies only at the top:
   //     below a multipart, a part that declares nothing is still MIME content and is
   //     converted.
   //   - The preamble of a multipart body, the text between its headers and the first
   //     boundary. It is not encoded content, and readers ignore it.
   //
   // A message that still holds 8-bit octets in one of those places is therefore still
   // converted as far as it can be, rather than not at all.
   class SevenBitConverter
   {
   public:
      // True if the buffer holds an octet above 0x7F. A message with none of them cannot
      // need converting, which lets a caller skip parsing it at all.
      static bool ContainsEightBitOctets(const AnsiString &buffer);

      // Re-encodes every 8-bit part at or below body, quoted-printable for text and
      // base64 for everything else, both folded at 76 columns. Returns true if anything
      // changed, so that a caller can leave the message as it found it when nothing did.
      static bool Convert(MimeBody &body);

   private:
      static bool Convert_(MimeBody &body);
      static bool ConvertPart_(MimeBody &body);
      static bool IsSignedOrEncrypted_(const MimeBody &body);
      static bool HasIdentityEncoding_(const MimeBody &body);
   };
}
