// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "FileTester.h"
#include "ByteBuffer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   void
   FileTester::Test()
   {
      OpenNonexistantFileShouldThrow();
      OpeningFileOnNonexistantDiskShouldThrow();

      WritingWithoutOpeningFileShouldThrow();
      WritingToFileOpenedInReadOnlyShouldThrow();

      ReadingWithoutOpeningFileShouldThrow();

      
   }

   void
   FileTester::OpenNonexistantFileShouldThrow()
   {
      try
      {
         File file;
         file.Open("C:\\Temp\\NonexistantFile", File::OTReadOnly);

         throw new int;
      }
      catch (std::runtime_error)
      {
         // expected
      }
   }

   void
   FileTester::WritingWithoutOpeningFileShouldThrow()
   {
      try
      {
         File file;
   
         file.Write(String("A"));

         throw new int;
      }
      catch (std::logic_error)
      {
         // expected
      }
   }

   void
   FileTester::ReadingWithoutOpeningFileShouldThrow()
   {
      try
      {
         File file;
         file.ReadChunk(10);

         throw new int;
      }
      catch (std::logic_error)
      {
         // expected
      }
   }

   void
   FileTester::OpeningFileOnNonexistantDiskShouldThrow()
   {
      try
      {
         File file;
         file.Open("U:\\test.txt", File::OTReadOnly);

         throw new int;
      }
      catch (std::runtime_error)
      {
         // expected
      }
   }

   void
   FileTester::WritingToFileOpenedInReadOnlyShouldThrow()
   {
      String file_name = FileUtilities::GetTempFileName();

      try
      {
         
         FileUtilities::WriteToFile(file_name, "Hello", false);

         File file;
         file.Open(file_name, File::OTReadOnly);

         file.Write(String("Test"));

         throw new int;

      }
      catch (std::runtime_error)
      {
         FileUtilities::DeleteFile(file_name);
         // expected
      }
   }

   void 
   FileTester::WritingAndReading100MBBufferShouldSucceed()
   {
      String file_name = FileUtilities::GetTempFileName();

      File file;
      file.Open(file_name, File::OTAppend);

      std::shared_ptr<ByteBuffer> large_buffer = std::make_shared<ByteBuffer>();
      file.Write(large_buffer);
         
      FileUtilities::DeleteFile(file_name);
   }
}
