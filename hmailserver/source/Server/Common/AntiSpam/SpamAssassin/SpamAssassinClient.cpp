// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\SpamAssassinClient.h"

#include "../../Util/ByteBuffer.h"
#include "../../Util/File.h"
#include "../../TCPIP/TCPConnection.h"

#include "../../Application/TimeoutCalculator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SpamAssassinClient::SpamAssassinClient(const String &sFile) 
   {
      TimeoutCalculator calculator;
      SetTimeout(calculator.Calculate(IniFileSettings::Instance()->GetSAMinTimeout(), IniFileSettings::Instance()->GetSAMaxTimeout()));
      
      m_sMessageFile = sFile;
	  m_iSpamDSize = -1;
	  m_iMessageSize = -1;
   }


   SpamAssassinClient::~SpamAssassinClient(void)
   {

   }

   void
   SpamAssassinClient::OnConnected()
   {
      // We'll handle all incoming data as binary.
      SetReceiveBinary(true);
      m_iMessageSize = FileUtilities::FileSize(m_sMessageFile);
      SendData("PROCESS SPAMC/1.2\r\n");
	  //LOG_DEBUG("SENT: PROCESS SPAMC/1.2");
	  String sConLen;
	  sConLen.Format(_T("Content-length: %d\r\n"), m_iMessageSize);
	  SendData(sConLen);
	  /*sConLen.Format(_T("Sent: Content-length: %d"), m_iMessageSize);
	  LOG_DEBUG(sConLen);*/
	  //Future feature, per user scanning. SendData("User: " ); // send the current recipient.
	  SendData("\r\n");
      _SendFileContents(m_sMessageFile);
   }

   AnsiString 
   SpamAssassinClient::GetCommandSeparator() const
   {
      return "\r\n";
   }

   void 
   SpamAssassinClient::OnCouldNotConnect(const AnsiString &sErrorDescription)
   {
      String logMessage;
      logMessage.Format(_T("Failed to connect to SpamAssassin. Session %d"), GetSessionID());
      LOG_DEBUG(logMessage);
   }

   bool
   SpamAssassinClient::_SendFileContents(const String &sFilename)
   {
      String logMessage;
      logMessage.Format(_T("Sending message to SpamAssassin. Session %d, File: %s"), GetSessionID(), sFilename);
      LOG_DEBUG(logMessage);

      File oFile;
      if (!oFile.Open(sFilename, File::OTReadOnly))
      {
         String sErrorMsg;
         sErrorMsg.Format(_T("Could not send file %s via socket since it does not exist."), sFilename);

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5019, "SMTPClientConnection::_SendFileContents", sErrorMsg);

         return false;
      }

      const int maxIterations = 100000;
      for (int i = 0; i < maxIterations; i++)
      {
         shared_ptr<ByteBuffer> pBuf = oFile.ReadChunk(20000);

         if (!pBuf)
            break;

         BYTE *pSendBuffer = (BYTE*) pBuf->GetBuffer();
         int iSendBufferSize = pBuf->GetSize();

         SendData(pBuf);
      }

      PostShutdown(TCPConnection::ShutdownSend);

      // Request the response...
      PostBufferReceive();
      
      return true;
   }

   void
   SpamAssassinClient::OnConnectionTimeout()
   {
      // do nothing
   }

   void
   SpamAssassinClient::OnExcessiveDataReceived()
   {
      // do nothing
   }

   void
   SpamAssassinClient::ParseData(const AnsiString &sData)
   {
      
   }

   void
   SpamAssassinClient::ParseData(shared_ptr<ByteBuffer> pBuf)
   {
      if (!m_pResult)
      {
         String logMessage;
         logMessage.Format(_T("Parsing response from SpamAssassin. Session %d"), GetSessionID());
         LOG_DEBUG(logMessage);

         m_pResult = shared_ptr<File>(new File);
         m_pResult->Open(FileUtilities::GetTempFileName(), File::OTAppend);

         m_iSpamDSize = _ParseFirstBuffer(pBuf);
      }

      // Append output to the file
      DWORD dwWritten = 0;
      m_pResult->Write(pBuf, dwWritten);

      PostReceive();
   }

   bool
   SpamAssassinClient::FinishTesting()
   {
      if (!m_pResult)
         return false;

      m_pResult->Close();

      // Copy message if test has been run
      bool bTestsRun = true;

      String sTempFile = m_pResult->GetName();
	  
      // new way: check the result from spamd.
      if (bTestsRun && (FileUtilities::FileSize(sTempFile) == m_iSpamDSize))
      {
         if (IniFileSettings::Instance()->GetSAMoveVsCopy())
         {
            // Move temp file overwriting message file
            FileUtilities::Move(sTempFile, m_sMessageFile, true);
            LOG_DEBUG("SA - Move used");
         }
         else
         {
            // Copy temp file to message file
            FileUtilities::Copy(sTempFile, m_sMessageFile, false);
            FileUtilities::DeleteFile(sTempFile);
            LOG_DEBUG("SA - Copy+Delete used");
         }
	  } else {
		 String logMessage;
		 logMessage.Format(_T("SA: Temp file size did not match what Spamd reported! (temp: %d, spamd: %d). Reverting to original message file."),FileUtilities::FileSize(sTempFile),m_iSpamDSize);
         LOG_DEBUG(logMessage);
	  }

      return true;
   }

   int
   SpamAssassinClient::_ParseFirstBuffer(shared_ptr<ByteBuffer> pBuffer) const
   {
      // Don't send first line, since it's the Result header.
      char *pHeaderEndPosition = StringParser::Search(pBuffer->GetCharBuffer(), pBuffer->GetSize(), "\r\n\r\n");
      if (!pHeaderEndPosition)
      {
         LOG_DEBUG("The response from SpamAssasin was not valid. Aborting. Expected a header.\r\n");
         return -1;
      }
            
      int headerLength = pHeaderEndPosition - pBuffer->GetCharBuffer();
      AnsiString spamAssassinHeader(pBuffer->GetCharBuffer(), headerLength);

      vector<AnsiString> headerLines = StringParser::SplitString(spamAssassinHeader, "\r\n");
      AnsiString firstLine = headerLines[0];
      AnsiString secondLine = headerLines[1];

      if (firstLine.Compare("SPAMD/1.1 0 EX_OK") != 0)
      {
         // We should never get here, since we should always have
         // a header in the result

         LOG_DEBUG(Formatter::Format("The response from SpamAssasin was not valid. Aborting. Expected: SPAMD/1.1 0 EX_OK, Got: {0}\r\n", firstLine));
         return -1;
      }

      if (!secondLine.StartsWith("Content-length:"))
      {
         // We should never get here, since we should always have
         // a header in the result
         LOG_DEBUG(Formatter::Format("The response from SpamAssasin was not valid. Aborting. Expected: Content-Length:<value>, Got: {0}\r\n", secondLine));
         return -1;
      }

      // Extract the second line from the first buffer. This buffer
      // contains the result of the operation (success / failure).
      vector<AnsiString> contentLengthHeader = StringParser::SplitString(secondLine, ":");
      if (contentLengthHeader.size() != 2)
      {
         LOG_DEBUG(Formatter::Format("The response from SpamAssasin was not valid. Aborting. Content-Length header not properly formatted. Expected: Content-Length:<value>, Got: {0}\r\n", secondLine));
         return -1;
      }

      int contentLength;
      string sConSize = contentLengthHeader[1].Trim();
      if (!StringParser::TryParseInt(sConSize, contentLength))
      {
        LOG_DEBUG(Formatter::Format("The response from SpamAssasin was not valid. Aborting. Content-Length header not properly formatted. Expected: Content-Length:<value>, Got: {0}\r\n", secondLine));
	     return -1;
      }

      // Remove the SA header lines from the result.
      int iEndingBytesSize = pBuffer->GetSize() - headerLength - 4; // 4 due to header ending with \r\n\r\n.
      pBuffer->Empty(iEndingBytesSize);

      return contentLength;
   }

   /*
      Handles the situation where SpamAssasin does not send the entire
      response to hMailServer. In this case, an error will be logged and
      SA won't have any effect.

   */
   void 
   SpamAssassinClient::OnReadError(int errorCode)
   {
      String errorMessage;
      errorMessage.Format(_T("There was a communication error with SpamAssassin. ")
                          _T("hMailServer tried to retrieve data from SpamAssassin but the connection ")
                          _T("to SpamAssassin was lost. The WinSock error code is %d. Enable debug ")
                          _T("logging to retrieve more information regarding this problem. ")
                          _T("The problem could be that SpamAssassin is malfunctioning."), errorCode);

      ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5157, "SpamAssassinClient::OnReadError", errorMessage);

      if (m_pResult)
      {
         m_pResult->Close();
         FileUtilities::DeleteFile(m_pResult->GetName());
         m_pResult.reset();
      }
   }
}
