// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\awstats.h"
#include "Time.h"
#include "../BO/Message.h"
#include "../BO/MessageRecipients.h"
#include "../BO/MessageRecipient.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool AWStats::m_bEnabled = false;

   AWStats::AWStats(void)
   {
   }

   AWStats::~AWStats(void)
   {
   }

   void 
   AWStats::LogDeliveryFailure(const String &senderIP, const String &sFromAddress, const String &sToAddress, int iErrorCode)
   {
      if (!m_bEnabled)
         return;

      LOG_DEBUG(_T("AWStats::LogDeliveryFailure"));

      // Since we were unable to deliver the message, we log that the recipient IP address was 127.0.0.1
      // Not really clear what the 'correct' thing to log here is.
      _Log(senderIP, "127.0.0.1", sFromAddress, sToAddress, iErrorCode, 0);
   }


   void
   AWStats::LogDeliverySuccess(const String &senderIP, const String &recipientIP, shared_ptr<Message> pMessage, const String &sRecipient)
   {
      if (!m_bEnabled)
         return;

      LOG_DEBUG(_T("AWStats::LogDeliverySuccess"));

      _Log(senderIP, recipientIP, pMessage->GetFromAddress(), sRecipient, 250, pMessage->GetSize());
   }

   void 
   AWStats::_Log(const String &senderIP, const String &recipientIP, const String &senderAddress, const String &recipientAddress, int iErrorCode, int iBytesReceived)
   {
      if (!m_bEnabled)
         return;

      // Following format is used:
      // %time2 %email %email_r %host %host_r %method %url %code %bytesd"
      
      String sTime = Time::GetCurrentDateTime();

      String sModifiedSender = senderAddress;
      sModifiedSender.Replace(_T("<"), _T(""));
      sModifiedSender.Replace(_T(">"), _T(""));
      sModifiedSender.Replace(_T(" "), _T(""));
      sModifiedSender.Replace(_T("\t"), _T(""));

      String sModifiedRecipient = recipientAddress;
      sModifiedRecipient.Replace(_T("<"), _T(""));
      sModifiedRecipient.Replace(_T(">"), _T(""));
      sModifiedRecipient.Replace(_T(" "), _T(""));
      sModifiedRecipient.Replace(_T("\t"), _T(""));

      String sLogLine;
      sLogLine.Format(_T("%s\t%s\t%s\t%s\t%s\tSMTP\t?\t%d\t%d\r\n"), 
                        sTime, sModifiedSender, sModifiedRecipient, senderIP, recipientIP, iErrorCode, iBytesReceived );

      Logger::Instance()->LogAWStats(sLogLine);
   }

   void 
   AWStats::SetEnabled(bool bNewVal)
   {
      m_bEnabled = bNewVal;
   }

   bool 
   AWStats::GetEnabled()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns true if the awstats log is enabled. false otherwise.
   //---------------------------------------------------------------------------()
   {
      return m_bEnabled;
   }
}