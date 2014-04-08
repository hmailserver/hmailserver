// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "File.h"
#include "ByteBuffer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   File::File() :
      m_hFile(INVALID_HANDLE_VALUE)
   {
      
   }

   File::~File()
   {
      if (m_hFile != INVALID_HANDLE_VALUE)
         CloseHandle(m_hFile);
   }
   
   void 
   File::Close()
   {
      CloseHandle(m_hFile);
      m_hFile = INVALID_HANDLE_VALUE;
   }

   bool 
   File::IsOpen() const
   {
      if (m_hFile == INVALID_HANDLE_VALUE)
         return false;
      
      return true;
   }

   bool
   File::Open(const String &sFilename, OpenType ot)
   {
      if (m_hFile != INVALID_HANDLE_VALUE)
      {
         // The file should be closed, before we
         // try to open it again...

         assert(0);
         Close();
      }

      DWORD dwDesiredAccess = 0;
      DWORD dwShareMode = 0;
      DWORD dwCreationDisposition = 0;

      switch (ot)
      {
      case OTReadOnly:
         dwDesiredAccess = GENERIC_READ;
         dwShareMode = FILE_SHARE_READ;
         dwCreationDisposition = OPEN_EXISTING;
         break;
      case OTCreate:
         dwDesiredAccess = GENERIC_WRITE;
         dwShareMode = FILE_SHARE_READ;
         dwCreationDisposition = CREATE_ALWAYS;
         break;
      case OTAppend:
         dwDesiredAccess = GENERIC_WRITE;
         dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE; // FILE_SHARE_DELETE;
         dwCreationDisposition = OPEN_ALWAYS;
         break;
      }

      m_hFile = CreateFile(sFilename, 
                              dwDesiredAccess, 
                              dwShareMode, 
                              NULL, // LPSECURITY_ATTRIBUTES
                              dwCreationDisposition, // -- open or create.
                              FILE_ATTRIBUTE_NORMAL, // attributes
                              NULL // file template
                           );

      if (m_hFile == INVALID_HANDLE_VALUE) 
         return false;

      m_sName = sFilename;

      if (ot == OTAppend)
      {  
         // Go to end of file
         SetFilePointer(m_hFile,0,0,FILE_END);
      }

      return true;
   }

   int 
   File::GetSize()
   {
      if (m_hFile == INVALID_HANDLE_VALUE)
         return 0;

      return GetFileSize (m_hFile, NULL);
   }

   bool 
   File::Write(const String &sWrite)
   {
      String sTmp = sWrite;
      DWORD dwWritten = 0;
      return Write((const unsigned char*) sTmp.GetBuffer(), sTmp.GetLength() * sizeof(TCHAR), dwWritten);
   }

   bool 
   File::Write(const AnsiString &sWrite)
   {  
      AnsiString sTmp = sWrite;
      DWORD dwWritten = 0;
      return Write((const unsigned char*) sTmp.GetBuffer(), sTmp.GetLength(), dwWritten);
   }

   bool 
   File::Write(const unsigned char *pBuf, int iBufLen, DWORD &dwNoOfBytesWritten)
   {
      bool bResult = ::WriteFile(m_hFile,pBuf, iBufLen, &dwNoOfBytesWritten, NULL) == TRUE;
      return bResult;
   }

   bool 
   File::Write(shared_ptr<ByteBuffer> pBuffer, DWORD &dwNoOfBytesWritten)
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

   shared_ptr<ByteBuffer> 
   File::ReadFile()
   {
      try
      {
         shared_ptr<ByteBuffer> pFileContents = shared_ptr<ByteBuffer>(new ByteBuffer);

         if (m_hFile == INVALID_HANDLE_VALUE)
            return pFileContents;

         int iFileSize = GetSize();

         // Create a buffer to hold the file
         pFileContents->Allocate(iFileSize);

         // Read the file to the buffer
         SetFilePointer(m_hFile, 0, 0, FILE_BEGIN);

         unsigned long nBytesRead = 0;
         ::ReadFile(m_hFile, (LPVOID) pFileContents->GetBuffer(), iFileSize, &nBytesRead, NULL);

         return pFileContents;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4227, "File::ReadFile", "An unknown error occurred while reading file from disk.");
         throw;
      }

      shared_ptr<ByteBuffer> pFileContents = shared_ptr<ByteBuffer>(new ByteBuffer);
      return pFileContents;
   }

   shared_ptr<ByteBuffer> 
   File::ReadTextFile()
   {
      try
      {
         shared_ptr<ByteBuffer> pFileContents = ReadFile();

         const BYTE pByte = '\0';
         pFileContents->Add(&pByte, 1);
         return pFileContents;
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5042, "File::ReadTextFile", "An unknown error occurred while reading a text file from disk.");
         throw;
      }

      shared_ptr<ByteBuffer> pFileContents = shared_ptr<ByteBuffer>(new ByteBuffer);
      return pFileContents;
   }

   shared_ptr<ByteBuffer> 
   File::ReadChunk(int iMaxSize)
   {  
      try
      {
         shared_ptr<ByteBuffer> pReadBuffer = shared_ptr<ByteBuffer>(new ByteBuffer);
         pReadBuffer->Allocate(iMaxSize);

         // Read
         unsigned long nBytesRead = 0;
         ::ReadFile(m_hFile, (LPVOID) pReadBuffer->GetBuffer(), iMaxSize, &nBytesRead, NULL);

         if (nBytesRead > 0)
         {
            shared_ptr<ByteBuffer> pRetBuffer = shared_ptr<ByteBuffer>(new ByteBuffer);
            pRetBuffer->Add(pReadBuffer->GetBuffer(), nBytesRead);
            return pRetBuffer;
         }
         
      }
      catch (...)
      {
         throw;   
      }

      shared_ptr<ByteBuffer> pBuffer;
      return pBuffer;
   }

   bool 
   File::MoveToEnd()
   {
      // --- Go to the end of the file.
      SetFilePointer(m_hFile,0,0,FILE_END);

      return true;
   }

   String 
   File::GetName() const
   {
      return m_sName;
   }

   bool 
   File::Write(File &sourceFile)
   {
      while (shared_ptr<ByteBuffer> sourceData = sourceFile.ReadChunk(FileChunkSize))
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