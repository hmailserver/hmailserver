// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class Result;
   class FetchAccount;

   class Events
   {
   public:
      Events(void);
      ~Events(void);

      static bool FireOnDeliverMessage(std::shared_ptr<Message> pMessage);
      static bool FireOnDeliveryStart(std::shared_ptr<Message> pMessage);
      static void FireOnDeliveryFailed(std::shared_ptr<Message> pMessage, const String &sSendersIP, const String &sRecipient, const String &sErrorMessage);
         
      static std::shared_ptr<Result> FireOnExternalAccountDownload(std::shared_ptr<FetchAccount> fetchAccount, std::shared_ptr<Message> pMessage, const String &sRemoteUID);
   };

}