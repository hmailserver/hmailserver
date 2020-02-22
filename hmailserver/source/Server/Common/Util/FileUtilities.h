// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class FileInfo;

   class FileUtilities
   {
   public:
      FileUtilities(void);
      ~FileUtilities(void);

      static const String PathSeparator;

      static String GetFilePath(const String &FileName);
      static String GetFileNameFromFullPath(const String & sFullPath);

      static bool DeleteFile(const String &FileName);

      //static bool ReadLine(HANDLE hFile, String &sLine);
      static bool Copy(const String &sFrom, const String &sTo, bool bCreateMissingDirectories = false);
      static bool Move(const String &sFrom, const String &sTo, bool overwrite = false);
      static bool Exists(const String &sFilename);

      static void ReadFileToBuf(const String &sFilename, BYTE *Buf, int iStart = -1, int iCount = -1);
      static String ReadCompleteTextFile(const String &sFilename);

      static bool WriteToFile(const String &sFilename, const String &sData, bool bUnicode);
      static bool WriteToFile(const String &sFilename, const AnsiString &sData);

      static long FileSize(const String &sFileName);

      static String GetTempFileName();
      static bool CreateDirectory(const String &sName);

      static bool CopyDirectory(String sFrom, String sTo, String &errorMessage);
      static bool DeleteDirectory(const String &sDirName, bool force);
      static bool DeleteFilesInDirectory(const String &sDirName);
      static bool DeleteDirectoriesInDirectory(const String &sDirName);

      static std::vector<FileInfo> GetFilesInDirectory(const String &sDirectoryName, const String &regularExpressionTest);
      static bool GetDirectoryContainsFileRecursive(const String &sDirectoryName);
      static bool IsUNCPath(const String &sPath);
      static bool IsValidUNCFolder(const String &sPath);
      static bool IsFullPath(const String &sPath);

      static String Combine(const String &path1, const String &path2);

   private:

      enum FileEncoding
      {
         ANSI = 1,
         UTF8 = 2,
         UTF16 = 3
      };

      
   };

   class FileUtilitiesTester
   {
   private:

   public:

      FileUtilitiesTester() {};
      virtual ~FileUtilitiesTester() {} ;

      void Test();
   private:


   };
}