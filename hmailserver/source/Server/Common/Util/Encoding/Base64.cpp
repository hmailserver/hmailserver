// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include <stdexcept>
#include <string.h>

#include "Base64.h"
#include "../../MIME/MimeCode.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      const char BASE64_ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   }

   AnsiString
   Base64::Encode(const char *input, int inputLength)
   {
      // base64 encode the signature.
      MimeCodeBase64 encoder;
      encoder.SetInput(input, inputLength, true);

      AnsiString sEncodedValue;
      encoder.GetOutput(sEncodedValue);

      // the MIME encoder will insert newlines. We don't want this
      // here since this is a generic base64 encoder which may be
      // used in none-mime environments (key encoding anyone?)
      AnsiString result = sEncodedValue;
      result.Replace("\r\n", "");

      return result;

   }

   AnsiString 
   Base64::Decode(const char *input, int inputLength)
   {
      // base64 encode the signature.
      MimeCodeBase64 decoder;
      decoder.SetInput(input, inputLength, false);

      AnsiString sEncodedValue;
      decoder.GetOutput(sEncodedValue);

      // Disabled crlf replace due to facebook dkim issue http://www.hmailserver.com/forum/viewtopic.php?f=7&t=24232
      // Should the replace be removed for encode as well?
      return sEncodedValue;
   }

   AnsiString
   Base64::EncodeUnpadded(const char *input, int inputLength)
   {
      AnsiString result = Encode(input, inputLength);

      int length = result.GetLength();

      while (length > 0 && result.GetAt(length - 1) == '=')
         length--;

      return result.Left(length);
   }

   bool
   Base64::DecodeUnpadded(const AnsiString &input, AnsiString &output)
   {
      output = "";

      unsigned int block = 0;
      int bitsInBlock = 0;

      for (int i = 0; i < input.GetLength(); i++)
      {
         char c = input.GetAt(i);

         const char *position = strchr(BASE64_ALPHABET, c);

         // strchr would match the terminating null of the alphabet itself.
         if (position == nullptr || c == '\0')
         {
            output = "";
            return false;
         }

         block = (block << 6) | (unsigned int) (position - BASE64_ALPHABET);
         bitsInBlock += 6;

         if (bitsInBlock >= 8)
         {
            bitsInBlock -= 8;
            output.append(1, (char) ((block >> bitsInBlock) & 0xff));
         }
      }

      // Whatever is left over must be padding bits, and there can never be a whole
      // byte of them. Anything else is a non-canonical encoding.
      if (bitsInBlock >= 6 || (block & ((1u << bitsInBlock) - 1u)) != 0)
      {
         output = "";
         return false;
      }

      return true;
   }

   void
   Base64Tester::Test()
   {
      String s;

      AnsiString input = "Test";
      s = Base64::Encode(input.GetBuffer(), input.GetLength());
      if (s.Compare(_T("VGVzdA==")) != 0)
         throw;

      input = "Test test test test test test test!!!!";
      s = Base64::Encode(input, input.GetLength());
      if (s.Compare(_T("VGVzdCB0ZXN0IHRlc3QgdGVzdCB0ZXN0IHRlc3QgdGVzdCEhISE=")) != 0)
         throw;

      input = "hMailServer is a free e-mail server for Microsoft Windows. It's used by Internet service providers, companies, governments, schools and enthusiasts in all parts of the world. It supports the common e-mail protocols (IMAP, SMTP and POP3) and can easily be integrated with many existing web mail systems. It has flexible score-based spam protection and can attach to your virus scanner to scan all incoming and outgoing email.";
      s = Base64::Encode(input, input.GetLength());
      if (s.Compare(_T("aE1haWxTZXJ2ZXIgaXMgYSBmcmVlIGUtbWFpbCBzZXJ2ZXIgZm9yIE1pY3Jvc29mdCBXaW5kb3dzLiBJdCdzIHVzZWQgYnkgSW50ZXJuZXQgc2VydmljZSBwcm92aWRlcnMsIGNvbXBhbmllcywgZ292ZXJubWVudHMsIHNjaG9vbHMgYW5kIGVudGh1c2lhc3RzIGluIGFsbCBwYXJ0cyBvZiB0aGUgd29ybGQuIEl0IHN1cHBvcnRzIHRoZSBjb21tb24gZS1tYWlsIHByb3RvY29scyAoSU1BUCwgU01UUCBhbmQgUE9QMykgYW5kIGNhbiBlYXNpbHkgYmUgaW50ZWdyYXRlZCB3aXRoIG1hbnkgZXhpc3Rpbmcgd2ViIG1haWwgc3lzdGVtcy4gSXQgaGFzIGZsZXhpYmxlIHNjb3JlLWJhc2VkIHNwYW0gcHJvdGVjdGlvbiBhbmQgY2FuIGF0dGFjaCB0byB5b3VyIHZpcnVzIHNjYW5uZXIgdG8gc2NhbiBhbGwgaW5jb21pbmcgYW5kIG91dGdvaW5nIGVtYWlsLg==")) != 0)
         throw;

      input = "VGVzdA==";
      s = Base64::Decode(input, input.GetLength());
      if (s.Compare(_T("Test")) != 0)
         throw;

      input = "VGVzdCB0ZXN0IHRlc3QgdGVzdCB0ZXN0IHRlc3QgdGVzdCEhISE=";
      s = Base64::Decode(input, input.GetLength());
      if (s.Compare(_T("Test test test test test test test!!!!")) != 0)
         throw;

      input = "aE1haWxTZXJ2ZXIgaXMgYSBmcmVlIGUtbWFpbCBzZXJ2ZXIgZm9yIE1pY3Jvc29mdCBXaW5kb3dzLiBJdCdzIHVzZWQgYnkgSW50ZXJuZXQgc2VydmljZSBwcm92aWRlcnMsIGNvbXBhbmllcywgZ292ZXJubWVudHMsIHNjaG9vbHMgYW5kIGVudGh1c2lhc3RzIGluIGFsbCBwYXJ0cyBvZiB0aGUgd29ybGQuIEl0IHN1cHBvcnRzIHRoZSBjb21tb24gZS1tYWlsIHByb3RvY29scyAoSU1BUCwgU01UUCBhbmQgUE9QMykgYW5kIGNhbiBlYXNpbHkgYmUgaW50ZWdyYXRlZCB3aXRoIG1hbnkgZXhpc3Rpbmcgd2ViIG1haWwgc3lzdGVtcy4gSXQgaGFzIGZsZXhpYmxlIHNjb3JlLWJhc2VkIHNwYW0gcHJvdGVjdGlvbiBhbmQgY2FuIGF0dGFjaCB0byB5b3VyIHZpcnVzIHNjYW5uZXIgdG8gc2NhbiBhbGwgaW5jb21pbmcgYW5kIG91dGdvaW5nIGVtYWlsLg==";
      s = Base64::Decode(input, input.GetLength());
      if (s.Compare(_T("hMailServer is a free e-mail server for Microsoft Windows. It's used by Internet service providers, companies, governments, schools and enthusiasts in all parts of the world. It supports the common e-mail protocols (IMAP, SMTP and POP3) and can easily be integrated with many existing web mail systems. It has flexible score-based spam protection and can attach to your virus scanner to scan all incoming and outgoing email.")) != 0)
         throw;

      TestUnpadded_();
   }

   void
   Base64Tester::TestUnpadded_()
   {
      // Encoding: the same output as Encode, with the padding removed. The inputs
      // below cover all three remainders of an input length divided by three.
      struct EncodingTestVector
      {
         const char *input;
         const char *expected;
      };

      const EncodingTestVector encodingTestVectors[] =
      {
         { "", "" },
         { "T", "VA" },
         { "Te", "VGU" },
         { "Tes", "VGVz" },
         { "Test", "VGVzdA" },
         { "Test test test test test test test!!!!", "VGVzdCB0ZXN0IHRlc3QgdGVzdCB0ZXN0IHRlc3QgdGVzdCEhISE" },
      };

      for (const EncodingTestVector &testVector : encodingTestVectors)
      {
         AnsiString input = testVector.input;
         AnsiString encoded = Base64::EncodeUnpadded(input.GetBuffer(), input.GetLength());

         if (encoded != testVector.expected)
            throw std::logic_error("EncodeUnpadded produced an unexpected result.");

         AnsiString decoded;
         if (!Base64::DecodeUnpadded(encoded, decoded))
            throw std::logic_error("DecodeUnpadded rejected a string produced by EncodeUnpadded.");

         if (decoded != input)
            throw std::logic_error("A string did not survive an EncodeUnpadded/DecodeUnpadded round trip.");
      }

      // Binary data, covering the two alphabet characters the text vectors above
      // never reach, and a null byte in the middle.
      {
         const unsigned char plusBytes[] = { 0xfb, 0xef, 0xbe };
         AnsiString encoded = Base64::EncodeUnpadded((const char*) plusBytes, sizeof(plusBytes));

         if (encoded != "++++")
            throw std::logic_error("EncodeUnpadded did not produce the expected plus characters.");

         const unsigned char slashBytes[] = { 0x00, 0x10, 0x83, 0xff, 0x00 };
         encoded = Base64::EncodeUnpadded((const char*) slashBytes, sizeof(slashBytes));

         if (encoded != "ABCD/wA")
            throw std::logic_error("EncodeUnpadded did not produce the expected slash character.");

         AnsiString decoded;
         if (!Base64::DecodeUnpadded(encoded, decoded))
            throw std::logic_error("DecodeUnpadded rejected encoded binary data.");

         if (decoded.GetLength() != sizeof(slashBytes) ||
             memcmp(decoded.GetBuffer(), slashBytes, sizeof(slashBytes)) != 0)
         {
            throw std::logic_error("Binary data did not survive an EncodeUnpadded/DecodeUnpadded round trip.");
         }
      }

      // Everything the lenient Decode would have accepted, or silently truncated,
      // must be rejected outright.
      {
         const char *invalidInputs[] =
         {
            "VGVzdA==",    // padded - EncodeUnpadded never produces this
            "VGVz*A",      // outside the alphabet
            "VGVz dA",     // white space
            "VGVz\r\ndA",  // line break, which Decode skips
            "VGVzd",       // length leaves six bits over, so a character is missing
            "VB",          // non-canonical: the four unused bits are not zero
            "VGV",         // non-canonical: the two unused bits are not zero
         };

         for (const char *invalidInput : invalidInputs)
         {
            AnsiString decoded = "not empty";

            if (Base64::DecodeUnpadded(invalidInput, decoded))
               throw std::logic_error("DecodeUnpadded accepted a malformed string.");

            if (decoded.GetLength() != 0)
               throw std::logic_error("DecodeUnpadded left output behind after rejecting a string.");
         }
      }

      // The canonical neighbours of the two rejected non-canonical strings above
      // must still decode, so that the check is not simply refusing everything.
      {
         AnsiString decoded;

         if (!Base64::DecodeUnpadded("VA", decoded) || decoded != "T")
            throw std::logic_error("DecodeUnpadded failed on a canonical two character group.");

         if (!Base64::DecodeUnpadded("VGU", decoded) || decoded != "Te")
            throw std::logic_error("DecodeUnpadded failed on a canonical three character group.");
      }
   }
}