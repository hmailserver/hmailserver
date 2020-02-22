// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;

   class AWStats
   {
   public:
      AWStats(void);
      ~AWStats(void);

      static void LogDeliveryFailure(const String &senderIP, const String &sFromAddress, const String &sToAddress, int iErrorCode);
      static void LogDeliverySuccess(const String &senderIP, const String &recipientIP, std::shared_ptr<Message> pMessage, const String &sRecipient);

      static void SetEnabled(bool bNewVal);
      static bool GetEnabled();

   private:

      static void Log_(const String &senderIP, const String &recipientIP, const String &senderAddress, const String &recipientAddress, int iErrorCode, int iBytesReceived);

      static bool enabled_;
   };
}