// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\POP3ClientConnection.h"

#include "FetchAccountUIDList.h"

#include "../Common/Application/ScriptingHost/Result.h"
#include "../Common/EventHandlers/Events.h"
#include "../Common/BO/FetchAccount.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/MessageData.h"
#include "../Common/BO/Account.h"
#include "../Common/Persistence/PersistentMessage.h"
#include "../Common/Util/ByteBuffer.h"
#include "../SMTP/RecipientParser.h"
#include "../Common/Util/Parsing/AddressListParser.h"
#include "../Common/Util/Utilities.h"
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

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   POP3ClientConnection::POP3ClientConnection(shared_ptr<FetchAccount> pAccount) :
      m_pAccount(pAccount),
      m_eCurrentState(StateConnected),
      m_bPendingDisconnect(false)
   {
      m_bAwaitingMultilineResponse = false;

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
      PostReceive();
   }

   AnsiString 
   POP3ClientConnection::GetCommandSeparator() const
   {
      return "\r\n";
   }

   void
   POP3ClientConnection::ParseData(const AnsiString &sRequest)
   {
      InternalParseData(sRequest);

      if (m_bPendingDisconnect == false)
         PostReceive();
   }

   void
   POP3ClientConnection::InternalParseData(const String &sRequest)
   {
	  bool commandBufferIsEmpty = m_sCommandBuffer.empty();
	
      m_sCommandBuffer.append(sRequest);
      m_sCommandBuffer.append(_T("\r\n"));

      if (m_bAwaitingMultilineResponse)
      {
         if (sRequest != _T("."))
		 {
			if (commandBufferIsEmpty && !_CommandIsSuccessfull(sRequest))
            {
               // An error has occured. Don't continue multiline
               // buffering, so that we can handle the error below.
            }
            else
            {
               return;
            }
		 }

         m_bAwaitingMultilineResponse = false;
      }

      // This code is temporary home of ETRN client settings in GUI
      // It checks External Account for ETRN domain.com for name
      // and if found uses that info to perform ETRN client connections
      String sTest1 = m_pAccount->GetName();
      if (sTest1.StartsWith(_T("ETRN")))
      {

      _LogSMTPString(m_sCommandBuffer, false);

      std::vector<String> vecParams = StringParser::SplitString(sTest1, " ");
      if (vecParams.size() == 2)
      {

         bool bRetVal = true;
         switch (m_eCurrentState)
            {

            // Re-using POP states names for now
            case StateConnected:
               // Realize we shouldn't blindly send but this works for now
              _SendData2("HELO " + vecParams[1]);
               m_eCurrentState = StateUsernameSent;
               break;

            case StateUsernameSent:
               _SendData2("ETRN " + vecParams[1]);
               Sleep(20);
               m_eCurrentState = StateUIDLRequestSent;
               break;

            case StateUIDLRequestSent:
               _SendData2("QUIT");
               m_eCurrentState = StateQUITSent;
               Sleep(20);
               break;
            }
      }
      else
      {
         //We just log error & QUIT because we have no domain to send..
         _SendData("NOOP ETRN-Domain not set");
         Sleep(20);
         _SendData("QUIT");
         _ParseQuitResponse(m_sCommandBuffer);
       }
      }
      else
      {
       // No sense in indenting code below inward as this is temp
       // and it'd just have to be moved back.
       // **** Don't miss } below when removing the above code! ****

      _LogPOP3String(m_sCommandBuffer, false);

      bool bRetVal = true;
      switch (m_eCurrentState)
      {
      case StateConnected:
         _ParseStateConnected(m_sCommandBuffer);
         break;
      case StateUsernameSent:
         _ParseUsernameSent(m_sCommandBuffer);
         break;
      case StatePasswordSent:
         _ParsePasswordSent(m_sCommandBuffer);
         break;
      case StateUIDLRequestSent:
         _ParseUIDLResponse(m_sCommandBuffer);
         break;
      case StateQUITSent:
         _ParseQuitResponse(m_sCommandBuffer);
         break;
      case StateDELESent:
         _ParseDELEResponse(m_sCommandBuffer);
         break;
      }
// This will be removed too when ETRN code is moved
    }
      // The ASCII buffer has been parsed, so we
      // may clear it now.
      m_sCommandBuffer.Empty();
   }

   void
   POP3ClientConnection::_ParseStateConnected(const String &sData)
   {
      if (_CommandIsSuccessfull(sData))
      {
         // We have connected successfully.
         // Time to send the username.

         String sResponse;
         sResponse.Format(_T("USER %s"), m_pAccount->GetUsername());

         _SendData(sResponse);

         m_eCurrentState = StateUsernameSent;
         return;
      }

      // Disconnect immediately.
      LOG_DEBUG("POP3 External Account: Connection to remote POP3-server failed upon USER command.");
      _QuitNow();
      return;
   }

   void 
   POP3ClientConnection::_ParseUsernameSent(const String &sData)
   {
      if (_CommandIsSuccessfull(sData))
      {
         // We have connected successfully.
         // Time to send the username.

         String sResponse;
         sResponse.Format(_T("PASS %s"), m_pAccount->GetPassword());

         m_eCurrentState = StatePasswordSent;

         _SendData(sResponse);

         return;
      }

      LOG_DEBUG("POP3 External Account: Connection to remote POP3-server failed upon PASS command.");
      _QuitNow();
      return;
   }

   void 
   POP3ClientConnection::_ParsePasswordSent(const String &sData)
   {
      if (_CommandIsSuccessfull(sData))
      {
         m_eCurrentState = StateUIDLRequestSent;

         // We have connected successfully.
         // Time to send the username.
         String sResponse;
         sResponse.Format(_T("UIDL"));
         
         _SendData(sResponse);

         m_bAwaitingMultilineResponse = true;

         return;
      }

      LOG_DEBUG("POP3 External Account: Connection to remote POP3-server failed upon UIDL command.");
      _QuitNow();
      return;
   }

   void 
   POP3ClientConnection::_ParseUIDLResponse(const String &sData)
   {
      if (_CommandIsSuccessfull(sData))
      {
         // We have connected successfully.
         // Time to send the username.
         
         vector<String> vecLines = StringParser::SplitString(sData, "\r\n");
         vector<String>::iterator iter = vecLines.begin();

         if (vecLines.size() < 3)
         {
            _StartMailboxCleanup();
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
            m_mapUIDLResponse[iMessageIdx] = sMessageUID;

            iter++;
         }

         m_iterCurMessage = m_mapUIDLResponse.begin();

         _RequestNextMessage();

         return;
      }

	  ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5343, "POP3ClientConnection::_ParseUIDLResponse",
         Formatter::Format("The remote server returned an error to the UIDL command: External account {0}, Message: {1}", m_pAccount->GetName(), sData));
 
      _QuitNow();
   }

   bool
   POP3ClientConnection::_RequestNextMessage()
   {
      while (m_iterCurMessage != m_mapUIDLResponse.end())
      {
         String sCurrentUID = (*m_iterCurMessage).second;

         // Check if the current message is already in the list
         // of fetch UID's

         bool bMessageDownloaded = _GetUIDList()->IsUIDInList(sCurrentUID);

         if (bMessageDownloaded)
         {  
            // Mark this message as downloaded. This is so that we can
            // drop it later on when purging the mailbox. (We only purge
            // items we have downloaded). And since it was downloaded during
            // a previous session, we can safely drop it..
            int iID = (*m_iterCurMessage).first;
            m_mapDownloadedMessages[iID] = sCurrentUID;

            // The message has already been downloaded. Give scripts a chance
            // to override the default delete behavior.
            shared_ptr<Message> messageEmpty;
            _FireOnExternalAccountDownload(messageEmpty, sCurrentUID);
         }
         else
         {
            // Request message download now.

            m_pCurrentMessage = shared_ptr<Message> (new Message);

            int iMessageIdx = (*m_iterCurMessage).first;

            String sResponse;
            sResponse.Format(_T("RETR %d"), iMessageIdx);

            _SendData(sResponse);

            m_eCurrentState = StateRETRSent;

            // Reset the transmission buffer. It will be
            // recreated when we receive binary the next time.

            m_pTransmissionBuffer.reset();

            SetReceiveBinary(true);
                          
            return true;
         }
      
         m_iterCurMessage++;

      }

      // We reached the end of the message list.
      if (m_iterCurMessage == m_mapUIDLResponse.end())
      {
         _StartMailboxCleanup();
      }


      return false;
   }

   void
   POP3ClientConnection::_StartMailboxCleanup()
   {
      m_iterCurMessage = m_mapDownloadedMessages.begin();

      _MailboxCleanup();
   }

   void
   POP3ClientConnection::_MailboxCleanup()
   {
      while (m_iterCurMessage != m_mapDownloadedMessages.end())
      {
         bool bRet = _MessageCleanup();

         m_iterCurMessage++;

         if (bRet)
         {
            // _MessageCleanup said something to the
            // remote server. We have to return here
            // to receive the response.

            return;
         }
      }

      _DeleteUIDsNoLongerOnServer();

      // Cleanup is complete. Time to quit.
      LOG_DEBUG("POP3 External Account: Normal QUIT.");
      _QuitNow();
     
   }

   void
   POP3ClientConnection::_DeleteUIDsNoLongerOnServer()
   {
      // Delete UID's from the database of those
      // messages that no longer exists on the
      // remote POP3 server. This happens if hMailServer
      // has downloaded a message and than the user has
      // deleted it from the POP3 server.
      shared_ptr<FetchAccountUIDList> uidList = _GetUIDList();

      // Build a vector with the UID's to keep. All UID's
      // not in this list should be deleted from the database.

      set<String> setUIDs;

      map<int ,String>::iterator iter = m_mapUIDLResponse.begin();
      map<int ,String>::iterator iterEnd = m_mapUIDLResponse.end();

      for (; iter != iterEnd; iter++)
         setUIDs.insert((*iter).second);

      uidList->DeleteUIDsNotInSet(setUIDs);
   }

   void
   POP3ClientConnection::_QuitNow()
   {
      String sResponse;
      sResponse.Format(_T("QUIT"));
   
      _SendData(sResponse);

      SetReceiveBinary(false);
      m_eCurrentState = StateQUITSent;
   }

   void 
   POP3ClientConnection::_ParseQuitResponse(const String &sData)
   {
      if (_CommandIsSuccessfull(sData))
      {
         // We have quitted successfully.
         // Disconnect
         m_bPendingDisconnect = true;
         PostDisconnect();
         return;
      }

      // Quit anyway
      return;
   }

   void 
   POP3ClientConnection::_ParseRETRResponse(const String &sData)
   {
      if (_CommandIsSuccessfull(sData))
      {
         // Log that this message has been downloaded.
         int iID = (*m_iterCurMessage).first;
         String sCurrentUID = (*m_iterCurMessage).second;
         m_mapDownloadedMessages[iID] = sCurrentUID;

         return;
      }

      SetReceiveBinary(false);
      
      // Do a mailbox cleanup and disconnect after that.
      _StartMailboxCleanup();
   }


   void 
   POP3ClientConnection::_ParseDELEResponse(const String &sData)
   {
      // Clean up the next message.
      _MailboxCleanup();

      return;
   }


   bool
   POP3ClientConnection::_CommandIsSuccessfull(const String &sData)
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
      _SendData(sMessage);
      
      Logger::Instance()->LogDebug("POP3ClientConnection::OnConnectionTimeout() - Connection timeout.");
   }

   
   void
   POP3ClientConnection::OnExcessiveDataReceived()
   {  

   }


   void
   POP3ClientConnection::_SendData(const String &sData) 
   {
      _LogPOP3String(sData, true);

      SendData(sData + "\r\n");
   }

   bool 
   POP3ClientConnection::_ParseFirstBinary(shared_ptr<ByteBuffer> pBuf)
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

      int iLineLength = pEndOfLine - pText;

      if (iLineLength <= 0)
      {
         // Wait for more data
         return false;
      }

      // Copy the first line from the binary buffer.
      AnsiString sLine;
      sLine.append(pText, iLineLength);
      
      _LogPOP3String(sLine, false);
      
      _ParseRETRResponse(sLine);

      int iRemaining = pBuf->GetSize() - iLineLength;
      pBuf->Empty(iRemaining);

      return true;
   }

   void 
   POP3ClientConnection::_PrependHeaders()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Adds headers to the beginning of the message.
   //---------------------------------------------------------------------------()
   {
      // Add a header with the name of the external account, so that
      // we can check where we downloaded it from later on.

      String sHeader;
      sHeader.Format(_T("X-hMailServer-ExternalAccount: %s\r\n"), m_pAccount->GetName());

      AnsiString sAnsiHeader = sHeader;

      m_pTransmissionBuffer->Append((BYTE*) sAnsiHeader.GetBuffer(), sAnsiHeader.GetLength());
   }

   void
   POP3ClientConnection::ParseData(shared_ptr<ByteBuffer> pBuf)
   {
      // Append message buffer with the binary data we've received.
      if (!m_pTransmissionBuffer)
      {
         if (!_ParseFirstBinary(pBuf))
         {
            PostBufferReceive();
            return;
         }

         String fileName = PersistentMessage::GetFileName(m_pCurrentMessage);

         // Create a binary buffer for this message. 
         m_pTransmissionBuffer = shared_ptr<TransparentTransmissionBuffer>(new TransparentTransmissionBuffer(false));
         if (!m_pTransmissionBuffer->Initialize(fileName))
         {
            // We have probably failed to create the file...
            LOG_DEBUG("POP3 External Account: Error creating binary buffer or file.");
            _QuitNow();
            return;
         }

         _PrependHeaders();
      }

      m_pTransmissionBuffer->Append(pBuf->GetBuffer(), pBuf->GetSize());
      m_pTransmissionBuffer->Flush();

      // Clear the binary buffer.
      pBuf->Empty();

      if (!m_pTransmissionBuffer->GetTransmissionEnded())
      {
         PostBufferReceive();
         return;
      }

      // Since this may be a time-consuming task, do it asynchronously
      shared_ptr<AsynchronousTask<TCPConnection> > finalizationTask = 
         shared_ptr<AsynchronousTask<TCPConnection> >(new AsynchronousTask<TCPConnection>
         (boost::bind(&POP3ClientConnection::_HandlePOP3FinalizationTaskCompleted, shared_from_this()), GetTCPConnectionTemporaryPointer()));

      Application::Instance()->GetAsyncWorkQueue()->AddTask(finalizationTask);
   }

   void
   POP3ClientConnection::_HandlePOP3FinalizationTaskCompleted()
   {
      // The entire message has now been downloaded from the
      // remote POP3 server. Save it in the database and deliver
      // it to the account.
      String fileName = PersistentMessage::GetFileName(m_pCurrentMessage);
      m_pCurrentMessage->SetSize(FileUtilities::FileSize(fileName));

      if (m_pCurrentMessage->GetSize() == 0)
      {
         // Error handling.
         LOG_DEBUG("POP3 External Account: Message is 0 bytes.");
         _QuitNow();
         return;
      }

      _ParseMessageHeaders();

      if (_DoSpamProtection())
      {
         // should we scan this message for virus later on?
         m_pCurrentMessage->SetFlagVirusScan(m_pAccount->GetUseAntiVirus());

         _FireOnExternalAccountDownload(m_pCurrentMessage, (*m_iterCurMessage).second);

         // the message was not classified as spam which we should delete.
         _SaveMessage();

         // Notify the SMTP deliverer that there is a new message.
         Application::Instance()->SubmitPendingEmail();
      }

      _MarkCurrentMessageAsRead();

      // Switch to ASCII since we're going to request a new message.
      SetReceiveBinary(false);

      // Move on to the next message to download
      m_iterCurMessage++;

      _RequestNextMessage();
   
      PostBufferReceive();
   }

   /*
      Run spam proteciton on this message. If it's classified as spam, we will either
      delete it, or we'll tag it as spam.
   */
   bool
   POP3ClientConnection::_DoSpamProtection()
   {
      if (!m_pAccount->GetUseAntiSpam())
      {
         // spam protection isn't enabled.
         return true;
      }

      String fileName = PersistentMessage::GetFileName(m_pCurrentMessage);

      IPAddress ipAddress;
      String hostName;

      String senderAddress = m_pCurrentMessage->GetFromAddress();
      MessageUtilities::RetrieveOriginatingAddress(m_pCurrentMessage, hostName, ipAddress);
      // The received header isn't safely parseable so we will always do anti-spam,


      if (SpamProtection::IsWhiteListed(senderAddress, ipAddress))
         return true;

      set<shared_ptr<SpamTestResult> > setSpamTestResults;
      
      // Run PreTransmissionTests. These consists of light tests such as DNSBL/SPF checks.
      set<shared_ptr<SpamTestResult> > setResult = 
           SpamProtection::Instance()->RunPreTransmissionTests(senderAddress, ipAddress, ipAddress, hostName);

      setSpamTestResults.insert(setResult.begin(), setResult.end());

      int iTotalSpamScore = SpamProtection::CalculateTotalSpamScore(setSpamTestResults);

      if (iTotalSpamScore >= Configuration::Instance()->GetAntiSpamConfiguration().GetSpamDeleteThreshold())
      {
         FileUtilities::DeleteFile(fileName);
         return false;
      }
      else if (iTotalSpamScore >= Configuration::Instance()->GetAntiSpamConfiguration().GetSpamMarkThreshold())
      {
         shared_ptr<MessageData> messageData = SpamProtection::TagMessageAsSpam(m_pCurrentMessage, setSpamTestResults);
         if (messageData)
            messageData->Write(fileName);
      }

      // Run PostTransmissionTests. These consists of more heavy stuff such as SURBL and SpamAssassin-
      setResult = 
            SpamProtection::Instance()->RunPostTransmissionTests(senderAddress, ipAddress, ipAddress, m_pCurrentMessage);

      setSpamTestResults.insert(setResult.begin(), setResult.end());

      iTotalSpamScore = SpamProtection::CalculateTotalSpamScore(setSpamTestResults);

      if (iTotalSpamScore >= Configuration::Instance()->GetAntiSpamConfiguration().GetSpamDeleteThreshold())
      {
         FileUtilities::DeleteFile(fileName);
         return false;
      }
      else if (iTotalSpamScore >= Configuration::Instance()->GetAntiSpamConfiguration().GetSpamMarkThreshold())
      {
         shared_ptr<MessageData> messageData = SpamProtection::TagMessageAsSpam(m_pCurrentMessage, setSpamTestResults);

         if (messageData)
            messageData->Write(fileName);
      }

      return true;
   }

   void 
   POP3ClientConnection::_ParseMessageHeaders()
   {
      assert(m_pCurrentMessage);

      String fileName = PersistentMessage::GetFileName(m_pCurrentMessage);

      AnsiString sHeader = PersistentMessage::LoadHeader(fileName);
      shared_ptr<MimeHeader> pHeader = shared_ptr<MimeHeader>(new MimeHeader);
      pHeader->Load(sHeader, sHeader.GetLength());

      {
         // Parse out the sender of the message. 
         String sFrom = pHeader->GetRawFieldValue("From");

         if (!sFrom.IsEmpty())
         {
            AddresslistParser oAddressParser;

            String sFullName, sUser, sDomain;
            oAddressParser.ExtractParts(sFrom, sFullName, sUser, sDomain);
            sFrom = sUser + "@" + sDomain;
            m_pCurrentMessage->SetFromAddress(sFrom);
         }
      }      

      // bool bPreprocessRecipientList = true;
      _CreateRecipentList(pHeader);

      // Remove non-existent accounts.
      _RemoveInvalidRecipients();

      _RetrieveReceivedDate(pHeader);
   }

   void
   POP3ClientConnection::_SaveMessage()
   {
      if (m_pCurrentMessage->GetRecipients()->GetCount() > 0)
      {
         m_pCurrentMessage->SetState(Message::Delivering);

         PersistentMessage::SaveObject(m_pCurrentMessage);
      }


   }

   void 
   POP3ClientConnection::_MarkCurrentMessageAsRead()
   {
      if (m_iterCurMessage != m_mapUIDLResponse.end())
      {
         String sUID = (*m_iterCurMessage).second;

         // If we're deleting this message immediately, there's
         // no point in adding it to the table.
         if (_GetDaysToKeep(sUID) != -1)
         {
            // Make sure that the UID exists in the database.
            // If it already exists, AddUID() will do nothing.
            _GetUIDList()->AddUID(sUID); 
         }

      }
   }

   bool
   POP3ClientConnection::_MessageCleanup()
   {
      int iIndex = (*m_iterCurMessage).first;
      String sUID = (*m_iterCurMessage).second;

      int iDaysToKeep = _GetDaysToKeep(sUID);

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
         shared_ptr<FetchAccountUID> pUID = _GetUIDList()->GetUID(sUID);

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
      _SendData(sResponse);

      m_eCurrentState = StateDELESent;

      // Delete this UID from the database.
      _GetUIDList()->DeleteUID(sUID);
     

      return true;
   }

   void 
   POP3ClientConnection::_CreateRecipentList(shared_ptr<MimeHeader> pHeader)
   {
      if (m_pAccount->GetProcessMIMERecipients())
      {  
         _ProcessMIMERecipients(pHeader);
      }  

      if (m_pCurrentMessage->GetRecipients()->GetCount() > 0)
         return;
      
      // Just fetch the account
      if (m_sReceivingAccountAddress.IsEmpty())
      {
         shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(m_pAccount->GetAccountID());
         if (pAccount)
         {
            m_sReceivingAccountAddress = pAccount->GetAddress();
         }
      }

      // Add the recipient to the message
      bool recipientOK = false;
      RecipientParser recipientParser;
      recipientParser.CreateMessageRecipientList(m_sReceivingAccountAddress, m_pCurrentMessage->GetRecipients(), recipientOK);
   }

   void
   POP3ClientConnection::_ProcessMIMERecipients(shared_ptr<MimeHeader> pHeader)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Goes through headers in the email and locates recipient. Adds these recipients
   // to the message
   //---------------------------------------------------------------------------()
   {
      String sTo = pHeader->GetRawFieldValue("To");
      String sCC = pHeader->GetRawFieldValue("CC");
      String sXRCPTTo = pHeader->GetRawFieldValue("X-RCPT-TO");
      String sXEnvelopeTo = pHeader->GetRawFieldValue("X-Envelope-To");

      String sAllRecipients = sTo + "," + sCC + "," + sXRCPTTo + "," + sXEnvelopeTo;

      // Parse this list.
      AddresslistParser oListParser;
      std::vector<shared_ptr<Address> > vecAddresses = oListParser.ParseList(sAllRecipients);
      std::vector<shared_ptr<Address> >::iterator iterAddress = vecAddresses.begin();

      RecipientParser recipientParser;
      while (iterAddress != vecAddresses.end())
      {
         shared_ptr<Address> pAddress = (*iterAddress);

         if (pAddress->sMailboxName.IsEmpty() || pAddress->sDomainName.IsEmpty())
         {
            iterAddress++;
            continue;
         }

         String sAddress = pAddress->sMailboxName + "@" + pAddress->sDomainName;

         // Add the recipient to the message
         bool recipientOK = false;
         recipientParser.CreateMessageRecipientList(sAddress, m_pCurrentMessage->GetRecipients(), recipientOK);

         iterAddress++;
      }

      // Go through the Received headers
      _ProcessReceivedHeaders(pHeader);

      // Remove non-existent accounts.
      _RemoveInvalidRecipients();
}

   void 
   POP3ClientConnection::_RetrieveReceivedDate(shared_ptr<MimeHeader> pHeader)
   {
      if (!m_pAccount->GetProcessMIMEDate())
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

      

      m_pCurrentMessage->SetCreateTime(sDate);
   }

   void 
   POP3ClientConnection::_ProcessReceivedHeaders(shared_ptr<MimeHeader> pHeader)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Goes throguh all the "Received" headers of the email. Tries to parse the
   // addresses and deliver the message to the recipients specified in them.
   //---------------------------------------------------------------------------()
   {
      AnsiString sHeaderName = "Received";
      vector<MimeField> &lstFields = pHeader->Fields();
      vector<MimeField>::iterator iter = lstFields.begin();
      vector<MimeField>::iterator iterEnd = lstFields.end();

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
               recipientParser.CreateMessageRecipientList(sRecipient, m_pCurrentMessage->GetRecipients(), recipientOK);
            }

         }
      }
   }

   void
   POP3ClientConnection::_LogPOP3String(const String &sLogString, bool bSent) const
   {
      String sTemp;

      if (bSent)
      {
         // Check if we should remove the password.
         if (m_eCurrentState == StatePasswordSent)
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
      sTemp.Replace(_T("\r\n"), _T("[nl]"));

      LOG_POP3_CLIENT(GetSessionID(), GetIPAddressString(), sTemp);
   }
   
   void 
   POP3ClientConnection::OnCouldNotConnect(const AnsiString &sErrorDescription)
   {
      LOG_DEBUG("Connection to remote POP3-server failed. Error message: " + String(sErrorDescription) );
   }

   void 
   POP3ClientConnection::_RemoveInvalidRecipients()
   {
      if (m_pAccount->GetEnableRouteRecipients())
         m_pCurrentMessage->GetRecipients()->RemoveExternal();
      else
         m_pCurrentMessage->GetRecipients()->RemoveNonLocalAccounts();
   }

   int
   POP3ClientConnection::_GetDaysToKeep(const String &sUID)
   {
      int iDaysToKeep = m_pAccount->GetDaysToKeep();

      // Has an event overriden when messages should be deleted?
      if (_eventResults.find(sUID) != _eventResults.end())
      {
         shared_ptr<Result> result = _eventResults[sUID];

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
   POP3ClientConnection::_FireOnExternalAccountDownload(shared_ptr<Message> message, const String &uid)
   {
      shared_ptr<Result> pResult = Events::FireOnExternalAccountDownload(m_pAccount, message, uid);

      if (pResult)
         _eventResults[uid] = pResult;
   }

   shared_ptr<FetchAccountUIDList>
   POP3ClientConnection::_GetUIDList()
   {
      if (!_fetchAccountUIDList)
      {
         _fetchAccountUIDList = shared_ptr<FetchAccountUIDList>(new FetchAccountUIDList());
         _fetchAccountUIDList->Refresh(m_pAccount->GetID());
      }

      return _fetchAccountUIDList;
   }
   // This is temp function to log ETRN client commands to SMTP
   void
   POP3ClientConnection::_LogSMTPString(const String &sLogString, bool bSent) const
   {
      String sTemp;

      if (bSent)
      {
         // Check if we should remove the password.
         if (m_eCurrentState == StatePasswordSent)
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
      sTemp.Replace(_T("\r\n"), _T("[nl]"));

      LOG_SMTP_CLIENT(GetSessionID(), GetIPAddressString(), sTemp);
   }

   // This is temp function to log ETRN client commands to SMTP
   void
   POP3ClientConnection::_SendData2(const String &sData) 
   {
      _LogSMTPString(sData, true);

      SendData(sData + "\r\n");
   }




}
