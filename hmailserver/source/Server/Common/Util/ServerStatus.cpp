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
      processed_messages_ = 0;
      number_of_spam_messages_detected_ = 0;
      number_of_viruses_removed_ = 0;
      state_ = StateUnknown ;

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

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
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

         std::shared_ptr<DALRecordset> pRecipientsRS = Application::Instance()->GetDBManager()->OpenRecordset(selectCommand);
         if (!pRecipientsRS)
            return "";

         while (!pRecipientsRS->IsEOF())
         {
            if (!sTo.IsEmpty())
               sTo += ",";

            sTo += pRecipientsRS->GetStringValue("recipientaddress");

            pRecipientsRS->MoveNext();
         }

         sLine.Format(_T("%I64d\t%s\t%s\t%s\t%s\t%s\t%d\t%d"), lMessageID, sCreateTime.c_str(), sFrom.c_str(), sTo.c_str(), sNextTryTime.c_str(), sFileName.c_str(), bLocked, lNoOfTries);
         
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
      return processed_messages_;
   }

   void
   ServerStatus::OnMessageProcessed()
   {
      // Called by SMTPDeliveryManger which is
      // single threaded, so no synchronization
      // is needed.

      processed_messages_++;
   }

   int 
   ServerStatus::GetNumberOfDetectedSpamMessages() const
   {
      return number_of_spam_messages_detected_;
   }

   void
   ServerStatus::OnSpamMessageDetected()
   {
      // This requires thread synchronization since
      // it's called by the SMTPConnection.

      boost::lock_guard<boost::recursive_mutex> guard(spam_message_dropped_mutex_);
      number_of_spam_messages_detected_++;
   }

   int 
   ServerStatus::GetNumberOfRemovedViruses() const
   {
      return number_of_viruses_removed_;
   }

   void
   ServerStatus::OnVirusRemoved()
   {
      // This requires thread synchronization since
      // it's called by the SMTPConnection.
      boost::lock_guard<boost::recursive_mutex> guard(virus_removed_mutex_);

      number_of_viruses_removed_++;
   }
   
   int
   ServerStatus::GetState() const
   {
      return state_;
   }

   void
   ServerStatus::SetState(ServerState i)
   {
      state_ = i; 
   }

   int 
   ServerStatus::GetNumberOfSessions(int iSessionType)
   {
      return SessionManager::Instance()->GetNumberOfConnections((SessionType) iSessionType);
   }

   int
   ServerStatus::GetThreadID() const
   {
      DWORD dwThreadID = GetCurrentThreadId();
      return dwThreadID;
   }
}
