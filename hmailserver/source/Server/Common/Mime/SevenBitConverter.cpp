// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SevenBitConverter.h"

#include "Mime.h"
#include "MimeChar.h"
#include "MimeCode.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool
   SevenBitConverter::ContainsEightBitOctets(const AnsiString &buffer)
   {
      // Iterated rather than indexed: AnsiString both inherits operator[] and converts
      // implicitly to const char*, which leaves buffer[i] ambiguous.
      for (char octet : buffer)
      {
         if (CMimeChar::IsNonAscii((unsigned char) octet))
            return true;
      }

      return false;
   }

   bool
   SevenBitConverter::Convert(MimeBody &body)
   {
      // Whether the top of the message is MIME at all has to be established from its own
      // headers. Everything below a multipart is MIME by construction, so the parts do not
      // have to say so again - RFC 2045 section 5.2 gives a part that declares nothing the
      // default of text/plain, 7bit, and quoted-printable is readable there.
      if (!body.FieldExists(CMimeConst::MimeVersion()) &&
          !body.FieldExists(CMimeConst::ContentType()) &&
          !body.FieldExists(CMimeConst::TransferEncoding()))
      {
         return false;
      }

      return Convert_(body);
   }

   bool
   SevenBitConverter::Convert_(MimeBody &body)
   {
      // A multipart body carries no content of its own that may be encoded: text_ holds
      // its preamble, which readers ignore. Only its parts are converted, and that holds
      // whether or not any were parsed out of it - encoding an unparsed multipart body
      // would put its boundaries out of reach of every reader.
      if (body.IsMultiPart())
      {
         if (IsSignedOrEncrypted_(body))
            return false;

         bool changed = false;

         for (std::shared_ptr<MimeBody> part = body.FindFirstPart(); part; part = body.FindNextPart())
         {
            if (Convert_(*part))
               changed = true;
         }

         return changed;
      }

      return ConvertPart_(body);
   }

   bool
   SevenBitConverter::ConvertPart_(MimeBody &body)
   {
      // RFC 2045 section 6.4: message/* may carry only an identity encoding, so this part
      // cannot be re-encoded. Converting what it encapsulates is left for a later change.
      if (body.IsMessage())
         return false;

      const AnsiString &content = body.GetContent();

      if (!ContainsEightBitOctets(content))
         return false;

      // Only an identity encoding leaves the raw octets in the content, and only then is
      // the content itself the thing to encode. A part that declares base64 or
      // quoted-printable and still holds 8-bit octets is malformed, and re-encoding it
      // would change what it decodes to.
      if (!HasIdentityEncoding_(body))
         return false;

      // Quoted-printable keeps text readable; everything else goes out as base64. Both
      // are folded at 76 columns, which also takes care of the lines an 8-bit body may
      // carry past the 998 octets RFC 5322 section 2.1.1 allows.
      MimeCodeQP qpCoder;
      qpCoder.AddLineBreak(true);

      MimeCodeBase64 base64Coder;

      const bool isText = body.IsText();
      MimeCodeBase &coder = isText ? static_cast<MimeCodeBase &>(qpCoder) : static_cast<MimeCodeBase &>(base64Coder);

      coder.SetInput(content, content.GetLength(), true);

      AnsiString encoded;
      coder.GetOutput(encoded);

      body.SetRawText(encoded);
      body.SetTransferEncoding(isText ? CMimeConst::EncodingQP() : CMimeConst::EncodingBase64());

      return true;
   }

   bool
   SevenBitConverter::IsSignedOrEncrypted_(const MimeBody &body)
   {
      AnsiString subType = body.GetSubType();

      return subType.CompareNoCase("signed") == 0 ||
             subType.CompareNoCase("encrypted") == 0;
   }

   bool
   SevenBitConverter::HasIdentityEncoding_(const MimeBody &body)
   {
      const char *encoding = body.GetTransferEncoding();

      // RFC 2045 section 6.1: absent means 7bit.
      if (encoding == NULL || *encoding == 0)
         return true;

      AnsiString value = encoding;
      value.Trim();

      return value.CompareNoCase(CMimeConst::Encoding7Bit()) == 0 ||
             value.CompareNoCase(CMimeConst::Encoding8Bit()) == 0 ||
             value.CompareNoCase(CMimeConst::EncodingBinary()) == 0;
   }
}
