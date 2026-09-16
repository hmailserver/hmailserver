// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SevenBitConverter.h"

#include "Mime.h"
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
      for (size_t i = 0; i < buffer.size(); i++)
      {
         if ((unsigned char) buffer[i] > 0x7F)
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
      bool isMimeContent = body.FieldExists(CMimeConst::MimeVersion()) ||
                           body.FieldExists(CMimeConst::ContentType()) ||
                           body.FieldExists(CMimeConst::TransferEncoding());

      return Convert_(body, isMimeContent);
   }

   bool
   SevenBitConverter::Convert_(MimeBody &body, bool isMimeContent)
   {
      // A multipart body carries no content of its own that may be encoded: text_ holds
      // its preamble, which readers ignore. Only its parts are converted, and that holds
      // whether or not any were parsed out of it - encoding an unparsed multipart body
      // would put its boundaries out of reach of every reader.
      if (body.GetMediaType() == MimeHeader::MEDIA_MULTIPART)
      {
         bool changed = false;

         for (std::shared_ptr<MimeBody> part = body.FindFirstPart(); part; part = body.FindNextPart())
         {
            if (Convert_(*part, true))
               changed = true;
         }

         return changed;
      }

      return ConvertPart_(body, isMimeContent);
   }

   bool
   SevenBitConverter::ConvertPart_(MimeBody &body, bool isMimeContent)
   {
      // RFC 2045 section 6.4: message/* may carry only an identity encoding, so this part
      // cannot be re-encoded. Converting what it encapsulates is left for a later change.
      if (body.GetMediaType() == MimeHeader::MEDIA_MESSAGE)
         return false;

      // Not MIME content. See the note in the header on why this is left alone.
      if (!isMimeContent)
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

      AnsiString encoded;

      if (body.GetMediaType() == MimeHeader::MEDIA_TEXT)
      {
         // Quoted-printable keeps text readable, and folds the long lines an 8-bit body
         // may carry past the 998 octets RFC 5322 section 2.1.1 allows.
         MimeCodeQP coder;
         coder.SetInput(content, content.GetLength(), true);
         coder.GetOutput(encoded);

         body.SetRawText(encoded);
         body.SetTransferEncoding(CMimeConst::EncodingQP());
      }
      else
      {
         MimeCodeBase64 coder;
         coder.SetInput(content, content.GetLength(), true);
         coder.GetOutput(encoded);

         body.SetRawText(encoded);
         body.SetTransferEncoding(CMimeConst::EncodingBase64());
      }

      return true;
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
