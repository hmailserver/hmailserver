// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Compression.h"
#include "ProcessLauncher.h"

#include <boost/thread/thread.hpp>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   Compression::Compression()
   {
      
   }

   Compression::~Compression(void)
   {

   }

   bool
   Compression::AddDirectory(const String &zipFile, const String &directoryToAdd)
   {
      // -r = recurse -t = type 7z -mmt = number of threads -mx1 = lowest compression
      String commandLine = Formatter::Format("\"{0}\" a \"{1}\" \"{2}\" -r -t7z {3} -mx1  -w\"{4}\"",
         GetExecutableFullPath_(), zipFile, directoryToAdd, GetMultithreadingParameter_(), IniFileSettings::Instance()->GetTempDirectory());

      return LaunchCommand_(commandLine);
   }

   bool
   Compression::AddFile(const String &zipFile, const String &fileToAdd)
   {
      // -t = type 7z -mmt = number of threads -mx1 = lowest compression
      String commandLine = Formatter::Format("\"{0}\" a \"{1}\" \"{2}\" -t7z {3} -mx1 -w\"{4}\"",
         GetExecutableFullPath_(), zipFile, fileToAdd, GetMultithreadingParameter_(), IniFileSettings::Instance()->GetTempDirectory());

      return LaunchCommand_(commandLine);
   }

   bool
   Compression::Uncompress(const String &zipFile, const String &targetDirectory)
   {
      return Uncompress(zipFile, targetDirectory, "*");
   }

   bool
   Compression::Uncompress(const String &zipFile, const String &targetDirectory, const String &wildCard)
   {
      String commandLine = Formatter::Format("\"{0}\" x \"{1}\" \"{2}\" -o\"{3}\" -y", 
         GetExecutableFullPath_(), zipFile, wildCard, targetDirectory);

      return LaunchCommand_(commandLine);
   }

   bool 
   Compression::LaunchCommand_(const String &commandLine)
   {
      unsigned int exitCode = 0;
      ProcessLauncher processLauncher(commandLine);

      if (!processLauncher.Launch(exitCode))
         return false;

      if (exitCode != 0 && exitCode != 1)
         return false;

      return true;
   }

   String
   Compression::GetMultithreadingParameter_()
   {
      // Backups run while the server is processing email, so compression is given at most half of
      // the logical processors rather than all of them.
      unsigned int processorCount = boost::thread::hardware_concurrency();
      unsigned int threadCount = processorCount / 2;

      if (threadCount < 2)
         return "-mmt=off";

      return Formatter::Format("-mmt={0}", threadCount);
   }

   String 
   Compression::GetExecutableFullPath_()
   {
      const String ZipExecutable = "7za.exe";

      String binDir = IniFileSettings::Instance()->GetBinDirectory();

      return FileUtilities::Combine(binDir, ZipExecutable);
   }
}