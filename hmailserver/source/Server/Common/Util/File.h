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
      void Open(const String &sFilename, OpenType ot);

      void Close();
      
      void Write(const String &sWrite);
      void Write(const AnsiString &sWrite);
      void Write(const unsigned char *pBuf, int iBufLen);
      void Write(std::shared_ptr<ByteBuffer> pBuffer);
      void Write(File &sourceFile);
      void WriteBOF();

      void Write_(void *buffer, int item_size, int item_count);

      int GetSize();

      void SetPosition(int position);
      bool ReadLine(AnsiString &sLine);
      std::shared_ptr<ByteBuffer> ReadFile();
      std::shared_ptr<ByteBuffer> ReadTextFile();
      std::shared_ptr<ByteBuffer> ReadChunk(int iMaxSize);

      String GetName() const;

   private:

      void ThrowRuntimeError_(const AnsiString &message);

      FILE * file_;
      String name_;
   };

}
