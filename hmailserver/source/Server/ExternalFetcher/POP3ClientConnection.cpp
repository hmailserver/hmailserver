// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\POP3ClientConnection.h"

#include "FetchAccountUIDList.h"

#include "../Common/Scripting/Result.h"
#include "../Common/Scripting/Events.h"
#include "../Common/BO/FetchAccount.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/MessageData.h"
#include "../Common/BO/Account.h"
#include "../Common/Persistence/PersistentMessage.h"
#include "../Common/Util/ByteBuffer.h"
#include "../SMTP/RecipientParser.h"
#include "../Common/Util/Parsing/AddressListParser.h"
#include "../Common/Util/Utilities.h"
#include "../Common/Util/ServerStatus.h"
#include "../Common/Mime/Mime.h"
#include "../Common/BO/FetchAccountUID.h"
#include "../Common/BO/MessageRecipients.h"
#include "../common/util/MessageUtilities.h"
#include "../common/Threading/AsynchronousTask.h"
#include "../common/Threading/WorkQueue.h"

#include "../Common/Util/TransparentTransmissionBuffer.h"

#include "../Common/Application/TimeoutCalculator.h"

#include "../Common/Util/VariantDateTime.h"
#include "../Common/Util/Time.h"
#include "../Common/Cache/CacheContainer.h"
#include "../Common/AntiSpam/AntiSpamConfiguration.h"
#include "../Common/AntiSpam/SpamProtection.h"

#include <boost/algorithm/string.hpp>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   POP3ClientConnection::POP3ClientConnection(std::shared_ptr<FetchAccount> pAccount,
                                              ConnectionSecurity connectionSecurity,
                                              boost::asio::io_service& io_service, 
                                              boost::asio::ssl::context& context,
                                              std::shared_ptr<Event> disconnected,
                                              AnsiString remote_hostname) :
      TCPConnection(connectionSecurity, io_service, context, disconnected, remote_hostname),
      account_(pAccount),
      current_state_(StateConnected)
   {

      /*
      RFC 1939, Basic Operation
      A POP3 server MAY have an inactivity autologout timer.  Such a timer
      MUST be of at least 10 minutes' duration.

      But since we're a client, we increase this a bit.
      */

      TimeoutCalculator calculator;
      SetTimeout(calculator.Calculate(IniFileSettings::Instance()->GetPOP3CMinTimeout(), IniFileSettings::Instance()->GetPOP3CMaxTimeout()));
   }

   POP3ClientConnection::~POP3ClientConnection(void)
   {
      
   }

   void
   POP3ClientConnection::OnConnected()
   {
      if (GetConnectionSecurity() == CSNone || 
          GetConnectionSecurity() == CSSTARTTLSOptional ||
          GetConnectionSecurity() == CSSTARTTLSRequired)
         EnqueueRead();
   }

   void
   POP3ClientConnection::OnHandshakeCompleted()
   {
      if (GetConnectionSecurity() == CSSSL)
         EnqueueRead();
      else if (GetConnectionSecurity() == CSSTARTTLSOptional ||
               GetConnectionSecurity() == CSSTARTTLSRequired)
      {
         SendUserName_();
         EnqueueRead();
      }
   }

   AnsiString 
   POP3ClientConnection::GetCommandSeparator() const
   {
      return "\r\n";
   }

   void
   POP3ClientConnection::ParseData(const AnsiString &sRequest)
   {
      command_buffer_.append(sRequest);
      command_buffer_.append("\r\n");

      bool is_awaiting_multiline_response  = current_state_ == StateCAPASent;

      if (is_awaiting_multiline_response)
      {
         bool multiline_response_completed = sRequest == "." ||
                                             !CommandIsSuccessfull_(command_buffer_);

         if (!multiline_response_completed)
         {
            EnqueueRead();
            return;
         }
      }

      bool postReceive = InternalParseData(command_buffer_);

      // The ASCII buffer has been parsed, so we
      // may clear it now.
      command_buffer_.Empty();

      if (postReceive)
         EnqueueRead();
   }

   bool
   POP3ClientConnection::InternalParseData(const String &sRequest)
   {
      // This code is temporary home of ETRN client settings in GUI
      // It checks External Account for ETRN domain.com for name
      // and if found uses that info to perform ETRN client connections
      String sAccountName = account_->GetName();
      if (sAccountName.StartsWith(_T("ETRN")))
      {
         HandleEtrn_(sRequest, sAccountName);
         return true;
      }
      else
      {
          // No sense in indenting code below inward as this is temp
          // and it'd just have to be moved back.
          // **** Don't miss } below when removing the above code! ****

         LogPOP3String_(sRequest, false);

         bool bRetVal = true;
         switch (current_state_)
         {
         case StateConnected:
            ParseStateConnected_(sRequest);
            return true;
         case StateCAPASent:
            ParseStateCAPASent_(sRequest);
            return true;
         case StateSTLSSent:
            return ParseStateSTLSSent_(sRequest);
         case StateUsernameSent:
            ParseUsernameSent_(sRequest);
            return true;
         case StatePasswordSent:
            ParsePasswordSent_(sRequest);
            return true;
         case StateUIDLRequestSent:
            ParseUIDLResponse_(sRequest);
            return true;
         case StateQUITSent:
            return ParseQuitResponse_(sRequest);
         case StateDELESent:
            ParseDELEResponse_(sRequest);
            return true;
         }
   
         // This will be removed too when ETRN code is moved
       }

      return true;
   }

   bool
   POP3ClientConnection::HandleEtrn_(const String &sRequest, const String &account_name)
   {
      LogSMTPString_(sRequest, false);

      std::vector<String> vecParams = StringParser::SplitString(account_name, " ");
      if (vecParams.size() == 2)
      {
         bool bRetVal = true;
         switch (current_state_)
         {
            // Re-using POP states names for now
         case StateConnected:
            // Realize we shouldn't blindly send but this works for now
            EnqueueWrite_LogAsSMTP("HELO " + vecParams[1]);
            current_state_ = StateUsernameSent;
            return true;
         case StateUsernameSent:
            EnqueueWrite_LogAsSMTP("ETRN " + vecParams[1]);
            Sleep(20);
            current_state_ = StateUIDLRequestSent;
            return true;
         case StateUIDLRequestSent:
            EnqueueWrite_LogAsSMTP("QUIT");
            current_state_ = StateQUITSent;
            Sleep(20);
            return true;

         default:
            return false;
         }
      }
      else
      {
         //We just log error & QUIT because we have no domain to send..
         EnqueueWrite_("NOOP ETRN-Domain not set");
         Sleep(20);
         EnqueueWrite_("QUIT");
         ParseQuitResponse_(sRequest);
         return false;
      }
   }

   void
   POP3ClientConnection::ParseStateConnected_(const String &sData)
   {
      if (CommandIsSuccessfull_(sData))
      {
         if (GetConnectionSecurity() == CSSTARTTLSOptional ||
             GetConnectionSecurity() == CSSTARTTLSRequired)
         {
            SendCAPA_();
            return;
         }
         else
         {
            SendUserName_();
            return;
         }
      }

      // Disconnect immediately.;
      QuitNow_();
      return;
   }

   void
   POP3ClientConnection::SendUserName_()
   {
      // We have connected successfully.
      // Time to send the username.

      String sResponse;
      sResponse.Format(_T("USER %s"), account_->GetUsername().c_str());

      EnqueueWrite_(sResponse);

      current_state_ = StateUsernameSent;
   }

   void
   POP3ClientConnection::SendCAPA_()
   {
      // We have connected successfully.
      // Time to send the username.
      EnqueueWrite_(_T("CAPA"));

      current_state_ = StateCAPASent;
   }

   void
   POP3ClientConnection::ParseStateCAPASent_(const String &sData)
   {
      if (!CommandIsSuccessfull_(sData) || !sData.Contains(_T("STLS")))
      {
         // STLS is not supported.
         if (GetConnectionSecurity() == CSSTARTTLSRequired)
         {
            String message = 
               Formatter::Format("The download of messages from external account {0} failed. The external aAccount is configured to use STARTTLS connection security, but the POP3 server does not support it.", account_->GetName());
            
            LOG_APPLICATION(message)
            QuitNow_();
            return;
         }
         else
         {
            SendUserName_();
            return;
         }
      }

      EnqueueWrite_("STLS");
      current_state_ = StateSTLSSent;
   }

   bool
   POP3ClientConnection::ParseStateSTLSSent_(const String &sData)
   {
      if (CommandIsSuccessfull_(sData))
      {
         EnqueueHandshake();
         return false;
      }

      // Disconnect immediately.
      QuitNow_();
      return true;
   }

   void 
   POP3ClientConnection::ParseUsernameSent_(const String &sData)
   {
      if (CommandIsSuccessfull_(sData))
      {
         // We have connected successfully.
         // Time to send the username.

         String sResponse;
         sResponse.Format(_T("PASS %s"), account_->GetPassword().c_str());

         current_state_ = StatePasswordSent;

         EnqueueWrite_(sResponse);

         return;
      }

      QuitNow_();
      return;
   }

   void 
   POP3ClientConnection::ParsePasswordSent_(const String &sData)
   {
      if (CommandIsSuccessfull_(sData))
      {
         current_state_ = StateUIDLRequestSent;

         SetReceiveBinary(true);

         // We have connected successfully.
         // Time to send the username.
         String sResponse;
         sResponse.Format(_T("UIDL"));
         
         EnqueueWrite_(sResponse);
         return;
      }

      QuitNow_();
      return;
   }

   void 
   POP3ClientConnection::ParseUIDLResponse_(const String &sData)
   {
      if (CommandIsSuccessfull_(sData))
      {
         // We have connected successfully.
         // Time to send the username.
         
         std::vector<String> vecLines = StringParser::SplitString(sData, "\r\n");
         auto iter = vecLines.begin();

         if (vecLines.size() < 3)
         {
            StartMailboxCleanup_();
            return;
         }

         // Move to first line containing a message ID.
         iter++;

         while (iter != vecLines.end())
         {
            String sLine = (*iter);

            if (sLine == _T("."))
               break;

            int iSpacePos = sLine.Find(_T(" "));
            String sMessageIndex = sLine.Mid(0,iSpacePos);
            String sMessageUID = sLine.Mid(iSpacePos + 1);

            int iMessageIdx = _ttoi(sMessageIndex);
            uidlresponse_[iMessageIdx] = sMessageUID;

            iter++;
         }

         cur_message_ = uidlresponse_.begin();

         RequestNextMessage_();

         return;
      }

      QuitNow_();
   }

   bool
   POP3ClientConnection::RequestNextMessage_()
   {
      while (cur_message_ != uidlresponse_.end())
      {
         String sCurrentUID = (*cur_message_).second;

         // Check if the current message is already in the list
         // of fetch UID's

         bool bMessageDownloaded = GetUIDList_()->IsUIDInList(sCurrentUID);

         if (bMessageDownloaded)
         {  
            // Mark this message as downloaded. This is so that we can
            // drop it later on when purging the mailbox. (We only purge
            // items we have downloaded). And since it was downloaded during
            // a previous session, we can safely drop it..
            int iID = (*cur_message_).first;
            downloaded_messages_[iID] = sCurrentUID;

            // The message has already been downloaded. Give scripts a chance
            // to override the default delete behavior.
            std::shared_ptr<Message> messageEmpty;
            FireOnExternalAccountDownload_(messageEmpty, sCurrentUID);
         }
         else
         {
            // Request message download now.

            current_message_ = std::shared_ptr<Message> (new Message);

            int iMessageIdx = (*cur_message_).first;

            String sResponse;
            sResponse.Format(_T("RETR %d"), iMessageIdx);

            EnqueueWrite_(sResponse);

            current_state_ = StateRETRSent;

            // Reset the transmission buffer. It will be
            // recreated when we receive binary the next time.

            transmission_buffer_.reset();

            SetReceiveBinary(true);
                          
            return true;
         }
      
         cur_message_++;

      }

      // We reached the end of the message list.
      if (cur_message_ == uidlresponse_.end())
      {
         StartMailboxCleanup_();
      }


      return false;
   }

   void
   POP3ClientConnection::StartMailboxCleanup_()
   {
      cur_message_ = downloaded_messages_.begin();
      SetReceiveBinary(false);

      MailboxCleanup_();
   }

   void
   POP3ClientConnection::MailboxCleanup_()
   {
      while (cur_message_ != downloaded_messages_.end())
      {
         bool bRet = MessageCleanup_();

         cur_message_++;

         if (bRet)
         {
            // MessageCleanup_ said something to the
            // remote server. We have to return here
            // to receive the response.

            return;
         }
      }

      DeleteUIDsNoLongerOnServer_();

      // Cleanup is complete. Time to quit.
      LOG_DEBUG("POP3 External Account: Normal QUIT.");
      QuitNow_();
     
   }

   void
   POP3ClientConnection::DeleteUIDsNoLongerOnServer_()
   {
      // Delete UID's from the database of those
      // messages that no longer exists on the
      // remote POP3 server. This happens if hMailServer
      // has downloaded a message and than the user has
      // deleted it from the POP3 server.
      std::shared_ptr<FetchAccountUIDList> uidList = GetUIDList_();

      // Build a vector with the UID's to keep. All UID's
      // not in this list should be deleted from the database.

      std::set<String> setUIDs;

      auto iter = uidlresponse_.begin();
      auto iterEnd = uidlresponse_.end();

      for (; iter != iterEnd; iter++)
         setUIDs.insert((*iter).second);

      uidList->DeleteUIDsNotInSet(setUIDs);
   }

   void
   POP3ClientConnection::QuitNow_()
   {
      String sResponse;
      sResponse.Format(_T("QUIT"));
   
      EnqueueWrite_(sResponse);

      SetReceiveBinary(false);
      current_state_ = StateQUITSent;
   }

   bool 
   POP3ClientConnection::ParseQuitResponse_(const String &sData)
   {
      if (CommandIsSuccessfull_(sData))
      {
         // We have quitted successfully.
         EnqueueDisconnect();
      }

      // Quit anyway
      return false;
   }

   void 
   POP3ClientConnection::ParseRETRResponse_(const String &sData)
   {
      if (CommandIsSuccessfull_(sData))
      {
         // Log that this message has been downloaded.
         int iID = (*cur_message_).first;
         String sCurrentUID = (*cur_message_).second;
         downloaded_messages_[iID] = sCurrentUID;

         return;
      }

      SetReceiveBinary(false);
      
      // Do a mailbox cleanup and disconnect after that.
      StartMailboxCleanup_();
   }


   void 
   POP3ClientConnection::ParseDELEResponse_(const String &sData)
   {
      // Clean up the next message.
      MailboxCleanup_();

      return;
   }


   bool
   POP3ClientConnection::CommandIsSuccessfull_(const String &sData)
   {
      if (sData.Mid(0,3).CompareNoCase(_T("+OK")) == 0)
         return true;
      else
         return false;
   }

   void
   POP3ClientConnection::OnConnectionTimeout()
   {  
      String sMessage = "QUIT\r\n";
      EnqueueWrite_(sMessage);
   }

   
   void
   POP3ClientConnection::OnExcessiveDataReceived()
   {  

   }


   void
   POP3ClientConnection::EnqueueWrite_(const String &sData) 
   {
      LogPOP3String_(sData, true);

      EnqueueWrite(sData + "\r\n");
   }

   bool 
   POP3ClientConnection::ParseFirstBinary_(std::shared_ptr<ByteBuffer> pBuf)
   {
      // Locate the first line
      const char *pText = pBuf->GetCharBuffer();
      const char *pEndOfLine = StringParser::Search(pText, pBuf->GetSize(), "\r\n");

      if (!pEndOfLine)
      {
         // Wait for more data
         return false;
      }

      // Skip passed the end of the line
      pEndOfLine += 2;

      size_t iLineLength = pEndOfLine - pText;

      // Copy the first line from the binary buffer.
      AnsiString sLine;
      sLine.append(pText, iLineLength);
      
      LogPOP3String_(sLine, false);
      
      ParseRETRResponse_(sLine);

      size_t iRemaining = pBuf->GetSize() - iLineLength;
      pBuf->Empty(iRemaining);

      return true;
   }

   void 
   POP3ClientConnection::PrependHeaders_()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Adds headers to the beginning of the message.
   //---------------------------------------------------------------------------()
   {
      // Add a header with the name of the external account, so that
      // we can check where we downloaded it from later on.

      String sHeader;
      sHeader.Format(_T("X-hMailServer-ExternalAccount: %s\r\n"), account_->GetName().c_str());

      AnsiString sAnsiHeader = sHeader;

      transmission_buffer_->Append((BYTE*) sAnsiHeader.GetBuffer(), sAnsiHeader.GetLength());
   }

   void
   POP3ClientConnection::ParseData(std::shared_ptr<ByteBuffer> pBuf)
   {
      // 
      if (current_state_ == StateUIDLRequestSent)
      {
         command_buffer_.append(pBuf->GetCharBuffer(), pBuf->GetSize());

         pBuf->Empty();

         if (command_buffer_.StartsWith("-ERR"))
         {
            // The server does not support UIDL. We can't fetch from this server.
            LogPOP3String_(command_buffer_, false);
            QuitNow_();
            return;
         }

         if (command_buffer_ == ".\r\n" || command_buffer_.EndsWith("\r\n.\r\n"))
         {
            ParseUIDLResponse_(command_buffer_);

            command_buffer_.clear();
         }

         EnqueueRead("");
         return;
      }

      // Append message buffer with the binary data we've received.
      if (!transmission_buffer_)
      {
         if (_firstRetrResponseBuffer == nullptr)
            _firstRetrResponseBuffer = std::make_shared<ByteBuffer>();

         _firstRetrResponseBuffer->Add(pBuf);

         if (!ParseFirstBinary_(_firstRetrResponseBuffer))
         {
            EnqueueRead("");
            return;
         }

         pBuf->Empty();
         pBuf->Add(_firstRetrResponseBuffer);
         _firstRetrResponseBuffer->Empty();

         String fileName = PersistentMessage::GetFileName(current_message_);

         // Create a binary buffer for this message. 
         transmission_buffer_ = std::shared_ptr<TransparentTransmissionBuffer>(new TransparentTransmissionBuffer(false));
         if (!transmission_buffer_->Initialize(fileName))
         {
            // We have probably failed to create the file...
            LOG_DEBUG("POP3 External Account: Error creating binary buffer or file.");
            QuitNow_();
            return;
         }

         PrependHeaders_();
      }

      transmission_buffer_->Append(pBuf->GetBuffer(), pBuf->GetSize());
      transmission_buffer_->Flush();

      // Clear the binary buffer.
      pBuf->Empty();

      if (!transmission_buffer_->GetTransmissionEnded())
      {
         EnqueueRead("");
         return;
      }

      // Since this may be a time-consuming task, do it asynchronously
      std::shared_ptr<AsynchronousTask<TCPConnection> > finalizationTask = 
         std::shared_ptr<AsynchronousTask<TCPConnection> >(new AsynchronousTask<TCPConnection>
         (std::bind(&POP3ClientConnection::HandlePOP3FinalizationTaskCompleted_, this), shared_from_this()));

      Application::Instance()->GetAsyncWorkQueue()->AddTask(finalizationTask);
   }

   void
   POP3ClientConnection::HandlePOP3FinalizationTaskCompleted_()
   {
      // The entire message has now been downloaded from the
      // remote POP3 server. Save it in the database and deliver
      // it to the account.
      String fileName = PersistentMessage::GetFileName(current_message_);
      current_message_->SetSize(FileUtilities::FileSize(fileName));

      if (current_message_->GetSize() == 0)
      {
         // Error handling.
         LOG_DEBUG("POP3 External Account: Message is 0 bytes.");
         QuitNow_();
         return;
      }

      ParseMessageHeaders_();

      if (DoSpamProtection_())
      {
         // should we scan this message for virus later on?
         current_message_->SetFlagVirusScan(account_->GetUseAntiVirus());

         FireOnExternalAccountDownload_(current_message_, (*cur_message_).second);

         // the message was not classified as spam which we should delete.
         SaveMessage_();

         // Notify the SMTP deliverer that there is a new message.
         Application::Instance()->SubmitPendingEmail();
      }

      MarkCurrentMessageAsRead_();

      // Switch to ASCII since we're going to request a new message.
      SetReceiveBinary(false);

      // Move on to the next message to download
      cur_message_++;

      RequestNextMessage_();
   
      EnqueueRead("");
   }

   /*
      Run spam proteciton on this message. If it's classified as spam, we will either
      delete it, or we'll tag it as spam.
   */
   bool
   POP3ClientConnection::DoSpamProtection_()
   {
      if (!account_->GetUseAntiSpam())
      {
         // spam protection isn't enabled.
         return true;
      }

      String fileName = PersistentMessage::GetFileName(current_message_);

      IPAddress ipAddress;
      String hostName;

      String senderAddress = current_message_->GetFromAddress();
      MessageUtilities::RetrieveOriginatingAddress(current_message_, hostName, ipAddress);
      // The received header isn't safely parseable so we will always do anti-spam,


      if (SpamProtection::IsWhiteListed(senderAddress, ipAddress))
         return true;

      std::set<std::shared_ptr<SpamTestResult> > setSpamTestResults;
      
      // Run PreTransmissionTests. These consists of light tests such as DNSBL/SPF checks.
      std::set<std::shared_ptr<SpamTestResult> > setResult = 
           SpamProtection::Instance()->RunPreTransmissionTests(senderAddress, ipAddress, ipAddress, hostName);

      setSpamTestResults.insert(setResult.begin(), setResult.end());

      // Run PostTransmissionTests. These consists of more heavy stuff such as SURBL and SpamAssassin-
      setResult =
         SpamProtection::Instance()->RunPostTransmissionTests(senderAddress, ipAddress, ipAddress, current_message_);

      setSpamTestResults.insert(setResult.begin(), setResult.end());
      
      int iTotalSpamScore = SpamProtection::CalculateTotalSpamScore(setSpamTestResults);

      if (iTotalSpamScore >= Configuration::Instance()->GetAntiSpamConfiguration().GetSpamDeleteThreshold())
      {
         FileUtilities::DeleteFile(fileName);
         return false;
      }
      
      bool classifiedAsSpam = iTotalSpamScore >= Configuration::Instance()->GetAntiSpamConfiguration().GetSpamMarkThreshold();
      
      if (classifiedAsSpam)
      {
         std::shared_ptr<MessageData> messageData = SpamProtection::AddSpamScoreHeaders(current_message_, setSpamTestResults, classifiedAsSpam);
         
         // Increase the spam-counter
         ServerStatus::Instance()->OnSpamMessageDetected();

         if (messageData)
            messageData->Write(fileName);
      }

      return true;
   }

   void 
   POP3ClientConnection::ParseMessageHeaders_()
   {
      assert(current_message_);

      String fileName = PersistentMessage::GetFileName(current_message_);

      AnsiString sHeader = PersistentMessage::LoadHeader(fileName);
      std::shared_ptr<MimeHeader> pHeader = std::shared_ptr<MimeHeader>(new MimeHeader);
      pHeader->Load(sHeader, sHeader.GetLength());

      {
         // Parse out the sender of the message. 
         String sFrom = pHeader->GetRawFieldValue("From");

         if (!sFrom.IsEmpty())
         {
            AddresslistParser oAddressParser;

            String sFullName, sUser, sDomain;
            oAddressParser.ExtractParts(sFrom, sFullName, sUser, sDomain);

            if (!sUser.IsEmpty() && !sDomain.IsEmpty())
            {
               sFrom = sUser + "@" + sDomain;
               if (StringParser::IsValidEmailAddress(sFrom))
                  current_message_->SetFromAddress(sFrom);
            }
         }
      }      

      // bool bPreprocessRecipientList = true;
      CreateRecipentList_(pHeader);

      // Remove non-existent accounts.
      RemoveInvalidRecipients_();

      RetrieveReceivedDate_(pHeader);
   }

   void
   POP3ClientConnection::SaveMessage_()
   {
      if (current_message_->GetRecipients()->GetCount() > 0)
      {
         current_message_->SetState(Message::Delivering);

         PersistentMessage::SaveObject(current_message_);
      }


   }

   void 
   POP3ClientConnection::MarkCurrentMessageAsRead_()
   {
      if (cur_message_ != uidlresponse_.end())
      {
         String sUID = (*cur_message_).second;

         // If we're deleting this message immediately, there's
         // no point in adding it to the table.
         if (GetDaysToKeep_(sUID) != -1)
         {
            // Make sure that the UID exists in the database.
            // If it already exists, AddUID() will do nothing.
            GetUIDList_()->AddUID(sUID); 
         }

      }
   }

   bool
   POP3ClientConnection::MessageCleanup_()
   {
      int iIndex = (*cur_message_).first;
      String sUID = (*cur_message_).second;

      int iDaysToKeep = GetDaysToKeep_(sUID);

      String sResponse;
      bool bDeleteMessageNow = false;

      if (iDaysToKeep == 0)
      {
         // Never delete messages
         return false;
      }
      else if (iDaysToKeep > 0)
      {
         // Check wether we should delete this UID.
         std::shared_ptr<FetchAccountUID> pUID = GetUIDList_()->GetUID(sUID);

         // Get the creation date of the UID.
         DateTime dtCreation = pUID->GetCreationDate();
         DateTime dtNow = Time::GetDateFromSystemDate(Time::GetCurrentDateTime());

         DateTimeSpan dtSpan = dtNow - dtCreation;

         if (dtSpan.GetNumberOfDays() <= iDaysToKeep)
         {
            // No, we should not delete this UID.
            return false;
         }
      }

      // Delete the message.
      sResponse.Format(_T("DELE %d"), iIndex);
      EnqueueWrite_(sResponse);

      current_state_ = StateDELESent;

      // Delete this UID from the database.
      GetUIDList_()->DeleteUID(sUID);
     

      return true;
   }

   void 
   POP3ClientConnection::CreateRecipentList_(std::shared_ptr<MimeHeader> pHeader)
   {
      if (account_->GetProcessMIMERecipients() && !account_->GetMIMERecipientHeaders().IsEmpty())
      {  
         ProcessMIMERecipients_(pHeader);
      }  

      if (current_message_->GetRecipients()->GetCount() > 0)
         return;
      
      // Just fetch the account
      if (receiving_account_address_.IsEmpty())
      {
         std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(account_->GetAccountID());
         if (pAccount)
         {
            receiving_account_address_ = pAccount->GetAddress();
         }
      }

      // Add the recipient to the message
      bool recipientOK = false;
      RecipientParser recipientParser;
      recipientParser.CreateMessageRecipientList(receiving_account_address_, current_message_->GetRecipients(), recipientOK);
   }

   void
   POP3ClientConnection::ProcessMIMERecipients_(std::shared_ptr<MimeHeader> pHeader)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Goes through headers in the email and locates recipient. Adds these recipients
   // to the message
   //---------------------------------------------------------------------------()
   {
      /*
      String sTo = pHeader->GetRawFieldValue("To");
      String sCC = pHeader->GetRawFieldValue("CC");
      String sXRCPTTo = pHeader->GetRawFieldValue("X-RCPT-TO");
      String sXEnvelopeTo = pHeader->GetRawFieldValue("X-Envelope-To");

      String sAllRecipients = sTo + "," + sCC + "," + sXRCPTTo + "," + sXEnvelopeTo;
      */

      AnsiString sMimeRecipientHeaders = account_->GetMIMERecipientHeaders();
      std::vector<std::string> sMimeRecipientHeader;
      std::vector<std::string> sMimeRecipientsList;
      boost::split(sMimeRecipientHeader, sMimeRecipientHeaders, boost::is_any_of(";, "), boost::token_compress_on);
      for (std::vector<std::string>::iterator it = sMimeRecipientHeader.begin(); it != sMimeRecipientHeader.end(); ++it)
      {
         auto value = pHeader->GetRawFieldValue(*it);
         if (value)
         {
            sMimeRecipientsList.push_back(value);
         }
      }
      String sAllRecipients = boost::join(sMimeRecipientsList, ",");

      // Parse this list.
      AddresslistParser oListParser;
      std::vector<std::shared_ptr<Address> > vecAddresses = oListParser.ParseList(sAllRecipients);
      auto iterAddress = vecAddresses.begin();

      RecipientParser recipientParser;
      while (iterAddress != vecAddresses.end())
      {
         std::shared_ptr<Address> pAddress = (*iterAddress);

         if (pAddress->sMailboxName.IsEmpty() || pAddress->sDomainName.IsEmpty())
         {
            iterAddress++;
            continue;
         }

         String sAddress = pAddress->sMailboxName + "@" + pAddress->sDomainName;

         // Add the recipient to the message
         bool recipientOK = false;
         recipientParser.CreateMessageRecipientList(sAddress, current_message_->GetRecipients(), recipientOK);

         iterAddress++;
      }

      // Go through the Received headers
      ProcessReceivedHeaders_(pHeader);

      // Remove non-existent accounts.
      RemoveInvalidRecipients_();
}

   void 
   POP3ClientConnection::RetrieveReceivedDate_(std::shared_ptr<MimeHeader> pHeader)
   {
      if (!account_->GetProcessMIMEDate())
         return;

      String sReceivedHeader = pHeader->GetRawFieldValue("Received");

      DateTime dtTime = Utilities::GetDateTimeFromReceivedHeader(sReceivedHeader);

      if (dtTime.GetYear() < 1980 || dtTime.GetYear() > 2040)
         return;

      long minutes = Time::GetUTCRelationMinutes();
      DateTimeSpan dtSpan;
      dtSpan.SetDateTimeSpan(0, 0, minutes, 0);
      dtTime = dtTime + dtSpan;     

      String sDate = Time::GetTimeStampFromDateTime(dtTime);

      

      current_message_->SetCreateTime(sDate);
   }

   void 
   POP3ClientConnection::ProcessReceivedHeaders_(std::shared_ptr<MimeHeader> pHeader)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Goes throguh all the "Received" headers of the email. Tries to parse the
   // addresses and deliver the message to the recipients specified in them.
   //---------------------------------------------------------------------------()
   {
      AnsiString sHeaderName = "Received";
      std::vector<MimeField> &lstFields = pHeader->Fields();
      auto iter = lstFields.begin();
      auto iterEnd = lstFields.end();

      RecipientParser recipientParser;
      for (; iter != iterEnd; iter++)
      {
         MimeField& fd = *iter;

         if (sHeaderName.CompareNoCase(fd.GetName()) == 0)
         {
            String sValue = fd.GetValue();

            String sRecipient = Utilities::GetRecipientFromReceivedHeader(sValue);

            if (!sRecipient.IsEmpty())
            {
               bool recipientOK = false;
               recipientParser.CreateMessageRecipientList(sRecipient, current_message_->GetRecipients(), recipientOK);
            }

         }
      }
   }

   void
   POP3ClientConnection::LogPOP3String_(const String &sLogString, bool bSent)
   {
      String sTemp;

      if (bSent)
      {
         // Check if we should remove the password.
         if (current_state_ == StatePasswordSent)
         {
            // Remove password.
            sTemp = "SENT: ***";
         }
         else
         {
            sTemp = "SENT: " + sLogString;
         }
      }
      else
         sTemp = "RECEIVED: " + sLogString;

      sTemp.TrimRight(_T("\r\n"));

      LOG_POP3_CLIENT(GetSessionID(), GetIPAddressString(), sTemp);
   }
   
   void 
   POP3ClientConnection::OnCouldNotConnect(const AnsiString &sErrorDescription)
   {
      LOG_DEBUG("Connection to remote POP3-server failed. Error message: " + String(sErrorDescription) );
   }

   void 
   POP3ClientConnection::RemoveInvalidRecipients_()
   {
      if (account_->GetEnableRouteRecipients())
         current_message_->GetRecipients()->RemoveExternal();
      else
         current_message_->GetRecipients()->RemoveNonLocalAccounts();
   }

   int
   POP3ClientConnection::GetDaysToKeep_(const String &sUID)
   {
      int iDaysToKeep = account_->GetDaysToKeep();

      // Has an event overriden when messages should be deleted?
      if (event_results_.find(sUID) != event_results_.end())
      {
         std::shared_ptr<Result> result = event_results_[sUID];

         switch (result->GetValue())
         {
         case 1:
            // Delete messages immediately.
            iDaysToKeep = -1;
            break;
         case 2:
            // Delete messages after a specified number of days.
            iDaysToKeep = result->GetParameter();
            break;
         case 3:
            // Never delete messages.
            iDaysToKeep = 0;
            break;
         }
      }

      return iDaysToKeep;
   }

   /// Fires an event which lets the user override the default delete-behavior on the remote server.
   void
   POP3ClientConnection::FireOnExternalAccountDownload_(std::shared_ptr<Message> message, const String &uid)
   {
      std::shared_ptr<Result> pResult = Events::FireOnExternalAccountDownload(account_, message, uid);

      if (pResult)
         event_results_[uid] = pResult;
   }

   std::shared_ptr<FetchAccountUIDList>
   POP3ClientConnection::GetUIDList_()
   {
      if (!fetch_account_uidlist_)
      {
         fetch_account_uidlist_ = std::shared_ptr<FetchAccountUIDList>(new FetchAccountUIDList());
         fetch_account_uidlist_->Refresh(account_->GetID());
      }

      return fetch_account_uidlist_;
   }
   // This is temp function to log ETRN client commands to SMTP
   void
   POP3ClientConnection::LogSMTPString_(const String &sLogString, bool bSent)
   {
      String sTemp;

      if (bSent)
      {
         // Check if we should remove the password.
         if (current_state_ == StatePasswordSent)
         {
            // Remove password.
            sTemp = "SENT: ***";
         }
         else
         {
            sTemp = "SENT: " + sLogString;
         }
      }
      else
         sTemp = "RECEIVED: " + sLogString;

      sTemp.TrimRight(_T("\r\n"));

      LOG_SMTP_CLIENT(GetSessionID(), GetIPAddressString(), sTemp);
   }

   // This is temp function to log ETRN client commands to SMTP
   void
   POP3ClientConnection::EnqueueWrite_LogAsSMTP(const String &sData) 
   {
      LogSMTPString_(sData, true);

      EnqueueWrite(sData + "\r\n");
   }

}
