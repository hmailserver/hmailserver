// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class MessageRecipients;
   class MessageRecipient;
   class DistributionList;

   class RecipientParser  
   {
   public:
	   RecipientParser();
	   virtual ~RecipientParser();

      enum DeliveryPossibility
      {
         DP_Possible = 0,
         DP_RecipientUnknown = 1,
         DP_PermissionDenied = 2,
      };

      DeliveryPossibility CheckDeliveryPossibility(bool bSenderIsAuthed, String sSender, const String &sOriginalRecipient, String &sErrMsg, bool &bTreatSecurityAsLocal, int iRecursionLevel);
      void CreateMessageRecipientList(const String &sRecipientAddress, std::shared_ptr<MessageRecipients> pRecipients, bool &recipientOK);
      
   private:

      void CreateMessageRecipientList_(const String &recipientAddress, const String &sOriginalAddress, long lRecurse, std::shared_ptr<MessageRecipients> pRecipients, bool &recipientOK);
      void AddRecipient_(std::shared_ptr<MessageRecipients> pRecipients, std::shared_ptr<MessageRecipient> pRecipient);
      DeliveryPossibility UserCanSendToList_(const String &sSender, bool bSenderIsAuthenticated, std::shared_ptr<const DistributionList> pList, String &sErrMsg, int iRecursionLevel);

  };
}