// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-10-05

#include "StdAfx.h"
#include ".\transparenttransmissionbuffer.h"

#include "ByteBuffer.h"
#include "../TCPIP/ProtocolParser.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TransparentTransmissionBuffer::TransparentTransmissionBuffer(bool bSending) : 
      m_bIsSending(bSending),
      m_bTransmissionEnded(false),
      m_bLastSendEndedWithNewline(false),
      m_iDataSent(0),
      m_iMaxSizeKB(0),
      _cancelTransmission(false)
   {
      m_pBuffer = shared_ptr<ByteBuffer>(new ByteBuffer);
   }

   TransparentTransmissionBuffer::~TransparentTransmissionBuffer(void)
   {

   }

   bool
   TransparentTransmissionBuffer::Initialize(ProtocolParser *pProtocolParser)
   {
      m_pProtocolParser = pProtocolParser;


      m_iDataSent = 0;

      return true;
   }

   bool 
   TransparentTransmissionBuffer::Initialize(const String &sFilename)
   {
      if (!m_oFile.Open(sFilename, File::OTAppend))
      {
         // This is not good. We failed to get a handle to the file.
         // Log to event log and notify the sender of this error.

         String sErrorMessage;
         sErrorMessage.Format(_T("Failed to write to the file %s. Data from sender rejected."), sFilename);

         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5075, "TransparentTransmissionBuffer::_SaveToFile", sErrorMessage);

         return false;
      } 

      m_iDataSent = 0;

      return true;
   }

   void 
   TransparentTransmissionBuffer::SetMaxSizeKB(int maxSize)
   {
      m_iMaxSizeKB = maxSize;
   }

   void 
   TransparentTransmissionBuffer::Append(const BYTE *pBuffer, int iBufferSize)
   {
      if (iBufferSize == 0)
      {
         // Nothing to add.
         return;
      }   

      if (pBuffer == 0)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5411, "TransparentTransmissionBuffer::Append", "pBuffer is NULL");
         throw;
      }

      if (m_pBuffer == 0)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5412, "TransparentTransmissionBuffer::Append", "m_pBuffer is NULL");
         throw;
      }

      m_iDataSent+= iBufferSize;

      // Add the new data to the buffer.
      try
      {
         // Add the new data to the buffer.
         m_pBuffer->Add(pBuffer, iBufferSize);
      }
      catch (...)
      {
         String message;
         message.Format(_T("Error when appending buffer. Buffer: %d, pBuffer: %d, Size: %d"), &m_pBuffer, &pBuffer, iBufferSize);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5413, "TransparentTransmissionBuffer::Append", message);
         throw;
      }

      // Check if we have received the entire buffer.
      if (m_pBuffer->GetSize() >= 3 && !m_bIsSending)
      {
         try
         {
            // If receiving, we should check for end-of-data
            int iSize = m_pBuffer->GetSize();
            const char *pCharBuffer = m_pBuffer->GetCharBuffer();

            // Check if the buffer only contains a dot on an empty line.
            bool bDotCRLFOnEmptyLine = (pCharBuffer[0] == '.' && pCharBuffer[1] == '\r' && pCharBuffer[2] == '\n');

            // Look for \r\n.\r\n. 
            bool bLineBeginnningWithDotCRLF = m_pBuffer->GetSize() >= 5 &&
               (pCharBuffer[iSize -5] == '\r' && 
               pCharBuffer[iSize -4] == '\n' && 
               pCharBuffer[iSize -3] == '.' && 
               pCharBuffer[iSize -2] == '\r' && 
               pCharBuffer[iSize -1] == '\n');

            if (bDotCRLFOnEmptyLine || bLineBeginnningWithDotCRLF)
            {
               // Remove the transmission-end characters. (the 3 last)
               m_pBuffer->DecreaseSize(3);

               m_bTransmissionEnded = true;
            }
         }
         catch (...)
         {
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5339, "TransparentTransmissionBuffer::Append", "Error when checking for linebreaks.");
            throw;
         }
      }
   }

   bool 
   TransparentTransmissionBuffer::GetRequiresFlush()
   {
      if (m_pBuffer->GetSize() > 40000 || m_bTransmissionEnded)
         return true;
      else
         return false;
   }

   int 
   TransparentTransmissionBuffer::GetSize()
   {  
      return m_iDataSent;
   }

   bool
   TransparentTransmissionBuffer::Flush(bool bForce)
   {
      bool dataProcessed = false;

      if (!GetRequiresFlush() && !bForce)
         return dataProcessed;

      if (m_pBuffer->GetSize() > MAX_LINE_LENGTH)
      {
         // Something fishy is going on. We've received over MAX_LINE_LENGTH
         // characters on a single line with no new line character. This should
         // never happen in normal email communication so let's assume someone
         // is trying to attack us.
         _cancelTransmission = true;
         _cancelMessage = "Too long line was received. Transmission aborted.";
         bForce = true;
      }

      // Locate last \n
      const char *pBuffer = m_pBuffer->GetCharBuffer();
      int bufferSize = m_pBuffer->GetSize();
      
      /*
         RFC rfc2821
         text line
            The maximum total length of a text line including the <CRLF> is
            1000 characters (not counting the leading dot duplicated for
            transparency).  This number may be increased by the use of SMTP
            Service Extensions.
      */

      int maxLineLength = MAX_LINE_LENGTH;

      // Start in the end and move 'back' MAX_LINE_LENGTH characters.
      int searchEndPos = max(bufferSize - maxLineLength, 0);
      for (int i = bufferSize - 1; i >= searchEndPos; i--)
      {
         char s = pBuffer[i];

         // If we found a newline, send anything up until that.
         // If we're forcing a send, send all we got
         // If we found no newline in the stream, the message is malformed according to RFC2821 (max 1000 chars per line). 
         //    Send all we got anyway. 

         if (s == '\n' || bForce)
         {
            m_bLastSendEndedWithNewline = s == '\n';

            // Copy the data up including this position
            int iCopySize = i+1;

            shared_ptr<ByteBuffer> pOutBuffer = shared_ptr<ByteBuffer>(new ByteBuffer);
            pOutBuffer->Add(m_pBuffer->GetBuffer(), iCopySize);

            // Remove it from the old buffer
            int iRemaining = m_pBuffer->GetSize() - iCopySize;
            m_pBuffer->Empty(iRemaining);

            // Parse this buffer and add it to file/socket
            if (m_bIsSending)
               _InsertTransmissionPeriod(pOutBuffer);
            else
               _RemoveTransmissionPeriod(pOutBuffer);

            // The parsed buffer can now be sent.
            if (m_bIsSending)
            {
               if (!m_pProtocolParser->SendData(pOutBuffer))
                  return false;
            }
            else 
            {
               _SaveToFile(pOutBuffer);
            }

            dataProcessed = true;

            break;
         }
      }

      if (m_bTransmissionEnded && m_oFile.IsOpen())
      {
         m_oFile.Close();
      }

      return dataProcessed;
   }

   bool 
   TransparentTransmissionBuffer::_SaveToFile(shared_ptr<ByteBuffer> pBuffer)
   {
      if (m_iMaxSizeKB > 0 && (m_iDataSent / 1024) > m_iMaxSizeKB)
      {
         // we've reached the max size. don't save more data.
         return false;
      }

      if (!_cancelTransmission)
      {
         DWORD dwNoOfBytesWritten = 0;
         bool bResult = m_oFile.Write(pBuffer, dwNoOfBytesWritten);
      }
 
      return true;
   }

   void 
   TransparentTransmissionBuffer::_InsertTransmissionPeriod(shared_ptr<ByteBuffer> pBuffer)
   {
      // All . which are placed as the first character on a new
      // line should be replaced with ..
      
      // Allocate maximum required length for the out buffer.
      char *pInBuffer = (char*) pBuffer->GetCharBuffer();

      char *pOutBuffer = new char[pBuffer->GetSize() * 2];
      char *pOutBufferStart = pOutBuffer;

      int iInBufferSize = pBuffer->GetSize();
     
      for (int i = 0; i < iInBufferSize; i++)
      {
         char c = pInBuffer[i];
         if (c == '.')
         {
            if (i == 0)
            {
               *pOutBuffer = '.';
               pOutBuffer++;
            }
            else if (i > 2 && pInBuffer[i-1] == '\n')
            {
               *pOutBuffer = '.';
               pOutBuffer++;
            }
         }

         // Add the character
         *pOutBuffer = c;
         pOutBuffer++;
      }

      // Clear the buffer and insert the new data
      int iOutBufferLen = pOutBuffer - pOutBufferStart;
      pBuffer->Empty();
      pBuffer->Add((BYTE*) pOutBufferStart, iOutBufferLen);

      // Free memory for the old buffer
      delete [] pOutBufferStart;
   }

   void
   TransparentTransmissionBuffer::_RemoveTransmissionPeriod(shared_ptr<ByteBuffer> pBuffer)
   {
      // Allocate maximum required length for the out buffer.
      char *pInBuffer = (char*) pBuffer->GetCharBuffer();

      char *pOutBuffer = new char[pBuffer->GetSize()];
      char *pOutBufferStart = pOutBuffer;

      int iInBufferSize = pBuffer->GetSize();

      for (int i = 0; i < iInBufferSize; i++)
      {
         char c = pInBuffer[i];
         if (c == '.')
         {
            if (i == 0)
               continue;
            else if (i > 2 && pInBuffer[i-1] == '\n')
               continue;
         }

         // Add the character to the out buffer
         *pOutBuffer = c;
         pOutBuffer++;

      }

      // Clear the buffer and insert the new data
      int iOutBufferLen = pOutBuffer - pOutBufferStart;
      pBuffer->Empty();
      pBuffer->Add((BYTE*) pOutBufferStart, iOutBufferLen);

      // Free memory for the old buffer
      delete [] pOutBufferStart;
   }

}