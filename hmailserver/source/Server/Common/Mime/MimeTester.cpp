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
   
   bool
   MimeTester::Test()
   {
      if (!TestFindStringEdgeCases())
         return false;

      if (!TestMultipartWithoutFinalCrlf())
         return false;

      if (!TestMultipartWithFinalCrlf())
         return false;

      if (!TestMultipartWithoutClosingBoundary())
         return false;

      if (!TestMultipartWithClosingBoundaryMissingSeparator())
         return false;

      if (!TestMultipartWithPartBoundaryMissingCrlf())
         return false;

      return true;
   }



   bool 
   MimeTester::TestFolder(const String &sFolderName)
   {
      return true;

      String sCleanFolder = sFolderName;
      if (sCleanFolder.Right(1) == _T("\\"))
         sCleanFolder = sCleanFolder.Left(sCleanFolder.GetLength() - 1);

      if (sCleanFolder.Right(1) != _T("\\"))
         sCleanFolder += "\\";

      String sWildCard = sCleanFolder + "*.*";

      // Locate first match
      WIN32_FIND_DATA ffData;
      HANDLE hFileFound = FindFirstFile(sWildCard, &ffData);

      if (hFileFound == INVALID_HANDLE_VALUE)
         return FALSE;

      while (hFileFound && FindNextFile(hFileFound, &ffData))
      {
         String sFullPath = sCleanFolder + ffData.cFileName;

         if (ffData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
         {
            if( (_tcscmp(ffData.cFileName, _T(".")) != 0) &&
               (_tcscmp(ffData.cFileName, _T("..")) != 0) ) 
            {
               if( !TestFolder(sFullPath) )
                  return false;
            }

         }
         else
         { 
            TestLoadFile(sFullPath);
         }
      }

      FindClose(hFileFound);

      return true;
   
   }

   void 
   MimeTester::TestLoadFile(const String &sFilename)
   {
	   try
	   {
         OutputDebugString("Loading file " + sFilename +"\n");

		   std::shared_ptr<Message> pMessage = std::shared_ptr<Message>(new Message(false));

		   std::shared_ptr<MessageData> pMsgData = std::shared_ptr<MessageData>(new MessageData());
		   pMsgData->LoadFromMessage(sFilename, pMessage);
	   }
	   catch (...)
	   {
		   assert(0);
		   MessageBox(0,_T("ERROR"), _T("ERROR"), 0);
		   throw;
	   }
   }

   void 
   MimeTester::TestFile(const String &sFilename)
   {
      try
      {
         std::shared_ptr<Message> pMessage = std::shared_ptr<Message>(new Message(false));
         
         std::shared_ptr<MessageData> pMsgData = std::shared_ptr<MessageData>(new MessageData());
         pMsgData->LoadFromMessage(sFilename, pMessage);

         String sOutput = "hMailServer: [MimeTester] --> "; 
         sOutput += sFilename + " --> "; 
         sOutput += pMsgData->GetSubject();
         sOutput += "\n";
         OutputDebugString(sOutput);

         // Add a message header
         pMsgData->SetFieldValue("X-MyHeader", "ValueOfMyHeader");

         // New message
         std::shared_ptr<Message> pNewMessage = std::shared_ptr<Message>(new Message());
         std::shared_ptr<Account> account;
         String newFileName = PersistentMessage::GetFileName(account, pNewMessage);

         pMsgData->Write(newFileName);

         // Delete the new message.
         FileUtilities::DeleteFile(newFileName);

      }
      catch (...)
      {
         assert(0);
         MessageBox(0,_T("ERROR"), _T("ERROR"), 0);
         throw;
      }
   }

}
