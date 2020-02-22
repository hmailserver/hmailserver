// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\SpamAssassinClient.h"

#include "../../Util/ByteBuffer.h"
#include "../../Util/File.h"
#include "../../Util/FileUtilities.h"

#include "../../Application/TimeoutCalculator.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SpamAssassinClient::SpamAssassinClient(const String &sFile,
                                          boost::asio::io_service& io_service, 
                                          boost::asio::ssl::context& context,
                                          std::shared_ptr<Event> disconnected,
                                          bool &testCompleted) :
               TCPConnection(CSNone, io_service, context, disconnected, ""),
               test_completed_(testCompleted),
               total_result_bytes_written_(0)
   {
      TimeoutCalculator calculator;
      SetTimeout(calculator.Calculate(IniFileSettings::Instance()->GetSAMinTimeout(), IniFileSettings::Instance()->GetSAMaxTimeout()));
            
      message_file_ = sFile;
	   spam_dsize_ = -1;
	   message_size_ = -1;

      test_completed_ = false;
   }


   SpamAssassinClient::~SpamAssassinClient(void)
   {
      try
      {
         Cleanup_();
      }
      catch (...)
      {
         
      }
   }

   void
   SpamAssassinClient::OnConnected()
   {
      // We'll handle all incoming data as binary.
      SetReceiveBinary(true);
      message_size_ = FileUtilities::FileSize(message_file_);
      EnqueueWrite("PROCESS SPAMC/1.2\r\n");
	  //LOG_DEBUG("SENT: PROCESS SPAMC/1.2");
	  String sConLen;
	  sConLen.Format(_T("Content-length: %d\r\n"), message_size_);
	  EnqueueWrite(sConLen);
	  EnqueueWrite("\r\n");
     SendFileContents_(message_file_);
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
   SpamAssassinClient::SendFileContents_(const String &sFilename)
   {
      String logMessage;
      logMessage.Format(_T("Sending message to SpamAssassin. Session %d, File: %s"), GetSessionID(), sFilename.c_str());
      LOG_DEBUG(logMessage);

      File oFile;
      if (!oFile.Open(sFilename, File::OTReadOnly))
      {
         String sErrorMsg;
         sErrorMsg.Format(_T("Could not send file %s via socket since it does not exist."), sFilename.c_str());

         ErrorManager::Instance()->ReportError(ErrorManager::High, 5019, "SMTPClientConnection::SendFileContents_", sErrorMsg);

         return false;
      }

      const int maxIterations = 100000;
      for (int i = 0; i < maxIterations; i++)
      {
         std::shared_ptr<ByteBuffer> pBuf = oFile.ReadChunk(20000);

         if (pBuf->GetSize() == 0)
            break;

         BYTE *pSendBuffer = (BYTE*) pBuf->GetBuffer();
         size_t iSendBufferSize = pBuf->GetSize();

         EnqueueWrite(pBuf);
      }

      EnqueueShutdownSend();

      // Request the response...
      EnqueueRead("");
      
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
   SpamAssassinClient::ParseData(std::shared_ptr<ByteBuffer> pBuf)
   {
      if (!result_)
      {
         String logMessage;
         logMessage.Format(_T("Parsing response from SpamAssassin. Session %d"), GetSessionID());
         LOG_DEBUG(logMessage);

         result_ = std::shared_ptr<File>(new File);
         result_->Open(FileUtilities::GetTempFileName(), File::OTAppend);

         spam_dsize_ = ParseFirstBuffer_(pBuf);
      }

      // Append output to the file
      size_t written_bytes = 0;
         result_->Write(pBuf, written_bytes);

      total_result_bytes_written_ += written_bytes;

      if (total_result_bytes_written_ < spam_dsize_)
         EnqueueRead();
      else
         FinishTesting_();
   }

   void
   SpamAssassinClient::FinishTesting_()
   {
      if (!result_)
         return;

      result_->Close();

      // Copy message if test has been run
      bool bTestsRun = true;

      String sTempFile = result_->GetName();
	  
      // new way: check the result from spamd.
      if (bTestsRun && (FileUtilities::FileSize(sTempFile) == spam_dsize_))
      {
         if (IniFileSettings::Instance()->GetSAMoveVsCopy())
         {
            // Move temp file overwriting message file
            FileUtilities::Move(sTempFile, message_file_, true);
            LOG_DEBUG("SA - Move used");
         }
         else
         {
            // Copy temp file to message file
            FileUtilities::Copy(sTempFile, message_file_, false);
            LOG_DEBUG("SA - Copy+Delete used");
         }
	  } 
     else 
     {
		 String logMessage;
		 logMessage.Format(_T("SA: Temp file size did not match what Spamd reported! (temp: %d, spamd: %d). Reverting to original message file."),FileUtilities::FileSize(sTempFile),spam_dsize_);
         LOG_DEBUG(logMessage);
	  }
     
     test_completed_ = true;
   }

   int
   SpamAssassinClient::ParseFirstBuffer_(std::shared_ptr<ByteBuffer> pBuffer) const
   {
      // Don't send first line, since it's the Result header.
      char *pHeaderEndPosition = StringParser::Search(pBuffer->GetCharBuffer(), pBuffer->GetSize(), "\r\n\r\n");
      if (!pHeaderEndPosition)
      {
         LOG_DEBUG("The response from SpamAssasin was not valid. Aborting. Expected a header.\r\n");
         return -1;
      }
            
      size_t headerLength = pHeaderEndPosition - pBuffer->GetCharBuffer();
      AnsiString spamAssassinHeader(pBuffer->GetCharBuffer(), headerLength);

      std::vector<AnsiString> headerLines = StringParser::SplitString(spamAssassinHeader, "\r\n");
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
      std::vector<AnsiString> contentLengthHeader = StringParser::SplitString(secondLine, ":");
      if (contentLengthHeader.size() != 2)
      {
         LOG_DEBUG(Formatter::Format("The response from SpamAssasin was not valid. Aborting. Content-Length header not properly formatted. Expected: Content-Length:<value>, Got: {0}\r\n", secondLine));
         return -1;
      }

      int contentLength;
      std::string sConSize = contentLengthHeader[1].Trim();
      if (!StringParser::TryParseInt(sConSize, contentLength))
      {
        LOG_DEBUG(Formatter::Format("The response from SpamAssasin was not valid. Aborting. Content-Length header not properly formatted. Expected: Content-Length:<value>, Got: {0}\r\n", secondLine));
	     return -1;
      }

      // Remove the SA header lines from the result.
      size_t iEndingBytesSize = pBuffer->GetSize() - headerLength - 4; // 4 due to header ending with \r\n\r\n.
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

   }

   void 
   SpamAssassinClient::Cleanup_()
   {
      if (result_ != nullptr)
      {
         result_->Close();
         FileUtilities::DeleteFile(result_->GetName());
         result_ = nullptr;
      }
   }
}
