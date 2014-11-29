// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class FileTester
   {
   public:
      void Test();
   private:

      void OpenNonexistantFileShouldThrow();
      void OpeningFileOnNonexistantDiskShouldThrow();

      void WritingWithoutOpeningFileShouldThrow();
      void WritingToFileOpenedInReadOnlyShouldThrow();
      void WritingAndReading100MBBufferShouldSucceed();

      void ReadingWithoutOpeningFileShouldThrow();
   };
}