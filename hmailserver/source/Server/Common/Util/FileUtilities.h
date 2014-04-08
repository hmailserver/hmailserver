// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class FileUtilities
   {
   public:
      FileUtilities(void);
      ~FileUtilities(void);

      static const String PathSeparator;

      static String GetFilePath(const String &FileName);
      static String GetFileNameFromFullPath(const String & sFullPath);

      static bool DeleteFile(const String &FileName);

      static bool ReadLine(HANDLE hFile, String &sLine);
      static bool Copy(const String &sFrom, const String &sTo, bool bCreateMissingDirectories = false);
      static bool Move(const String &sFrom, const String &sTo, bool overwrite = false);
      static bool Exists(const String &sFilename);

      static void ReadFileToBuf(const String &sFilename, BYTE *Buf, int iStart = -1, int iCount = -1);
      static void ReadFileToBuf(HANDLE hFile, char *Buf, int iStart = -1, int iCount = -1);
      static String ReadCompleteTextFile(const String &sFilename);

      static bool WriteToFile(const String &sFilename, String &sData, bool bUnicode);
      static bool WriteToFile(const String &sFilename, AnsiString &sData);

      static long FileSize(const String &sFileName);

      static String GetTempFileName();
      static bool CreateDirectory(const String &sName);

      static String  GetShortPath(const String &sInPath);
      static String  GetLongPath(const String &sInPath);

      static bool CopyDirectory(String sFrom, String sTo, String &errorMessage);
      static bool DeleteDirectory(const String &sDirName);
      static bool DeleteFilesInDirectory(const String &sDirName);
      static bool DeleteDirectoriesInDirectory(const String &sDirName, const std::set<String> vecExcludes);

      static vector<String> GetFilesInDirectory(const String &sDirectoryName);

      static bool CreateDirectoryRecursive(const String &sDirName);

      static bool IsUNCPath(const String &sPath);
      static bool IsValidUNCFolder(const String &sPath);
      static bool IsFullPath(const String &sPath);

      static String Combine(const String &path1, const String &path2);

   private:

      
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