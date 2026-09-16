// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "MimeTester.h"
#include "Mime.h"
#include "MimeCode.h"
#include "SevenBitConverter.h"
#include "../BO/Message.h"
#include "../BO/MessageData.h"
#include "../Persistence/PersistentMessage.h"

#ifdef _DEBUG
   #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
   #define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      bool TestFindStringEdgeCases()
      {
         const char* sample = "abc\r\ndef";
         const char* repeated = "abcabc";
         const char* beginning = "\r\nheader";
         const char* emptyNeedleSample = "payload";

         // Finds CRLF in the middle of the buffer.
         if (HM::FindString(sample, "\r\n", sample + strlen(sample)) != sample + 3)
            return false;

         // Returns the first occurrence when the haystack contains repeated matches.
         if (HM::FindString(repeated, "abc", repeated + strlen(repeated)) != repeated)
            return false;

         // Respects the provided start pointer and finds a later occurrence.
         if (HM::FindString(repeated + 1, "abc", repeated + strlen(repeated)) != repeated + 3)
            return false;

         // Finds a match located at the very beginning of the haystack.
         if (HM::FindString(beginning, "\r\n", beginning + strlen(beginning)) != beginning)
            return false;

         // Finds a match located at the very end of the haystack.
         if (HM::FindString(sample, "def", sample + strlen(sample)) != sample + 5)
            return false;

         // Returns the haystack start for an empty needle.
         if (HM::FindString(emptyNeedleSample, "", emptyNeedleSample + strlen(emptyNeedleSample)) != emptyNeedleSample)
            return false;

         // Returns NULL when searching past the only CRLF in the buffer.
         if (HM::FindString(sample + 5, "\r\n", sample + strlen(sample)) != NULL)
            return false;

         // Returns NULL when the allowed search range is too short to contain the needle.
         if (HM::FindString(sample + 3, "\r\n", sample + 4) != NULL)
            return false;

         // Returns NULL when the needle is longer than the haystack range.
         if (HM::FindString(sample, "abcdefghi", sample + 3) != NULL)
            return false;

         // Returns NULL when the needle does not exist in the haystack.
         if (HM::FindString(sample, "xyz", sample + strlen(sample)) != NULL)
            return false;

         // Returns NULL when the match would start exactly at haystackEnd.
         if (HM::FindString(sample, "\r\n", sample + 3) != NULL)
            return false;

         // Returns NULL when the haystack pointer is NULL.
         if (HM::FindString(NULL, "\r\n", sample + strlen(sample)) != NULL)
            return false;

         // Returns NULL when the needle pointer is NULL.
         if (HM::FindString(sample, NULL, sample + strlen(sample)) != NULL)
            return false;

         // Returns NULL when the haystack end pointer is NULL.
         if (HM::FindString(sample, "\r\n", NULL) != NULL)
            return false;

         // Returns NULL when haystackEnd points before haystack.
         if (HM::FindString(sample + 4, "\r\n", sample + 3) != NULL)
            return false;

         // Returns NULL for a closing MIME boundary at EOF with no trailing CRLF.
         if (HM::FindString("--------------9RmO0bL0Xu1K5PGiz2FQ63S5--",
            "\r\n",
            "--------------9RmO0bL0Xu1K5PGiz2FQ63S5--" + strlen("--------------9RmO0bL0Xu1K5PGiz2FQ63S5--")) != NULL)
            return false;

         return true;
      }

      bool TestMultipartWithoutFinalCrlf()
      {
         const char* multipartWithoutFinalCrlf =
            "Content-Type: multipart/mixed; boundary=\"boundary42\"\r\n"
            "\r\n"
            "--boundary42\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "hello\r\n"
            "--boundary42--";

         MimeBody message;
         size_t index = 0;
         bool part_loaded = false;
         size_t loaded = message.Load(multipartWithoutFinalCrlf, strlen(multipartWithoutFinalCrlf), index, part_loaded);

         if (!part_loaded)
            return false;

         if (loaded != strlen(multipartWithoutFinalCrlf))
            return false;

         if (message.GetPartCount() != 1)
            return false;

         return true;
      }

      bool TestMultipartWithFinalCrlf()
      {
         const char* multipartWithFinalCrlf =
            "Content-Type: multipart/mixed; boundary=\"boundary42\"\r\n"
            "\r\n"
            "--boundary42\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "hello\r\n"
            "--boundary42--\r\n";

         MimeBody message;
         size_t index = 0;
         bool part_loaded = false;
         size_t loaded = message.Load(multipartWithFinalCrlf, strlen(multipartWithFinalCrlf), index, part_loaded);

         if (!part_loaded)
            return false;

         if (loaded != strlen(multipartWithFinalCrlf))
            return false;

         if (message.GetPartCount() != 1)
            return false;

         return true;
      }

      bool TestMultipartWithoutClosingBoundary()
      {
         const char* multipartWithoutClosingBoundary =
            "Content-Type: multipart/mixed; boundary=\"boundary42\"\r\n"
            "\r\n"
            "--boundary42\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "hello\r\n";

         MimeBody message;
         size_t index = 0;
         bool part_loaded = false;
         size_t loaded = message.Load(multipartWithoutClosingBoundary, strlen(multipartWithoutClosingBoundary), index, part_loaded);

         if (!part_loaded)
            return false;

         if (loaded != strlen(multipartWithoutClosingBoundary))
            return false;

         if (message.GetPartCount() != 1)
            return false;

         return true;
      }

      bool TestMultipartWithClosingBoundaryMissingSeparator()
      {
         const char* multipartWithClosingBoundaryMissingSeparator =
            "Content-Type: multipart/mixed; boundary=\"boundary42\"\r\n"
            "\r\n"
            "--boundary42\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "hello\r\n"
            "--boundary42--garbage";

         MimeBody message;
         size_t index = 0;
         bool part_loaded = false;
         size_t loaded = message.Load(multipartWithClosingBoundaryMissingSeparator, strlen(multipartWithClosingBoundaryMissingSeparator), index, part_loaded);

         if (!part_loaded)
            return false;

         if (loaded != strlen(multipartWithClosingBoundaryMissingSeparator))
            return false;

         if (message.GetPartCount() != 1)
            return false;

         return true;
      }

      AnsiString QPEncode(const char* input, bool addLineBreak = false)
      {
         MimeCodeQP coder;
         if (addLineBreak)
            coder.AddLineBreak(true);
         coder.SetInput(input, (int)strlen(input), true);
         AnsiString output;
         coder.GetOutput(output);
         return output;
      }

      bool TestQPEncodeEmpty()
      {
         return QPEncode("") == "";
      }

      bool TestQPEncodeSinglePrintableChar()
      {
         return QPEncode("a") == "a";
      }

      bool TestQPEncodeEqualsSign()
      {
         // '=' must always be quoted
         return QPEncode("=") == "=3D";
      }

      bool TestQPEncodeNonAscii()
      {
         // bytes outside printable ASCII range must be quoted
         return QPEncode("\x80") == "=80";
      }

      bool TestQPEncodeCRLF()
      {
         // hard line breaks must be preserved as-is
         return QPEncode("\r\n") == "\r\n";
      }

      bool TestQPEncodeBareLF()
      {
         // bare LF must be preserved (the encoder does not normalise line endings)
         return QPEncode("\n") == "\n";
      }

      bool TestQPEncodeTrailingSpace()
      {
         // space at end of input must be quoted per RFC 2045 §6.7
         return QPEncode("hello ") == "hello=20";
      }

      bool TestQPEncodeTrailingTab()
      {
         // tab at end of input must be quoted per RFC 2045 §6.7
         return QPEncode("hello\t") == "hello=09";
      }

      bool TestQPEncodeSpaceBeforeHardBreak()
      {
         // space immediately before \r\n must be quoted
         return QPEncode("hello \r\n") == "hello=20\r\n";
      }

      bool TestQPEncodeTabBeforeHardBreak()
      {
         // tab immediately before \r\n must be quoted
         return QPEncode("hello\t\r\n") == "hello=09\r\n";
      }

      bool TestQPEncodeSmtpDotQuoted()
      {
         // a lone '.' on its own line (\r\n.\r\n) must be quoted to avoid
         // being interpreted as the SMTP end-of-data marker
         return QPEncode("\r\n.\r\n") == "\r\n=2E\r\n";
      }

      bool TestQPEncodeDotNotQuotedMidLine()
      {
         // '.' in the middle of a line must not be quoted
         return QPEncode("a.b") == "a.b";
      }

      bool TestQPEncodeDotAtEndOfInputNotQuoted()
      {
         // '.' at end of input: pbData+2 would be out of bounds; must fall back to bCopy
         return QPEncode("a\r\n.") == "a\r\n.";
      }

      bool TestQPEncode75CharsNoSoftBreak()
      {
         // 75 regular chars followed by CRLF must not produce a soft line break
         AnsiString input(75, 'a');
         input += "\r\n";
         AnsiString output = QPEncode(input.c_str(), true);
         return output.find("=\r\n") == AnsiString::npos;
      }

      bool TestQPEncode76CharsTriggersSoftBreak()
      {
         // 76 regular chars: the 76th triggers a soft break (line limit is 75 content chars + '=')
         AnsiString input(76, 'a');
         AnsiString output = QPEncode(input.c_str(), true);
         return output.find("=\r\n") != AnsiString::npos;
      }

      bool TestQPEncodeNoTrailingWhitespaceBeforeSoftBreakHighCostNextChar()
      {
         // 73 'a' chars + two spaces + 'b': the first space is copied at nLineLen=73 (below the
         // quoting threshold), leaving nLineLen=74. The second space gets quoted (cost=3), firing
         // a soft break and leaving the first literal space as the last char before =\r\n.
         AnsiString input(73, 'a');
         input += "  b\r\n";

         MimeCodeQP coder;
         coder.AddLineBreak(true);
         coder.SetInput(input.c_str(), (int)input.size(), true);

         AnsiString output;
         coder.GetOutput(output);

         size_t softBreakPos = output.find("=\r\n");
         if (softBreakPos == AnsiString::npos)
            return false;

         char charBeforeSoftBreak = output[(int)(softBreakPos - 1)];
         if (charBeforeSoftBreak == ' ' || charBeforeSoftBreak == '\t')
            return false;

         return true;
      }

      bool TestQPEncodeNoTrailingWhitespaceBeforeSoftBreak()
      {
         // 74 'a' chars + space + 'b': the space lands at line position 75, then 'b' triggers
         // the soft break. Per RFC 2045 §6.7 rule 3, the space must be quoted (=20), not literal.
         AnsiString input(74, 'a');
         input += " b\r\n";

         MimeCodeQP coder;
         coder.AddLineBreak(true);
         coder.SetInput(input.c_str(), (int)input.size(), true);

         AnsiString output;
         coder.GetOutput(output);

         size_t softBreakPos = output.find("=\r\n");
         if (softBreakPos == AnsiString::npos)
            return false;

         char charBeforeSoftBreak = output[(int)(softBreakPos - 1)];
         if (charBeforeSoftBreak == ' ' || charBeforeSoftBreak == '\t')
            return false;

         return true;
      }

      bool TestMultipartWithPartBoundaryMissingCrlf()
      {
         const char* multipartWithPartBoundaryMissingCrlf =
            "Content-Type: multipart/mixed; boundary=\"boundary42\"\r\n"
            "\r\n"
            "--boundary42"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "hello\r\n"
            "--boundary42--";

         MimeBody message;
         size_t index = 0;
         bool part_loaded = false;
         size_t loaded = message.Load(multipartWithPartBoundaryMissingCrlf, strlen(multipartWithPartBoundaryMissingCrlf), index, part_loaded);

         if (!part_loaded)
            return false;

         if (loaded != strlen(multipartWithPartBoundaryMissingCrlf))
            return false;

         if (message.GetPartCount() != 0)
            return false;

         return true;
      }

      // GetParameter must not split on ';' inside a quoted value.
      bool TestGetParameterWithQuotedSemicolon()
      {
         MimeField field;
         const char* line = "Content-Disposition: attachment; filename=\"semi;colon.dll\"\r\n";
         field.Load(line, strlen(line), false);

         AnsiString value;
         if (!field.GetParameter("filename", value))
            return false;
         return value == "semi;colon.dll";
      }

      // A quoted value containing ';' must not consume the parameter that follows it.
      bool TestGetParameterNeighboringParamUnaffectedByQuotedSemicolon()
      {
         MimeField field;
         const char* line = "Content-Disposition: attachment; filename=\"semi;colon.dll\"; size=42\r\n";
         field.Load(line, strlen(line), false);

         AnsiString filename;
         if (!field.GetParameter("filename", filename))
            return false;
         if (filename != "semi;colon.dll")
            return false;

         AnsiString size;
         if (!field.GetParameter("size", size))
            return false;
         return size == "42";
      }

      // RemoveParameter must remove the named parameter while leaving others intact.
      bool TestRemoveParameterRemovesSimpleParam()
      {
         MimeField field;
         const char* line = "Content-Type: text/plain; charset=utf-8; name=\"test.txt\"\r\n";
         field.Load(line, strlen(line), false);

         field.RemoveParameter("name");

         AnsiString val;
         if (field.GetParameter("name", val))
            return false;

         AnsiString charset;
         if (!field.GetParameter("charset", charset))
            return false;
         return charset == "utf-8";
      }

      // RemoveParameter("filename") must remove RFC 2231 continuation parameters
      // (filename*0, filename*1) so that a subsequent SetParameter produces a clean
      // single value that GetParameter returns first.
      bool TestRemoveParameterRemovesRfc2231Continuations()
      {
         MimeField field;
         const char* line = "Content-Disposition: attachment; filename*0=\"long\"; filename*1=\"name.dll\"\r\n";
         field.Load(line, strlen(line), false);

         field.RemoveParameter("filename");
         field.SetParameter("filename", "new.dll");

         AnsiString val;
         if (!field.GetParameter("filename", val))
            return false;
         return val == "new.dll";
      }

      // SetFileName must remove RFC 2231 continuation filename parameters before installing
      // the new single filename= value.  With the old code the continuations were left
      // in place and shadowed the new value returned by GetParameter.
      bool TestSetFileNameReplacesRfc2231ContinuationFilename()
      {
         MimeHeader header;
         const char* headers =
            "Content-Type: application/octet-stream\r\n"
            "Content-Disposition: attachment; filename*0=\"oldpart0\"; filename*1=\"oldpart1\"\r\n"
            "\r\n";
         header.Load(headers, strlen(headers));

         header.SetFileName(L"replacement.dll");

         string result = header.GetParameter(CMimeConst::ContentDisposition(), CMimeConst::Filename());

         if (result == "oldpart0" || result == "oldpart1")
            return false;

         return result.find("replacement.dll") != string::npos;
      }

      // SetFileName must also replace a single RFC 2231 encoded filename*= parameter
      // rather than appending a new filename= after it.
      bool TestSetFileNameReplacesEncodedFilenameVariant()
      {
         MimeHeader header;
         const char* headers =
            "Content-Type: application/octet-stream\r\n"
            "Content-Disposition: attachment; filename*=UTF-8''old%20name.dll\r\n"
            "\r\n";
         header.Load(headers, strlen(headers));

         header.SetFileName(L"replacement.dll");

         string result = header.GetParameter(CMimeConst::ContentDisposition(), CMimeConst::Filename());

         if (result.find("old") != string::npos)
            return false;

         return result.find("replacement.dll") != string::npos;
      }

      // If there is no filename= on Content-Disposition, SetFileName falls back to
      // Content-Type name=. That path should also clear old RFC 2231-style name*
      // values before writing the new filename.
      bool TestSetFileNameReplacesContentTypeNameVariant()
      {
         MimeHeader header;
         const char* headers =
            "Content-Type: application/octet-stream; name*=UTF-8''old%20name.dll\r\n"
            "\r\n";
         header.Load(headers, strlen(headers));

         header.SetFileName(L"replacement.dll");

         string result = header.GetParameter(CMimeConst::ContentType(), CMimeConst::Name());

         if (result.find("old") != string::npos)
            return false;

         return result.find("replacement.dll") != string::npos;
      }

      // An RFC 2231 unquoted value like filename*=UTF-8''hello.dll must not be truncated
      // at the apostrophes.  The old code used IsToken which stops at non-token characters
      // such as apostrophe; the fix scans to ';' or end-of-string instead.
      bool TestRfc2231ApostropheInUnquotedValue()
      {
         MimeField field;
         const char* line = "Content-Disposition: attachment; filename*=UTF-8''hello.dll\r\n";
         field.Load(line, strlen(line), false);

         AnsiString val;
         if (!field.GetParameter("filename", val))
            return false;
         // GetParameter decodes the RFC 2231 value; the decoded result must contain the
         // actual filename.  With the old code the scan stopped at the first apostrophe
         // so only "UTF-8" was captured and "hello.dll" would be absent.
         return val.Find("hello.dll") >= 0;
      }

      // Loads one body part from an in-memory buffer, the way the multipart cases above do.
      std::shared_ptr<MimeBody> LoadBody(const char *text)
      {
         std::shared_ptr<MimeBody> body = std::shared_ptr<MimeBody>(new MimeBody);

         size_t index = 0;
         bool part_loaded = false;
         body->Load(text, strlen(text), index, part_loaded);

         return body;
      }

      AnsiString QPDecode(const AnsiString &input)
      {
         MimeCodeQP coder;
         coder.SetInput(input, input.GetLength(), false);
         AnsiString output;
         coder.GetOutput(output);
         return output;
      }

      AnsiString Base64Decode(const AnsiString &input)
      {
         MimeCodeBase64 coder;
         coder.SetInput(input, input.GetLength(), false);
         AnsiString output;
         coder.GetOutput(output);
         return output;
      }

      bool TestContainsEightBitOctets()
      {
         if (SevenBitConverter::ContainsEightBitOctets(""))
            return false;

         if (SevenBitConverter::ContainsEightBitOctets("plain ascii\r\n"))
            return false;

         // 0x7F is the last 7-bit octet, so it does not count as 8-bit.
         if (SevenBitConverter::ContainsEightBitOctets("\x7f"))
            return false;

         if (!SevenBitConverter::ContainsEightBitOctets("\x80"))
            return false;

         if (!SevenBitConverter::ContainsEightBitOctets("\xff"))
            return false;

         // An octet in the middle of otherwise 7-bit content is still found.
         if (!SevenBitConverter::ContainsEightBitOctets("before\xc3\xa4" "after"))
            return false;

         // A NUL does not end the scan: the content is counted, not treated as a C string.
         AnsiString withNul("a\0\xc3", 3);
         if (!SevenBitConverter::ContainsEightBitOctets(withNul))
            return false;

         return true;
      }

      bool TestSevenBitConverterLeavesSevenBitTextAlone()
      {
         std::shared_ptr<MimeBody> body = LoadBody(
            "Content-Type: text/plain; charset=\"utf-8\"\r\n"
            "Content-Transfer-Encoding: 8bit\r\n"
            "\r\n"
            "nothing here is above 0x7f\r\n");

         // Nothing to do, so the part must be reported as unchanged and left as it was.
         if (SevenBitConverter::Convert(*body))
            return false;

         return AnsiString(body->GetTransferEncoding()) == "8bit";
      }

      bool TestSevenBitConverterEncodesEightBitTextAsQuotedPrintable()
      {
         const char *original = "H\xc3\xa4" "ll\xc3\xb6 - 8-bit octets here\r\n";

         std::shared_ptr<MimeBody> body = LoadBody(
            "Content-Type: text/plain; charset=\"utf-8\"\r\n"
            "Content-Transfer-Encoding: 8bit\r\n"
            "\r\n"
            "H\xc3\xa4" "ll\xc3\xb6 - 8-bit octets here\r\n");

         if (!SevenBitConverter::Convert(*body))
            return false;

         if (AnsiString(body->GetTransferEncoding()) != "quoted-printable")
            return false;

         // The point of the exercise: what is signed and sent must be free of 8-bit octets,
         // so that no hop further down the line has cause to rewrite it.
         if (SevenBitConverter::ContainsEightBitOctets(body->GetContent()))
            return false;

         // And it must still decode to exactly the bytes it started as.
         return QPDecode(body->GetContent()) == AnsiString(original);
      }

      bool TestSevenBitConverterEncodesEightBitBinaryAsBase64()
      {
         // Kept as its own array so that the expected bytes cannot drift from the ones fed
         // to the parser, and so that the length is taken rather than counted by hand.
         static const char bodyBytes[] = "\x01\xfe\xff binary\r\n";
         const size_t bodyLength = sizeof(bodyBytes) - 1;

         std::shared_ptr<MimeBody> body = LoadBody(
            "Content-Type: application/octet-stream\r\n"
            "Content-Transfer-Encoding: binary\r\n"
            "\r\n"
            "\x01\xfe\xff binary\r\n");

         if (!SevenBitConverter::Convert(*body))
            return false;

         // Quoted-printable would bloat content that is mostly non-ASCII, so it goes out
         // as base64 instead.
         if (AnsiString(body->GetTransferEncoding()) != "base64")
            return false;

         if (SevenBitConverter::ContainsEightBitOctets(body->GetContent()))
            return false;

         return Base64Decode(body->GetContent()) == AnsiString(bodyBytes, bodyLength);
      }

      bool TestSevenBitConverterConvertsPartsOfMultipart()
      {
         std::shared_ptr<MimeBody> body = LoadBody(
            "Content-Type: multipart/mixed; boundary=\"boundary42\"\r\n"
            "Content-Transfer-Encoding: 7bit\r\n"
            "\r\n"
            "This is a multi-part message in MIME format.\r\n"
            "--boundary42\r\n"
            "Content-Type: text/plain; charset=\"utf-8\"\r\n"
            "Content-Transfer-Encoding: 8bit\r\n"
            "\r\n"
            "\xc3\xa4" "ight bit\r\n"
            "--boundary42\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Transfer-Encoding: 7bit\r\n"
            "\r\n"
            "seven bit\r\n"
            "--boundary42--\r\n");

         if (body->GetPartCount() != 2)
            return false;

         if (!SevenBitConverter::Convert(*body))
            return false;

         // The multipart body itself is never encoded: doing so would bury the boundaries
         // its own Content-Type announces.
         if (AnsiString(body->GetTransferEncoding()) != "7bit")
            return false;

         std::shared_ptr<MimeBody> eightBitPart = body->FindFirstPart();
         std::shared_ptr<MimeBody> sevenBitPart = body->FindNextPart();

         if (!eightBitPart || !sevenBitPart)
            return false;

         if (AnsiString(eightBitPart->GetTransferEncoding()) != "quoted-printable")
            return false;

         if (SevenBitConverter::ContainsEightBitOctets(eightBitPart->GetContent()))
            return false;

         // The part that needed nothing is left exactly as it was.
         if (AnsiString(sevenBitPart->GetTransferEncoding()) != "7bit")
            return false;

         if (AnsiString(sevenBitPart->GetContent()).Find("seven bit") < 0)
            return false;

         // What matters in the end is the message as it is written back out, since that is
         // what gets hashed and sent. Re-serializing must carry the converted part with it.
         AnsiString serialized;
         body->Store(serialized);

         if (SevenBitConverter::ContainsEightBitOctets(serialized))
            return false;

         return serialized.Find("Content-Transfer-Encoding: quoted-printable") >= 0;
      }

      bool TestSevenBitConverterConvertsBareMultipartPart()
      {
         // The part declares no headers of its own. RFC 2045 section 5.2 gives it the
         // default of text/plain, 7bit - it is still MIME content, because the multipart
         // around it is, so quoted-printable there is read back correctly.
         std::shared_ptr<MimeBody> body = LoadBody(
            "MIME-Version: 1.0\r\n"
            "Content-Type: multipart/mixed; boundary=\"boundary42\"\r\n"
            "\r\n"
            "--boundary42\r\n"
            "\r\n"
            "\xc3\xa4" "ight bit\r\n"
            "--boundary42--\r\n");

         if (body->GetPartCount() != 1)
            return false;

         if (!SevenBitConverter::Convert(*body))
            return false;

         std::shared_ptr<MimeBody> part = body->FindFirstPart();
         if (!part)
            return false;

         if (AnsiString(part->GetTransferEncoding()) != "quoted-printable")
            return false;

         return !SevenBitConverter::ContainsEightBitOctets(part->GetContent());
      }

      bool TestSevenBitConverterLeavesMessageRfc822Alone()
      {
         // RFC 2045 section 6.4 allows message/* only the identity encodings, so this part
         // cannot be re-encoded. Descending into what it carries is left for a later change;
         // until then it must be left alone rather than mangled.
         std::shared_ptr<MimeBody> body = LoadBody(
            "Content-Type: message/rfc822\r\n"
            "Content-Transfer-Encoding: 8bit\r\n"
            "\r\n"
            "Subject: inner\r\n"
            "\r\n"
            "\xc3\xa4" "ight bit\r\n");

         if (SevenBitConverter::Convert(*body))
            return false;

         return AnsiString(body->GetTransferEncoding()) == "8bit";
      }

      bool TestSevenBitConverterLeavesNonMimeBodyAlone()
      {
         // No Content-Type and no Content-Transfer-Encoding: nothing marks this body as
         // MIME, so quoted-printable would reach the reader as literal "=C3=A4" text.
         std::shared_ptr<MimeBody> body = LoadBody(
            "Subject: no mime headers at all\r\n"
            "\r\n"
            "\xc3\xa4" "ight bit\r\n");

         if (SevenBitConverter::Convert(*body))
            return false;

         return SevenBitConverter::ContainsEightBitOctets(body->GetContent());
      }

      bool TestSevenBitConverterConvertsPartWithContentTransferEncodingOnly()
      {
         // No Content-Type, but Content-Transfer-Encoding marks the body as MIME content,
         // and the media type then defaults to text/plain.
         std::shared_ptr<MimeBody> body = LoadBody(
            "Content-Transfer-Encoding: 8bit\r\n"
            "\r\n"
            "\xc3\xa4" "ight bit\r\n");

         if (!SevenBitConverter::Convert(*body))
            return false;

         if (AnsiString(body->GetTransferEncoding()) != "quoted-printable")
            return false;

         return !SevenBitConverter::ContainsEightBitOctets(body->GetContent());
      }

      bool TestSevenBitConverterLeavesDeclaredBase64Alone()
      {
         // A part declaring base64 that still holds 8-bit octets is malformed. Its content
         // is not the raw bytes, so re-encoding it would change what it decodes to.
         std::shared_ptr<MimeBody> body = LoadBody(
            "Content-Type: application/octet-stream\r\n"
            "Content-Transfer-Encoding: base64\r\n"
            "\r\n"
            "aGVsbG8=\xc3\xa4\r\n");

         if (SevenBitConverter::Convert(*body))
            return false;

         return AnsiString(body->GetTransferEncoding()) == "base64";
      }

      bool TestSevenBitConverterTreatsAbsentEncodingAsIdentity()
      {
         // RFC 2045 section 6.1: an absent Content-Transfer-Encoding means 7bit, so the
         // content is the raw octets and an 8-bit body here is convertible.
         std::shared_ptr<MimeBody> body = LoadBody(
            "Content-Type: text/plain; charset=\"utf-8\"\r\n"
            "\r\n"
            "\xc3\xa4" "ight bit\r\n");

         if (!SevenBitConverter::Convert(*body))
            return false;

         return AnsiString(body->GetTransferEncoding()) == "quoted-printable";
      }
   }

   MimeTester::MimeTester(void)
   {
   }

   MimeTester::~MimeTester(void)
   {
   }
   
   void
   MimeTester::Test()
   {
     if (!TestFindStringEdgeCases())
         throw;

      if (!TestMultipartWithoutFinalCrlf())
         throw;

      if (!TestMultipartWithFinalCrlf())
         throw;

      if (!TestMultipartWithoutClosingBoundary())
         throw;

      if (!TestMultipartWithClosingBoundaryMissingSeparator())
         throw;

      if (!TestMultipartWithPartBoundaryMissingCrlf())
         throw;

      if (!TestQPEncodeEmpty())
         throw;

      if (!TestQPEncodeSinglePrintableChar())
         throw;

      if (!TestQPEncodeEqualsSign())
         throw;

      if (!TestQPEncodeNonAscii())
         throw;

      if (!TestQPEncodeCRLF())
         throw;

      if (!TestQPEncodeBareLF())
         throw;

      if (!TestQPEncodeTrailingSpace())
         throw;

      if (!TestQPEncodeTrailingTab())
         throw;

      if (!TestQPEncodeSpaceBeforeHardBreak())
         throw;

      if (!TestQPEncodeTabBeforeHardBreak())
         throw;

      if (!TestQPEncodeSmtpDotQuoted())
         throw;

      if (!TestQPEncodeDotNotQuotedMidLine())
         throw;

      if (!TestQPEncodeDotAtEndOfInputNotQuoted())
         throw;

      if (!TestQPEncode75CharsNoSoftBreak())
         throw;

      if (!TestQPEncode76CharsTriggersSoftBreak())
         throw;

      if (!TestQPEncodeNoTrailingWhitespaceBeforeSoftBreakHighCostNextChar())
         throw;

      if (!TestQPEncodeNoTrailingWhitespaceBeforeSoftBreak())
         throw;
      if (!TestGetParameterWithQuotedSemicolon())
         throw;

      if (!TestGetParameterNeighboringParamUnaffectedByQuotedSemicolon())
         throw;

      if (!TestRemoveParameterRemovesSimpleParam())
         throw;

      if (!TestRemoveParameterRemovesRfc2231Continuations())
         throw;

      if (!TestSetFileNameReplacesRfc2231ContinuationFilename())
         throw;

      if (!TestSetFileNameReplacesEncodedFilenameVariant())
         throw;

      if (!TestSetFileNameReplacesContentTypeNameVariant())
         throw;

      if (!TestRfc2231ApostropheInUnquotedValue())
         throw;

      if (!TestContainsEightBitOctets())
         throw;

      if (!TestSevenBitConverterLeavesSevenBitTextAlone())
         throw;

      if (!TestSevenBitConverterEncodesEightBitTextAsQuotedPrintable())
         throw;

      if (!TestSevenBitConverterEncodesEightBitBinaryAsBase64())
         throw;

      if (!TestSevenBitConverterConvertsPartsOfMultipart())
         throw;

      if (!TestSevenBitConverterConvertsBareMultipartPart())
         throw;

      if (!TestSevenBitConverterLeavesMessageRfc822Alone())
         throw;

      if (!TestSevenBitConverterLeavesNonMimeBodyAlone())
         throw;

      if (!TestSevenBitConverterConvertsPartWithContentTransferEncodingOnly())
         throw;

      if (!TestSevenBitConverterLeavesDeclaredBase64Alone())
         throw;

      if (!TestSevenBitConverterTreatsAbsentEncodingAsIdentity())
         throw;

   }
}
