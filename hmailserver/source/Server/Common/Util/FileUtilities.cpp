// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\fileutilities.h"

#include "FileInfo.h"
#include "File.h"
#include "ByteBuffer.h"
#include "GUIDCreator.h"
#include "../Application/Dictionary.h"
#include "../Util/Assert.h"
#include "../Util/Unicode.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   const String FileUtilities::PathSeparator = _T("\\");

   FileUtilities::FileUtilities(void)
   {
   }

   FileUtilities::~FileUtilities(void)
   {
   }

   bool
   FileUtilities::DeleteFile(const String &FileName)
   {
      int iNumberOfTries = 0;
      const int iMaxNumberOfTries = 5;

      const int FILE_NOT_FOUND = 2;
      const int PATH_NOT_FOUND = 3;

      const int maxRecursions = 10000;
      for (int i = 0; i < maxRecursions; i++)
      {
         AnsiString sFilename = FileName;
         if (remove(sFilename) != -1)
            return true;

         int iLastError = ::GetLastError();
         if (iLastError == FILE_NOT_FOUND || iLastError == PATH_NOT_FOUND )
         {
            // We could not delete the file. But it doesn't exist, so it doens't matter.
            return true;
         }

         iNumberOfTries ++;

         // We failed to delete the file. 

         if (iNumberOfTries >= iMaxNumberOfTries)
         {
            // We still couldn't delete the file. Lets give up and report in windows event log.
            String sErrorMessage;
            sErrorMessage.Format(_T("Could not delete the file %s. Tried 5 times without success. Windows error code: %d (%s)"), FileName.c_str(), iLastError, Dictionary::GetWindowsErrorDescription(iLastError).c_str());

            ErrorManager::Instance()->ReportError(ErrorManager::High, 5047, "File::DeleteFile", sErrorMessage);
            return false;
         }

         // Some other process must have locked the file.
         Sleep(1000);
      }

      return false;
   }

   bool
   FileUtilities::Copy(const String &sFrom, const String &sTo, bool bCreateMissingDirectories)
   {
      int iNumberOfTries = 0;
      const int iMaxNumberOfTries = 5;

      if (bCreateMissingDirectories)
      {
         String sToPath = sTo.Mid(0, sTo.ReverseFind(_T("\\")));
         CreateDirectoryRecursive(sToPath);
      }


      const int maxRecursions = 10000;
      for (int i = 0; i < maxRecursions; i++)
      {
         // Use classic api to copy the file
         if (::CopyFile(sFrom, sTo, FALSE) != 0)
         {
            //Copy OK
            return true;
         }

         iNumberOfTries ++;

         // We failed to delete the file. 

         if (iNumberOfTries >= iMaxNumberOfTries)
         {
            // We still couldn't copy the file. Lets give up and report in windows event log and hMailServer application log
            int iLastError = ::GetLastError();

            String sErrorMessage;
            sErrorMessage.Format(_T("Could not copy the file %s to %s. Tried 5 times without success. Windows eror code: %d (%s)"), sFrom.c_str(), sTo.c_str(), iLastError, Dictionary::GetWindowsErrorDescription(iLastError).c_str());
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5048, "File::Copy", sErrorMessage);
            return false;
         }

         // Some other process must have locked the file.
         Sleep(1000);
      }

      assert(0); // if we get here, something is really strange...
      return false;
   }

   bool
   FileUtilities::Move(const String &sFrom, const String &sTo, bool overwrite)
   {
      int iNumberOfTries = 0;
      const int iMaxNumberOfTries = 5;

      const int maxRecursions = 10000;
      for (int i = 0; i < maxRecursions; i++)
      {
         if (overwrite)
         {
            if (::MoveFileEx(sFrom, sTo, MOVEFILE_REPLACE_EXISTING) != 0)
            {
               return true;
            }
         }
         else
         {
            if (::MoveFile(sFrom, sTo) != 0)
            {
               //Copy OK
               return true;
            }
         }

         iNumberOfTries ++;

         // We failed to delete the file. 

         if (iNumberOfTries >= iMaxNumberOfTries)
         {
            // We still couldn't move the file. Lets give up and report in windows event log and hMailServer application log.
            int iLastError = ::GetLastError();

            String sErrorMessage;
            sErrorMessage.Format(_T("Could not move the file %s to %s. Tried 5 times without success. Windows eror code: %d (%s)"), sFrom.c_str(), sTo.c_str(), iLastError, Dictionary::GetWindowsErrorDescription(iLastError).c_str());
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5049, "File::Normal", sErrorMessage);

            return false;
         }

         // Some other process must have locked the file.
         Sleep(250);
      }

      assert(0); // if we get here, something is really strange...
      return false;
   }

   bool
   FileUtilities::Exists(const String &sFilename)
   {
      DWORD dwAttr = GetFileAttributes(sFilename);
      if (dwAttr == INVALID_FILE_ATTRIBUTES)
         return false;
      else 
         return true;

   }

   String
   FileUtilities::GetFilePath(const String & FileName)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the path to the folder in which FileName resides.
   //---------------------------------------------------------------------------()
   {
      int iLastSlash = FileName.ReverseFind(_T("\\"));
      String Path = FileName.Mid(0, iLastSlash);

      return Path;
   }

   String
   FileUtilities::GetFileNameFromFullPath(const String & sFullPath)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the filename in the full path.
   //---------------------------------------------------------------------------()
   {
      int iLastSlash = sFullPath.ReverseFind(_T("\\"));

      if (iLastSlash == -1)
      {
         // Path is relative.
         return sFullPath;
      }

      return sFullPath.Mid(iLastSlash + 1);
   }

   bool
   FileUtilities::ReadLine(HANDLE hFile, String &sLine)
   {
      BYTE buf[2048];
      memset(buf, 0, 2048);

      unsigned long nbytes = 0;

      // Get current position in file.
      DWORD dwCurrentFilePosition = SetFilePointer( 
         hFile, // must have GENERIC_READ and/or GENERIC_WRITE 
         0,     // do not move pointer 
         NULL,  // hFile is not large enough to need this pointer 
         FILE_CURRENT);  // provides offset from current position    


      // read buffer from file.
      BOOL bMoreData = ReadFile(hFile,buf,2048, &nbytes, NULL);

      // Search for line end
      String sData((char*) buf);
      int iEndPos = sData.Find(_T("\r\n"), 0);

      if (iEndPos < 0)
      {
         // Couldn't find end of line. Assume that all 
         // the remaining data is on the current line.
         iEndPos = nbytes;
      }

      if (iEndPos >= 0)
      {
         sLine = sData.Mid(0, iEndPos);  
         iEndPos = iEndPos + 2;

      }

      // Set new position in file.
      SetFilePointer( 
         hFile, // must have GENERIC_READ and/or GENERIC_WRITE 
         iEndPos + dwCurrentFilePosition,     // do not move pointer 
         NULL,  // hFile is not large enough to need this pointer 
         FILE_BEGIN);  // provides offset from current position    

      if (bMoreData && nbytes > 0)
         return true;
      else
         return false;
   }

   String
   FileUtilities::ReadCompleteTextFile(const String &sFilename)
   {
      File oFile;
      oFile.Open(sFilename, File::OTReadOnly);

      // Read file
      std::shared_ptr<ByteBuffer> pBuffer = oFile.ReadFile();

      if (!pBuffer || pBuffer->GetSize() == 0)
      {
         // Could not read from this file.
         return "";
      }

      FileEncoding file_encoding = ANSI;

      bool is_utf8 = false;
      
      // check if utf8 bom exists
      const unsigned char *unsigned_char_buffer = (const unsigned char*) pBuffer->GetCharBuffer();

      if (pBuffer->GetSize() >= 3 &&
          *unsigned_char_buffer == 0xef && 
          *(unsigned_char_buffer + 1) == 0xbb &&
          *(unsigned_char_buffer + 2) == 0xbf)
          file_encoding = UTF8;
      else if (pBuffer->GetSize() >= 2 &&
         *unsigned_char_buffer == 0xff && 
         *(unsigned_char_buffer + 1) == 0xfe)
         file_encoding = UTF16;
      
      switch (file_encoding)
      {
      case ANSI:
         {
            AnsiString sRetVal((const char*) unsigned_char_buffer, pBuffer->GetSize());
            return sRetVal;
         }
      case UTF8:
         {
            AnsiString raw_data((const char*) unsigned_char_buffer, pBuffer->GetSize());

            String utf8_data;
            Unicode::MultiByteToWide(raw_data, utf8_data);

            return utf8_data;
         }
      case UTF16:
         {
            int iChars = pBuffer->GetSize() / sizeof(TCHAR);
            String sRetVal((const wchar_t*) pBuffer->GetCharBuffer() +1, iChars -1);
            return sRetVal;
         }
      default:
         throw std::logic_error(Formatter::FormatAsAnsi("Unsupported encoding type: {0}", file_encoding));
      }
   }
 
   void
   FileUtilities::ReadFileToBuf(const String &sFilename, BYTE *OutBuf, int iStart, int iCount)
   {
      // --- Open the file for writing.
      int iBefore = GetTickCount();
      HANDLE handleFile = CreateFile(sFilename, 
         GENERIC_READ, 
         FILE_SHARE_READ, 
         NULL, // LPSECURITY_ATTRIBUTES
         OPEN_EXISTING, // -- open or create.
         FILE_ATTRIBUTE_NORMAL, // attributes
         NULL // file template
         );

      if (handleFile == INVALID_HANDLE_VALUE) 
      { 
         // This is not good. We failed to get a handle to the file.
         return;
      } 

      ReadFileToBuf(handleFile, (char*) OutBuf, iStart, iCount);

      CloseHandle(handleFile);
   }

   void
   FileUtilities::ReadFileToBuf(HANDLE handleFile, char *OutBuf, int iStart, int iCount)
   {
      if (iStart == -1 && iCount == -1)
      {
         iStart = 0;
         iCount = GetFileSize (handleFile, NULL) ; 
      }

      unsigned long nBytesRead = 0;
      BOOL bMoreData = TRUE;

      if (iStart >= 0)
      {
         SetFilePointer(handleFile, iStart, 0, FILE_BEGIN);
      }

      bMoreData = ReadFile(handleFile,OutBuf, iCount, &nBytesRead, NULL);

   }

   bool 
   FileUtilities::WriteToFile(const String &sFilename, const String &sData, bool bUnicode)
   {
      File oFile;
      if (!oFile.Open(sFilename, File::OTCreate))
         return false;

      if (bUnicode)
      {
         if (!oFile.WriteBOF())
            return false;

         if (!oFile.Write(sData))
            return false;
      }
      else
      {
         // Enforce ANSI format.
         AnsiString sAnsi = sData;

         if (!oFile.Write(sAnsi))
            return false;
      }

      oFile.Close();

      return true;
   }

   bool 
   FileUtilities::WriteToFile(const String &sFilename, const AnsiString &sData)
   {
      File oFile;
      if (!oFile.Open(sFilename, File::OTCreate))
         return false;

      if (!oFile.Write(sData))
         return false;

      oFile.Close();

      return true;
   }

   long
   FileUtilities::FileSize(const String &sFileName)
   {
      HANDLE handleFile = CreateFile(sFileName, 
         GENERIC_READ, 
         FILE_SHARE_READ, 
         NULL, // LPSECURITY_ATTRIBUTES
         OPEN_EXISTING, // -- open or create.
         FILE_ATTRIBUTE_NORMAL, // attributes
         NULL // file template
         );

      if (handleFile == INVALID_HANDLE_VALUE) 
         return 0;

      long nFileSize = GetFileSize (handleFile, NULL) ; 

      CloseHandle(handleFile);

      return nFileSize;
   }

   String
   FileUtilities::GetTempFileName()
   {
      String sTmpFile;
      sTmpFile.Format(_T("%s\\%s.tmp"), IniFileSettings::Instance()->GetTempDirectory().c_str(), GUIDCreator::GetGUID().c_str());
      return sTmpFile;
   }

   bool
   FileUtilities::CreateDirectory(const String &sName)
   {
      int iNumberOfTries = 0;
      const int iMaxNumberOfTries = 5;

      int maxRecursions = 10000;
      for (int i = 0; i < maxRecursions; i++)
      {
         if (::CreateDirectory(sName, 0) != 0)
         {
            // Create Directory OK
            return true;
         }

         int iWinErr = ::GetLastError();

         if (iWinErr == ERROR_ALREADY_EXISTS)
         {
            // Create directory failed, because the directory exists. 
            // That's good enough for us.

            return true;
         }

         iNumberOfTries ++;

         // We failed to delete the file. 

         if (iNumberOfTries >= iMaxNumberOfTries)
         {
            // We still couldn't copy the file. Lets give up and report in windows event log and hMailServer application log.
            String sErrorMessage;
            sErrorMessage.Format(_T("Could not create the directory %s. Tried 5 times without success. Windows error code: %d (%s)"), sName.c_str(), iWinErr, Dictionary::GetWindowsErrorDescription(iWinErr).c_str());

            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5050, "File::CreateDirectory", sErrorMessage);
            return false;
         }

         // Some other process must have locked the file.
         Sleep(250);

      }

      assert(0);
      return false;
   }

   String
   FileUtilities::GetShortPath(const String &sInPath)
   {
      TCHAR szModuleShort[_MAX_PATH];
      GetShortPathName(sInPath, szModuleShort, _MAX_PATH );

      return szModuleShort;
   }

   String
   FileUtilities::GetLongPath(const String &sInPath)
   {
      TCHAR szLong[_MAX_PATH];
      GetLongPathName(sInPath, szLong, _MAX_PATH );

      return szLong;
   }


   bool 
   FileUtilities::CopyDirectory(String sFrom, String sTo, String &errorMessage)
   {
      if (!FileUtilities::Exists(sTo))
      {
         if( !CreateDirectory(sTo))
         {
            errorMessage = Formatter::Format(_T("CreateDirectory {0} failed. See previous error."), sTo);

            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4234, "File::CopyDirectory", errorMessage);
            return false;
         }
      }

      if (sFrom.Right(1) != _T("\\"))
         sFrom += "\\";
      if (sTo.Right(1) != _T("\\"))
         sTo += "\\";

      String sWildCard = sFrom + "*.*";

      // Locate first match
      WIN32_FIND_DATA ffData;
      HANDLE hFileFound = FindFirstFile(sWildCard, &ffData);

      if (hFileFound == INVALID_HANDLE_VALUE)
      {
         errorMessage.Format(_T("Find first file with wildcard %s failed. Error: %d."), sWildCard.c_str(), GetLastError());
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4233, "File::CopyDirectory", errorMessage);
         return false;
      }

      while (hFileFound && FindNextFile(hFileFound, &ffData))
      {
         String sOldFullPath = sFrom + ffData.cFileName;
         String sNewFullPath = sTo + ffData.cFileName;

         if (ffData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
         {
            if( (_tcscmp(ffData.cFileName, _T(".")) != 0) &&
               (_tcscmp(ffData.cFileName, _T("..")) != 0) ) 
            {
               if( !CopyDirectory(sOldFullPath, sNewFullPath, errorMessage) )
                  return false;
            }

         }
         else
         { 
            if (FileUtilities::Exists(sNewFullPath))
            {
               // File already exists
               continue;
            }

            if (CopyFile(sOldFullPath, sNewFullPath, TRUE))
            {
               // We have copied the file successfully
               continue;
            }

            // We failed to copy the file. Check if the file no 
            // longer exists
            if (!FileUtilities::Exists(sOldFullPath))
               continue;

            // The file exists , but we were not able to copy it.
            errorMessage.Format(_T("Copy of file from %s to %s failed. Error: %d"), sOldFullPath.c_str(), sNewFullPath.c_str(), GetLastError());
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4232, "File::CopyDirectory", errorMessage);
            return false;
         }
      }

      FindClose(hFileFound);

      return true;
   }

   bool 
   FileUtilities::DeleteDirectory(const String &sDirName)
   {
      TCHAR szSource[MAX_PATH + 2] = _T("");
      _tcsncpy_s(szSource, MAX_PATH + 2, sDirName, MAX_PATH);

      // szSource should be double null terminated. Otherwise it won't
      // work. At least not when using Unicode.
      szSource[sDirName.GetLength()+1] = 0;

      SHFILEOPSTRUCT fs;
      ::memset(&fs, 0, sizeof(SHFILEOPSTRUCT));

      fs.pFrom = szSource;
      fs.wFunc = FO_DELETE;
      fs.fFlags |= (FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT |FOF_NOERRORUI);

      int iResult = ::SHFileOperation(&fs);
      if (iResult != 0)
         return false;

      return true;
   }

   bool
   FileUtilities::DeleteFilesInDirectory(const String &sDirName)
   {
      String sDir = sDirName;
      if (sDir.Right(1) != _T("\\"))
         sDir += "\\";

      WIN32_FIND_DATA ffData;
      HANDLE hFileFound = FindFirstFile(sDir + "*.*", &ffData);

      if (hFileFound == INVALID_HANDLE_VALUE)
         return TRUE;

      while (hFileFound && FindNextFile(hFileFound, &ffData))
      {
         if (!(ffData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
         {
            String sFileName = sDir + ffData.cFileName;
            FileUtilities::DeleteFile(sFileName);
         }
      }

      FindClose(hFileFound);

      return true;
   }


   std::vector<FileInfo> 
   FileUtilities::GetFilesInDirectory(const String &sDirectoryName, const String &sWildcard)
   {
      std::vector<FileInfo> result;

      String file_name_wildcard = sDirectoryName;
      if (file_name_wildcard.Right(1) != _T("\\"))
         file_name_wildcard += "\\";

      file_name_wildcard+= sWildcard;

      WIN32_FIND_DATA find_file_data;
      HANDLE search_handle = FindFirstFile(file_name_wildcard, &find_file_data);

      while (search_handle != INVALID_HANDLE_VALUE)
      {
         if (!(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
         {
            result.push_back(FileInfo(find_file_data.cFileName, find_file_data.ftCreationTime));
         }

         if (!FindNextFile(search_handle, &find_file_data))
         {
            FindClose(search_handle);
            break;
         }
      }

      return result;
   }


   bool
   FileUtilities::DeleteDirectoriesInDirectory(const String &sDirName, const std::set<String> vecExcludes)
   {
      String sDir = sDirName;
      if (sDir.Right(1) != _T("\\"))
         sDir += "\\";

      WIN32_FIND_DATA ffData;
      HANDLE hFileFound = FindFirstFile(sDir + "*.*", &ffData);

      if (hFileFound == INVALID_HANDLE_VALUE)
         return TRUE;

      while (hFileFound && FindNextFile(hFileFound, &ffData))
      {
         if (ffData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
         {
            if( (_tcscmp(ffData.cFileName, _T(".")) != 0) &&
               (_tcscmp(ffData.cFileName, _T("..")) != 0) ) 
            {
               if (vecExcludes.find(ffData.cFileName) == vecExcludes.end())
               {
                  String sFileName = sDir + ffData.cFileName;
                  FileUtilities::DeleteDirectory(sFileName);
               }
            }
         }
      }

      FindClose(hFileFound);

      return true;
   }

   bool
   FileUtilities::CreateDirectoryRecursive(const String &sDirName)
   {
      if (FileUtilities::Exists(sDirName))
         return true;

      bool isUNCPath = IsUNCPath(sDirName);

      int iLength = sDirName.GetLength();
      for (int i = 3; i < iLength; i++)
      {
         wchar_t c = sDirName.GetAt(i);

         if (c == '\\')
         {
            String sDirectoryName = sDirName.Mid(0, i);
            
            if (isUNCPath)
            {
               // Have we specified share name?
               if (!IsValidUNCFolder(sDirectoryName))
               {
                  // No. Not much to do yet. We can't check for the
                  // existance of an UNC.
                  continue;
               }
            }
            
            if (FileUtilities::Exists(sDirectoryName))
               continue;

            if (!CreateDirectory(sDirectoryName))
               return false;
         }
      }

      if (!FileUtilities::Exists(sDirName))
         return CreateDirectory(sDirName);

      return true;
   }

   bool 
   FileUtilities::IsUNCPath(const String &sPath)
   {
      if (sPath.StartsWith(_T("\\\\")))
         return true;
      else
         return false;
   }

   bool 
   FileUtilities::IsFullPath(const String &sPath)
   {
      if (sPath.GetLength() < 2)
         return false;

      bool isFullPath = (sPath[1] == ':' ||
                        IsUNCPath(sPath));

      return isFullPath;


   }

   String 
   FileUtilities::Combine(const String &path1, const String &path2)
   {
      String firstHalf = path1;
      String secondHalf = path2;

      if (firstHalf.EndsWith(_T("\\")) || firstHalf.EndsWith(_T("/")))
         firstHalf = firstHalf.Mid(0, firstHalf.GetLength() -1);

      if (secondHalf.StartsWith(_T("\\")) || secondHalf.StartsWith(_T("/")))
         secondHalf = secondHalf.Mid(1);

      String result = firstHalf + "\\" + secondHalf;

      return result;
   }

   /// Returns true if the supplied UNC path contains a Share name.
   bool 
   FileUtilities::IsValidUNCFolder(const String &sPath)
   {
      if (!IsUNCPath(sPath))
         return false;

      // We have at least \\server

      int shareStartPos = sPath.Find(_T("\\"), 3);
      if (shareStartPos < 0)
         return false;

      // We have at least \\server\

      int folderStartPos = sPath.Find(_T("\\"), shareStartPos + 1);
      if (folderStartPos < 0)
         return false;

      // We have at least \\server\share\

      int length = sPath.GetLength();
      if (folderStartPos == length-1)
         return false;

      // We have something after \\server\share. That is a folder.
      return true;
   }

   void FileUtilitiesTester::Test()
   {  
      Assert::AreEqual("C:\\Temp", FileUtilities::Combine("C:\\", "Temp"));
      Assert::AreEqual("C:\\Temp", FileUtilities::Combine("C:\\", "\\Temp"));
      Assert::AreEqual("C:\\Temp", FileUtilities::Combine("C:", "\\Temp"));
      Assert::AreEqual("C:\\Temp", FileUtilities::Combine("C:", "Temp"));
      Assert::AreEqual("C:\\Temp", FileUtilities::Combine("C:/", "Temp"));

      Assert::IsTrue(FileUtilities::IsFullPath("C:\\"));
      Assert::IsTrue(FileUtilities::IsFullPath("C:/"));
      Assert::IsTrue(FileUtilities::IsFullPath("\\\\Test\\Monkey"));
      Assert::IsTrue(FileUtilities::IsFullPath("\\\\Test\\Monkey"));

      Assert::IsFalse(FileUtilities::IsFullPath("\\Test.eml"));
      Assert::IsFalse(FileUtilities::IsFullPath("Test.eml"));
      Assert::IsFalse(FileUtilities::IsFullPath("AB\\Data.eml"));
   }
}