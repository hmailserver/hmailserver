// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Common/Threading/Task.h"

namespace HM
{
   class Message;
   class ClientInfo;
   
   class SMTPDeliveryManager : public Task
   {
   public:
	   SMTPDeliveryManager();
	   virtual ~SMTPDeliveryManager();

      void SetDeliverMessage();
      void DoWork();
   
      const String &GetQueueName() const;

      void UncachePendingMessages();

      void OnPropertyChanged(shared_ptr<Property> pProperty);

   private:
      
      void _SendStatistics(bool bIgnoreMessageCount = false);

      void _LoadPendingMessageList();
      shared_ptr<Message> _GetNextMessage();

      long m_lCurNumberOfSent;

      shared_ptr<DALRecordset> m_pPendingMessages;
      
      const String m_sQueueName;

      bool m_bUncachePendingMessages;

      int m_iQueueID;

      Event deliver_messages_;
   };

}
