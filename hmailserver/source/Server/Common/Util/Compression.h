// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Compression
   {
   public:
      Compression();
      ~Compression(void);

      bool AddDirectory(const String &zipFile, const String &directoryToAdd);
      bool AddFile(const String &zipFile, const String &fileToAdd);

      bool Uncompress(const String &zipFile, const String &targetDirectory);
      bool Uncompress(const String &zipFile, const String &targetDirectory, const String &wildCard);

   private:
 
      bool _LaunchCommand(const String &commandLine);
      String _GetExecutableFullPath();

   };
}