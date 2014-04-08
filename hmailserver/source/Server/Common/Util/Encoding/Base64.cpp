// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Base64.h"
#include "../../MIME/MimeCode.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
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
   }
}