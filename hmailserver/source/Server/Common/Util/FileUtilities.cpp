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
#include "../Util/RegularExpression.h"

#include <boost/filesystem.hpp>

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
      const int iMaxNumberOfTries = 5;

      const int FILE_NOT_FOUND = 2;
      const int PATH_NOT_FOUND = 3;

      for (int i = 1; i <= iMaxNumberOfTries; i++)
      {
         AnsiString sFilename = FileName;

         boost::system::error_code error_code;

         boost::filesystem::path path(sFilename.begin(), sFilename.end());

         if (!boost::filesystem::remove(path, error_code))
         {
            // file did not exist.
            return true;
         }

         if (error_code)
         {
            // We failed to delete the file. 

            if (i == iMaxNumberOfTries)
            {
               // We still couldn't delete the file. Lets give up and report in windows event log.
               String sErrorMessage;
               sErrorMessage.Format(_T("Could not delete the file %s. Tried 5 times without success."), FileName.c_str());
               ErrorManager::Instance()->ReportError(ErrorManager::High, 5047, "File::DeleteFile", sErrorMessage, error_code);
               return false;
            }

            // Some other process must have locked the file.
            Sleep(1000);
         }
         else
         {
            // file deleted.
            return true;
         }
      }

      return false;
   }

   bool
   FileUtilities::Copy(const String &sFrom, const String &sTo, bool bCreateMissingDirectories)
   {
      const int iMaxNumberOfTries = 5;

      if (bCreateMissingDirectories)
      {
         String sToPath = sTo.Mid(0, sTo.ReverseFind(_T("\\")));
         CreateDirectory(sToPath);
      }

      for (int i = 1; i <= iMaxNumberOfTries; i++)
      {
         boost::system::error_code error_code;
         boost::filesystem::copy_file(sFrom, sTo, boost::filesystem::copy_option::overwrite_if_exists, error_code);

         // Use classic api to copy the file
         if (!error_code)
         {
            //Copy OK
            return true;
         }

         // We failed to delete the file. 

         if (i == iMaxNumberOfTries)
         {
            // We still couldn't copy the file. Lets give up and report in windows event log and hMailServer application log
            String sErrorMessage;
            sErrorMessage.Format(_T("Could not copy the file %s to %s. Tried 5 times without success."), sFrom.c_str(), sTo.c_str());
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5048, "File::Copy", sErrorMessage, error_code);
            return false;
         }

         // Some other process must have locked the file.
         Sleep(1000);
      }

      throw std::logic_error("Copy file logic error.");
   }

   bool
   FileUtilities::Move(const String &sFrom, const String &sTo, bool overwrite)
   {
      const int iMaxNumberOfTries = 5;

      if (overwrite)
         DeleteFile(sTo);

      for (int i = 1; i <= iMaxNumberOfTries; i++)
      {
         boost::system::error_code error_code;
         boost::filesystem::rename(sFrom, sTo, error_code);

         if (!error_code)
            return true;

         if (i == iMaxNumberOfTries)
         {
            // We still couldn't move the file. Lets give up and report in windows event log and hMailServer application log.
            int iLastError = ::GetLastError();

            String sErrorMessage;
            sErrorMessage.Format(_T("Could not move the file %s to %s. Tried 5 times without success."), sFrom.c_str(), sTo.c_str());
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5049, "File::Normal", sErrorMessage, error_code);

            return false;
         }

         // Some other process must have locked the file.
         Sleep(250);
      }

      throw std::logic_error("Move file logic error.");
   }

   bool
   FileUtilities::Exists(const String &sFilename)
   {
      return boost::filesystem::exists(sFilename);
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

   String
   FileUtilities::ReadCompleteTextFile(const String &sFilename)
   {
      File oFile;
      if (!oFile.Open(sFilename, File::OTReadOnly))
         return "";

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
            size_t iChars = pBuffer->GetSize() / sizeof(TCHAR);
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
      File file;
      if (!file.Open(sFilename, File::OTReadOnly))
      {
         throw std::logic_error(Formatter::FormatAsAnsi("Unable to open file {0}", sFilename));
      }

      file.SetPosition(iStart);

      std::shared_ptr<ByteBuffer> bytes = file.ReadChunk(iCount);

      memcpy(OutBuf, bytes->GetBuffer(), iCount);

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
      boost::system::error_code error_code;
      int result = (int)boost::filesystem::file_size(sFileName, error_code);

      if (error_code)
         return 0;

      return result;
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
      const int iMaxNumberOfTries = 5;

      for (int i = 1; i <= iMaxNumberOfTries; i++)
      {
         boost::system::error_code error_code;

         boost::filesystem::create_directories(sName, error_code);
         
         if (!error_code)
            return true;
         
         if (i == iMaxNumberOfTries)
         {
            // We still couldn't create the directory. Lets give up and report in windows event log and hMailServer application log.
            String sErrorMessage;
            sErrorMessage.Format(_T("Could not create the directory %s. Tried 5 times without success."), sName.c_str());

            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5050, "File::CreateDirectory", sErrorMessage, error_code);
            return false;
         }

         // Some other process must have locked the file.
         Sleep(250);

      }

      throw std::logic_error("Create directory logic error.");
   }

   bool 
   FileUtilities::CopyDirectory(String sFrom, String sTo, String &errorMessage)
   {
      // Check whether the function call is valid
      if (!boost::filesystem::exists(sFrom) || !boost::filesystem::is_directory(sFrom))
      {
         throw std::logic_error(Formatter::FormatAsAnsi("Source {0} is not a valid directory.", sFrom));
      }

      if (!boost::filesystem::exists(sTo))
      {
         if (!CreateDirectory(sTo))
            return false;
      }


      for (boost::filesystem::directory_iterator file(sFrom); file != boost::filesystem::directory_iterator(); ++file )
      {
         boost::filesystem::path current(file->path());
         if (boost::filesystem::is_directory(current))
         {
            if (!CopyDirectory(current.c_str(), (sTo / current.filename()).c_str(), errorMessage))
            {
               return false;
            }
         }
         else
         {
            boost::filesystem::copy_file(current, sTo / current.filename());
         }
      }

      return true;
   }

   bool 
   FileUtilities::DeleteDirectory(const String &sDirName, bool force)
   {
      if (!force)
      {
         if (!boost::filesystem::is_directory(sDirName))
         {
            // The directory is already gone.
            return true;
         }

         if (GetDirectoryContainsFileRecursive(sDirName))
         {
            // Directory is not empty.
            return false;
         }
      }

      boost::system::error_code error_code;
      boost::filesystem::remove_all(sDirName, error_code);

      if (error_code)
         return false;
      
      return true;
   }

   bool
   FileUtilities::DeleteFilesInDirectory(const String &sDirName)
   {
      for (boost::filesystem::directory_iterator file(sDirName); file != boost::filesystem::directory_iterator(); ++file)
      {
         boost::filesystem::path current(file->path());
         if (!boost::filesystem::is_directory(current))
            boost::filesystem::remove(current);
      }

      return true;
   }


   std::vector<FileInfo> 
   FileUtilities::GetFilesInDirectory(const String &sDirectoryName, const String &regularExpressionTest)
   {
      std::vector<FileInfo> result;

      for (boost::filesystem::directory_iterator file(sDirectoryName); file != boost::filesystem::directory_iterator(); ++file)
      {
         boost::filesystem::path current(file->path());

         if (RegularExpression::TestExactMatch(regularExpressionTest, current.filename().wstring()))
         {
            WIN32_FILE_ATTRIBUTE_DATA file_info;
            GetFileAttributesExW(current.wstring().c_str(), GetFileExInfoStandard, &file_info);

            result.push_back(FileInfo(current.filename().wstring(), file_info.ftCreationTime));
         }
      }


      return result;
      

   }

   bool
   FileUtilities::GetDirectoryContainsFileRecursive(const String &sDirectoryName)
   {
      if (!boost::filesystem::is_directory(sDirectoryName))
         return false;

      std::vector<FileInfo> result;

      for (boost::filesystem::directory_iterator file(sDirectoryName); file != boost::filesystem::directory_iterator(); ++file)
      {
         boost::filesystem::path current(file->path());

         if(boost::filesystem::is_directory(current))
         {
            auto result = GetDirectoryContainsFileRecursive(current.wstring().c_str());

            if (result)
               return true;
         }
         else
         {
            return true;
         }
      }

      return false;
   }


   bool
   FileUtilities::DeleteDirectoriesInDirectory(const String &sDirName)
   {
      for (boost::filesystem::directory_iterator file(sDirName); file != boost::filesystem::directory_iterator(); ++file)
      {
         boost::filesystem::path current(file->path());
         if (boost::filesystem::is_directory(current))
         {
            boost::system::error_code error_code;
            boost::filesystem::remove_all(current, error_code);

            if (error_code)
            {
               String sErrorMessage;
               sErrorMessage.Format(_T("Could not delete the directory %s."), file->path().string());
               ErrorManager::Instance()->ReportError(ErrorManager::High, 5700, "FileUtilities::DeleteDirectoriesInDirectory", sErrorMessage, error_code);
               return false;
            }
         }
      }

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