// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2006-03-25
//

#pragma once

namespace HM
{
   class ByteBuffer;

   class File  
   {
   public:
	   File();
	   virtual ~File();

      enum OpenType
      {
         OTReadOnly = 1,
         OTCreate = 2,
         OTAppend = 3,
      };

      enum Constants
      {
         FileChunkSize = 1048576 // 1MB
      };

      bool IsOpen() const;
      bool Open(const String &sFilename, OpenType ot);

      void Close();
      
      bool Write(const String &sWrite);
      bool Write(const AnsiString &sWrite);
      bool Write(const unsigned char *pBuf, size_t iBufLen);
      bool Write(const unsigned char *pBuf, size_t iBufLen, size_t &noOfBytesWritten);
      bool Write(std::shared_ptr<ByteBuffer> pBuffer, size_t &noOfBytesWritten);
      bool Write(File &sourceFile);
      bool WriteBOF();

      void Write_(void *buffer, int item_size, int item_count);
      int GetSize();

      bool SetPosition(int position);
      bool ReadLine(AnsiString &sLine);
      std::shared_ptr<ByteBuffer> ReadFile();
      std::shared_ptr<ByteBuffer> ReadTextFile();
      std::shared_ptr<ByteBuffer> ReadChunk(int iMaxSize);

      String GetName() const;

   private:
      
      FILE * file_;
      String name_;
   };

}
