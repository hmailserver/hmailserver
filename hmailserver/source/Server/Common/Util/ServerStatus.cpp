// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "ServerStatus.h"

#include "../Application/SessionManager.h"
#include "../Persistence/PersistentMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ServerStatus::ServerStatus()
   {
      m_iProcessedMessages = 0;
      m_iNumberOfSpamMessagesDetected = 0;
      m_iNumberOfVirusesRemoved = 0;
      m_eState = StateUnknown ;

   }

   ServerStatus::~ServerStatus()
   {

   }

   String 
   ServerStatus::GetUnsortedMessageStatus() const
   {
      // messagetype 3 added for ETRN on GUI Delivery Queue
      SQLCommand command("select messageid, messagecurnooftries, messagecreatetime, messagefrom, messagenexttrytime, messagefilename, messagelocked from hm_messages "
                         " where messagetype = 1 OR messagetype = 3 order by messageid asc");

      shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return "";

      String sRetVal;
      String sCreateTime, sFrom, sTo, sNextTryTime, sFileName, sLine;
      __int64 lMessageID;
      int lNoOfTries;
      bool bLocked;
      String dataDirectory = IniFileSettings::Instance()->GetDataDirectory();

      while (!pRS->IsEOF())
      {
         
         lMessageID = pRS->GetInt64Value("messageid");
         lNoOfTries = pRS->GetLongValue("messagecurnooftries");
         sCreateTime = pRS->GetStringValue("messagecreatetime");
         sFrom = pRS->GetStringValue("messagefrom");
         sNextTryTime = pRS->GetStringValue("messagenexttrytime");
         sFileName = pRS->GetStringValue("messagefilename");
         bLocked = pRS->GetLongValue("messagelocked") == 1;
         sTo = ""; // reset between every recipient
         
         // Construct a full path to the file if it's partial.
         if (PersistentMessage::IsPartialPath(sFileName))
            sFileName = FileUtilities::Combine(dataDirectory, sFileName);

         SQLCommand selectCommand("select recipientaddress from hm_messagerecipients where recipientmessageid = @MESSAGEID");
         selectCommand.AddParameter("@MESSAGEID", lMessageID);

         shared_ptr<DALRecordset> pRecipientsRS = Application::Instance()->GetDBManager()->OpenRecordset(selectCommand);
         if (!pRecipientsRS)
            return "";

         while (!pRecipientsRS->IsEOF())
         {
            if (!sTo.IsEmpty())
               sTo += ",";

            sTo += pRecipientsRS->GetStringValue("recipientaddress");

            pRecipientsRS->MoveNext();
         }

         sLine.Format(_T("%I64d\t%s\t%s\t%s\t%s\t%s\t%d\t%d"), lMessageID, sCreateTime, sFrom, sTo, sNextTryTime, sFileName, bLocked, lNoOfTries);
         
         if (!sRetVal.IsEmpty())
            sRetVal += "\r\n";
         
         sRetVal += sLine;

         pRS->MoveNext();
      }

      return sRetVal;
   }

   int 
   ServerStatus::GetNumberOfProcessedMessages() const
   {
      return m_iProcessedMessages;
   }

   void
   ServerStatus::OnMessageProcessed()
   {
      // Called by SMTPDeliveryManger which is
      // single threaded, so no synchronization
      // is needed.

      m_iProcessedMessages++;
   }

   int 
   ServerStatus::GetNumberOfDetectedSpamMessages() const
   {
      return m_iNumberOfSpamMessagesDetected;
   }

   void
   ServerStatus::OnSpamMessageDetected()
   {
      // This requires thread synchronization since
      // it's called by the SMTPConnection.

      CriticalSectionScope scope(m_oCSSpamMessgeDropped);
      m_iNumberOfSpamMessagesDetected++;
   }

   int 
   ServerStatus::GetNumberOfRemovedViruses() const
   {
      return m_iNumberOfVirusesRemoved;
   }

   void
   ServerStatus::OnVirusRemoved()
   {

      // This requires thread synchronization since
      // it's called by the SMTPConnection.
      CriticalSectionScope scope(m_oCSVirusesRemoved);

      m_iNumberOfVirusesRemoved++;
   }
   
   int
   ServerStatus::GetState() const
   {
      return m_eState;
   }

   void
   ServerStatus::SetState(ServerState i)
   {
      m_eState = i; 
   }

   int 
   ServerStatus::GetNumberOfSessions(int iSessionType)
   {
      return SessionManager::Instance()->GetNumberOfConnections((SessionType) iSessionType);
   }
}
