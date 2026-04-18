// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "MimeTester.h"
#include "Mime.h"
#include "MimeCode.h"
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
   }
}
