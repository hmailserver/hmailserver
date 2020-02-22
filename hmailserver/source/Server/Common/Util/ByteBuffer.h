// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ByteBuffer  
   {
   public:
	   ByteBuffer();
	   virtual ~ByteBuffer();

      bool IsEmpty() const {return buffer_size_ == 0; }
      // Returns true if the buffer is empty.

      size_t GetSize() const {return buffer_size_; }
      // Returns the size of the buffer.
      
      void Empty();
      // Empties the entire buffer
      
      void Empty(size_t iLeaveEndingBytes);
      // Empties the entire buffer, except for the
      // remaining bytes

      const BYTE* GetBuffer() const {return buffer_; }
      const char* GetCharBuffer() const  {return (const char*) buffer_; }
      // Returns pointer to the buffer

      void Add(const BYTE *pBuf, size_t lSize);
      void Add(ByteBuffer *pBuf);
      void Add(std::shared_ptr<ByteBuffer> pBuf);
      // Appends the buffer
   
      void Allocate(size_t lSize);
      // Allocates a buffer.

      void DecreaseSize(size_t iDecreaseWith);

   private:

      BYTE *buffer_;
      size_t buffer_size_;
   };

}
