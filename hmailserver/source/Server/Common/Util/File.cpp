// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "File.h"
#include "ByteBuffer.h"

#include <boost/filesystem.hpp>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   File::File() :
      file_(nullptr)
   {
      
   }

   File::~File()
   {
      Close();
   }
   
   void 
   File::Close()
   {
      if (file_ != nullptr)
      {
         fclose(file_);
         file_ = nullptr;
      }

   }

   bool 
   File::IsOpen() const
   {
      if (file_ == nullptr)
         return false;
      
      return true;
   }

   bool
   File::Open(const String &sFilename, OpenType ot)
   {
      if (IsOpen())
      {
         // The file should be closed, before we
         // try to open it again...
         throw std::logic_error(Formatter::FormatAsAnsi("The file {0} is already open.", sFilename));
      }

      std::wstring open_mode;

      switch (ot)
      {
      case OTReadOnly:
         open_mode = _T("rb");
         break;
      case OTCreate:
         open_mode = _T("wb");
         break;
      case OTAppend:
         open_mode = _T("ab");
         break;
      }

      file_ = _wfsopen(sFilename.c_str(), open_mode.c_str(), _SH_DENYNO);

      if (file_ == nullptr)
      {
         return false;
      }

      name_ = sFilename;

      return true;
   }

   int 
   File::GetSize()
   {
      if (file_ == INVALID_HANDLE_VALUE)
         return 0;

      return (int) boost::filesystem::file_size(name_);
   }

   bool
   File::SetPosition(int position)
   {
      if (file_ == nullptr)
         throw std::logic_error("Attempt to set position on file which has not been opened.");

      return fseek(file_, position, 0) == 0;
   }

   bool 
   File::Write(const String &sWrite)
   {
      if (file_ == nullptr)
         throw std::logic_error("Attempt to write to file which has not been opened.");

      String temp_nonconst = sWrite;

      int result = fwrite(temp_nonconst.GetBuffer(), sizeof(TCHAR), temp_nonconst.GetLength(), file_);

      return result == temp_nonconst.GetLength();
   }

   bool 
   File::Write(const AnsiString &sWrite)
   {  
      if (file_ == nullptr)
         throw std::logic_error("Attempt to write to file which has not been opened.");

      AnsiString temp_nonconst = sWrite;
      int result = fwrite(temp_nonconst.GetBuffer(), sizeof(char), temp_nonconst.GetLength(), file_);
      return result == temp_nonconst.GetLength();
   }

   bool 
   File::Write(const unsigned char *pBuf, int iBufLen, DWORD &dwNoOfBytesWritten)
   {
      if (file_ == nullptr)
         throw std::logic_error("Attempt to write to file which has not been opened.");

      dwNoOfBytesWritten = fwrite(pBuf, 1, iBufLen, file_);
      return dwNoOfBytesWritten == iBufLen;
   }

   bool 
   File::Write(std::shared_ptr<ByteBuffer> pBuffer, DWORD &dwNoOfBytesWritten)
   {
      return Write((const unsigned char*) pBuffer->GetCharBuffer(), pBuffer->GetSize(), dwNoOfBytesWritten);
   }

   bool 
   File::WriteBOF()
   {
      // Write unicode beginner markers.
      unsigned char charByteOrderMarker[2] = {-1, -2};

      DWORD dwWritten = 0;
      Write(charByteOrderMarker, 2, dwWritten);

      if (dwWritten != 2)
      {
         // Failed to write BOM.
         return false;
      }

      return true;
   }

   bool 
   File::ReadLine(AnsiString &sLine)
   {
      if (file_ == nullptr)
         throw std::logic_error("Attempt to read to file which has not been opened.");

      const int buffer_size = 10000;
      char *line_buffer = sLine.GetBuffer(buffer_size);
      
      bool result = fgets(line_buffer, buffer_size, file_) != 0;

      sLine.ReleaseBuffer();

      return result;
   }

   std::shared_ptr<ByteBuffer> 
   File::ReadFile()
   {
      try
      {
         std::shared_ptr<ByteBuffer> pFileContents = std::shared_ptr<ByteBuffer>(new ByteBuffer);

         if (file_ == nullptr)
            throw std::logic_error("Attempt to read from file which has not been opened.");

         int iFileSize = GetSize();

         // Create a buffer to hold the file
         pFileContents->Allocate(iFileSize);

         int bytes_read = fread((void*) pFileContents->GetBuffer(), 1, iFileSize, file_);

         if (bytes_read != iFileSize)
         {
            throw std::logic_error(Formatter::FormatAsAnsi("Unable to read file {0}. Expected bytes: {1}, Actual read bytes: {2}.", name_, iFileSize, bytes_read));
         }

         return pFileContents;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4227, "File::ReadFile", "An unknown error occurred while reading file from disk.");
         throw;
      }

      std::shared_ptr<ByteBuffer> pFileContents = std::shared_ptr<ByteBuffer>(new ByteBuffer);
      return pFileContents;
   }

   std::shared_ptr<ByteBuffer> 
   File::ReadTextFile()
   {
      try
      {
         std::shared_ptr<ByteBuffer> pFileContents = ReadFile();

         const BYTE pByte = '\0';
         pFileContents->Add(&pByte, 1);
         return pFileContents;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5042, "File::ReadTextFile", "An unknown error occurred while reading a text file from disk.");
         throw;
      }

      std::shared_ptr<ByteBuffer> pFileContents = std::shared_ptr<ByteBuffer>(new ByteBuffer);
      return pFileContents;
   }

   std::shared_ptr<ByteBuffer> 
   File::ReadChunk(int iMaxSize)
   {  
      if (file_ == nullptr)
         throw std::logic_error("Attempt to read from file which has not been opened.");

      try
      {
         std::shared_ptr<ByteBuffer> pReadBuffer = std::shared_ptr<ByteBuffer>(new ByteBuffer);
         pReadBuffer->Allocate(iMaxSize);

         // Read
         int bytes_read = fread((void*)pReadBuffer->GetBuffer(), 1, iMaxSize, file_);

         if (bytes_read > 0)
         {
            std::shared_ptr<ByteBuffer> pRetBuffer = std::shared_ptr<ByteBuffer>(new ByteBuffer);
            pRetBuffer->Add(pReadBuffer->GetBuffer(), bytes_read);
            return pRetBuffer;
         }
         
      }
      catch (...)
      {
         throw;   
      }

      std::shared_ptr<ByteBuffer> pBuffer;
      return pBuffer;
   }

   String 
   File::GetName() const
   {
      return name_;
   }

   bool 
   File::Write(File &sourceFile)
   {
      while (std::shared_ptr<ByteBuffer> sourceData = sourceFile.ReadChunk(FileChunkSize))
      {
         DWORD dummy;
         if (!Write(sourceData, dummy))
            return false;

         if (dummy != sourceData->GetSize())
            return false;
      }

      return true;
   }

}