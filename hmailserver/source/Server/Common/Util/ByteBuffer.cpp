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
      buffer_ (0),
      buffer_size_ (0)
   {

   }

   ByteBuffer::~ByteBuffer()
   {
      if (buffer_)
      {
         delete [] buffer_;
         buffer_ = 0;

         buffer_size_ = 0;
      }   
   }

   void 
   ByteBuffer::Empty()
   {
      if (buffer_)
      {
         delete [] buffer_;
         buffer_ = 0;

         buffer_size_ = 0;
      }   
   }

   void 
   ByteBuffer::Empty(size_t iLeaveEndingBytes)
   {
      if (iLeaveEndingBytes > buffer_size_)
      {
         throw std::logic_error(Formatter::FormatAsAnsi("The number of bytes to leave exceeds buffer size. Bytes to leave: {0}, Buffer size: {1}", iLeaveEndingBytes, buffer_size_));
      }

      // Allocate a temporary buffer.
      BYTE * pRemaining = new BYTE[iLeaveEndingBytes];
      
      // Copy the remaining data to this buffer.
      memcpy(pRemaining, buffer_ + (buffer_size_ - iLeaveEndingBytes), iLeaveEndingBytes );

      // Empty this buffer.
      Empty();

      // Add the remaining data to the buffer again.
      Add(pRemaining, iLeaveEndingBytes);

      delete [] pRemaining;
   }

   void 
   ByteBuffer::Allocate(size_t lSize)
   {
      
      Empty();
      
      // Allocate a new buffer
      buffer_ = new BYTE[lSize];
      memset(buffer_, 0, lSize);
      buffer_size_ = lSize;
   }

   void 
   ByteBuffer::Add(ByteBuffer *pBuf)
   {
      Add(pBuf->GetBuffer(), pBuf->GetSize());
   }

   void 
   ByteBuffer::Add(std::shared_ptr<ByteBuffer> pBuf)
   {
      Add(pBuf->GetBuffer(), pBuf->GetSize());
   }

   void
   ByteBuffer::Add(const BYTE *pBuf, size_t lSize)
   {
      
      if (lSize == 0)
      {
         // Nothing to do.
         return;
      }

      size_t iTotBufLen = buffer_size_ + lSize;

      // Allocate a new buffer big enough to contain
      // both old and new buffer.
      BYTE *tmpbuf = new BYTE[iTotBufLen];
      memset(tmpbuf, 0, iTotBufLen);
      
      // Copy the old data to the temporary buffer.
      if (buffer_size_ > 0)
         memcpy(tmpbuf, buffer_, buffer_size_);
      
      // Copy the new data to the temporary buffer.
      memcpy(tmpbuf + buffer_size_,pBuf, lSize);

      // We should now repoint this->buffer to
      // tmpbuf. Free current buffer.
      Empty();

      buffer_ = tmpbuf;
      buffer_size_ = iTotBufLen;
   }

   void 
   ByteBuffer::DecreaseSize(size_t iDecreaseWith)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Decreases the size of the buffer. This is done just by decreasing the
   // member variable that tells us how large the buffer is. This may look weird
   // but gives better performance than allocating a whole new buffer.
   //---------------------------------------------------------------------------()
   {
      if (buffer_size_ - iDecreaseWith < 0)
      {
         // We should never get here. This code is only run if you
         // decrease so that the size get's negative.
         ErrorManager::Instance()->ReportError(ErrorManager::High, 4222, "ByteBuffer::DecreaseSize", "Tried to decrease the size of the buffer to a negative value.");
         return ;
      }

      buffer_size_ -= iDecreaseWith;
   }
}

