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
      try
      {
         Close();
      }
      catch (...)
      {

      }
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

   void
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
         ThrowRuntimeError_(Formatter::FormatAsAnsi("Unable to open the file {0}.", sFilename));
      }

      name_ = sFilename;
   }

   int 
   File::GetSize()
   {
      if (file_ == nullptr)
         throw std::logic_error(Formatter::FormatAsAnsi("Unable to retrieve size of file {0}. The file is not open", name_));

      return (int) boost::filesystem::file_size(name_);
   }

   void
   File::SetPosition(int position)
   {
      if (file_ == nullptr)
         throw std::logic_error(Formatter::FormatAsAnsi("Unable to change position in file {0}. The file is not open", name_));

      if (fseek(file_, position, 0) != 0)
      {
         ThrowRuntimeError_(Formatter::FormatAsAnsi("Unable to change position in file {0}.", name_));
      }
   }

   void
   File::Write(const String &sWrite)
   {
      String temp_nonconst = sWrite;

      Write_(temp_nonconst.GetBuffer(), sizeof(TCHAR), temp_nonconst.GetLength());
   }

   void
   File::Write(const AnsiString &sWrite)
   {  
      AnsiString temp_nonconst = sWrite;
      
      Write_(temp_nonconst.GetBuffer(), sizeof(char), temp_nonconst.GetLength());
   }

   void
   File::Write(const unsigned char *pBuf, size_t iBufLen)
   {
      Write_((void*) pBuf, 1, iBufLen);
   }

   void
   File::Write(std::shared_ptr<ByteBuffer> pBuffer)
   {
      return Write((const unsigned char*) pBuffer->GetCharBuffer(), pBuffer->GetSize());
   }

   void
   File::WriteBOF()
   {
      // Write unicode beginner markers.
      unsigned char charByteOrderMarker[2] = {-1, -2};

      Write(charByteOrderMarker, 2);
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
      int iFileSize = GetSize();

      return ReadChunk(iFileSize);
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
      std::shared_ptr<ByteBuffer> pFileContents = std::shared_ptr<ByteBuffer>(new ByteBuffer);

      if (file_ == nullptr)
         throw std::logic_error("Attempt to read from file which has not been opened.");
         
      // Create a buffer to hold the file
      pFileContents->Allocate(iMaxSize);

      // fread fails reading large files. If the file is too large, fread will read zero bytes and the
      // errno will be set to invalid argument. The below code therefore reads the file in chunks.
      size_t remaining_bytes = iMaxSize;
      BYTE *buffer_position = (BYTE*)pFileContents->GetBuffer();
         
      size_t total_bytes_read = 0;

      while (remaining_bytes > 0)
      {
         size_t bytes_to_read = min(FileChunkSize, remaining_bytes);
         size_t bytes_actually_read = fread((void*)buffer_position, 1, bytes_to_read, file_);

         total_bytes_read += bytes_actually_read;
                
         if (bytes_actually_read != bytes_to_read)
         {
            if (feof(file_))
            {
               // we've reached end of file.
               break;
            }
            
            char error_msg[255];
            strerror_s(error_msg, 255, errno);
            throw std::runtime_error(Formatter::FormatAsAnsi("Unable to read file {0}. Expected bytes: {1}, Actual read bytes: {2}. Error: {3}", name_, bytes_to_read, bytes_actually_read, error_msg));
         }

         buffer_position += bytes_actually_read;
         remaining_bytes -= bytes_actually_read;
            
      }

      pFileContents->DecreaseSize(iMaxSize - total_bytes_read);
         
      if (pFileContents->GetSize() == 0)
      {
         std::shared_ptr<ByteBuffer> empty;
         return empty;
      }

      return pFileContents;
   }

   String 
   File::GetName() const
   {
      return name_;
   }

   void
   File::Write(File &sourceFile)
   {
      while (std::shared_ptr<ByteBuffer> sourceData = sourceFile.ReadChunk(FileChunkSize))
      {  
         Write(sourceData);
      }
   }

   void
   File::Write_(void *buffer, size_t item_size, size_t item_count)
   {
      if (file_ == nullptr)
         throw std::logic_error("Attempt to write to file which has not been opened.");

      if (item_size * item_count > FileChunkSize)
      {
         throw std::logic_error(Formatter::FormatAsAnsi("Attempt to write {0} bytes to {1}. Exceeds max chunk size.", item_size * item_count, name_));
      }

      size_t items_written = fwrite(buffer, item_size, item_count, file_);

      if (items_written != item_count)
      {
         ThrowRuntimeError_(Formatter::FormatAsAnsi("Attempted to write {0} items of size {1} to file {2}. The write failed.", item_size, item_count, name_));
      }
   }



   void
   File::ThrowRuntimeError_(const AnsiString &message)
   {
      char error_msg[255];
      strerror_s(error_msg, 255, errno);

      AnsiString complete_message = Formatter::FormatAsAnsi("{0} - Error: {1}", message, error_msg);

      throw std::runtime_error(complete_message);
   }

}