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

      static bool FireOnDeliverMessage(shared_ptr<Message> pMessage);
      static bool FireOnDeliveryStart(shared_ptr<Message> pMessage);
      static void FireOnDeliveryFailed(shared_ptr<Message> pMessage, const String &sSendersIP, const String &sRecipient, const String &sErrorMessage);
         
      static shared_ptr<Result> FireOnExternalAccountDownload(shared_ptr<FetchAccount> fetchAccount, shared_ptr<Message> pMessage, const String &sRemoteUID);
   };

}