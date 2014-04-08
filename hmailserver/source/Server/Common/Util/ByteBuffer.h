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

      bool IsEmpty() const {return m_lBufferSize == 0; }
      // Returns true if the buffer is empty.

      long GetSize() const {return m_lBufferSize; }
      // Returns the size of the buffer.
      
      void Empty();
      // Empties the entire buffer
      
      void Empty(int iLeaveEndingBytes);
      // Empties the entire buffer, except for the
      // remaining bytes

      const BYTE* GetBuffer() const {return m_pBuffer; }
      const char* GetCharBuffer() const  {return (const char*) m_pBuffer; }
      // Returns pointer to the buffer

      void Add(const BYTE *pBuf, long lSize);
      void Add(ByteBuffer *pBuf);
      void Add(shared_ptr<ByteBuffer> pBuf);
      // Appends the buffer
   
      void Allocate(long lSize);
      // Allocates a buffer.

      void DecreaseSize(int iDecreaseWith);

   private:

      BYTE *m_pBuffer;
      long m_lBufferSize;
   };

}
