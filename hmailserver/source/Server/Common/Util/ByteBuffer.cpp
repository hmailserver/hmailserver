// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "ByteBuffer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ByteBuffer::ByteBuffer() :
      m_pBuffer (0),
      m_lBufferSize (0)
   {

   }

   ByteBuffer::~ByteBuffer()
   {
      try
      {
         if (m_pBuffer)
         {
            delete [] m_pBuffer;
            m_pBuffer = 0;

            m_lBufferSize = 0;
         }   
      }
      catch (...)
      {
         String message;
         message.Format(_T("Error when emptying buffer. This: %d"), this);
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5339, "ByteBuffer::~ByteBuffer", message);
         throw;
      }
   }

   void 
   ByteBuffer::Empty()
   {
      if (m_pBuffer)
      {
         delete [] m_pBuffer;
         m_pBuffer = 0;

         m_lBufferSize = 0;
      }   
   }

   void 
   ByteBuffer::Empty(int iLeaveEndingBytes)
   {
      if (iLeaveEndingBytes > m_lBufferSize)
      {
         assert(0);
         iLeaveEndingBytes = m_lBufferSize;
      }

      if (iLeaveEndingBytes < 0)
      {
         assert(0);
         iLeaveEndingBytes = 0;
      }

      // Allocate a temporary buffer.
      BYTE * pRemaining = new BYTE[iLeaveEndingBytes];
      
      // Copy the remaining data to this buffer.
      memcpy(pRemaining, m_pBuffer + (m_lBufferSize - iLeaveEndingBytes), iLeaveEndingBytes );

      // Empty this buffer.
      Empty();

      // Add the remaining data to the buffer again.
      Add(pRemaining, iLeaveEndingBytes);

      delete [] pRemaining;
   }

   void 
   ByteBuffer::Allocate(long lSize)
   {
      
      Empty();
      
      // Allocate a new buffer
      m_pBuffer = new BYTE[lSize];
      memset(m_pBuffer, 0, lSize);
      m_lBufferSize = lSize;
   }

   void 
   ByteBuffer::Add(ByteBuffer *pBuf)
   {
      Add(pBuf->GetBuffer(), pBuf->GetSize());
   }

   void 
   ByteBuffer::Add(shared_ptr<ByteBuffer> pBuf)
   {
      Add(pBuf->GetBuffer(), pBuf->GetSize());
   }

   void
   ByteBuffer::Add(const BYTE *pBuf, long lSize)
   {
      
      if (lSize == 0)
      {
         // Nothing to do.
         return;
      }

      long iTotBufLen = m_lBufferSize + lSize;

      // Allocate a new buffer big enough to contain
      // both old and new buffer.
      BYTE *tmpbuf = new BYTE[iTotBufLen];
      memset(tmpbuf, 0, iTotBufLen);
      
      // Copy the old data to the temporary buffer.
      if (m_lBufferSize > 0)
         memcpy(tmpbuf, m_pBuffer, m_lBufferSize);
      
      // Copy the new data to the temporary buffer.
      memcpy(tmpbuf + m_lBufferSize,pBuf, lSize);

      // We should now repoint this->buffer to
      // tmpbuf. Free current buffer.
      Empty();

      m_pBuffer = tmpbuf;
      m_lBufferSize = iTotBufLen;
   }

   void 
   ByteBuffer::DecreaseSize(int iDecreaseWith)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Decreases the size of the buffer. This is done just by decreasing the
   // member variable that tells us how large the buffer is. This may look weird
   // but gives better performance than allocating a whole new buffer.
   //---------------------------------------------------------------------------()
   {
      if (m_lBufferSize - iDecreaseWith < 0)
      {
         // We should never get here. This code is only run if you
         // decrease so that the size get's negative.
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4222, "ByteBuffer::DecreaseSize", "Tried to decrease the size of the buffer to a negative value.");
         return ;
      }

      m_lBufferSize -= iDecreaseWith;
   }
}

