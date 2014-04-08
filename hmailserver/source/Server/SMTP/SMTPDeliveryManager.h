// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Common/Threading/Task.h"
#include "../Common/Util/Event.h"

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
      void StopWork();
   
      const String &GetQueueName() const;

      void UncachePendingMessages();

      void OnPropertyChanged(shared_ptr<Property> pProperty);

   private:
      
      void _SendStatistics(bool bIgnoreMessageCount = false);

      void _LoadPendingMessageList();
      shared_ptr<Message> _GetNextMessage();

      void _StartTimer();

      HANDLE m_evtTimer;

      Event m_evtDeliverMessage;

      long m_lCurNumberOfSent;

      shared_ptr<DALRecordset> m_pPendingMessages;

      Event m_hStopRequest;
      
      const String m_sQueueName;

      bool m_bUncachePendingMessages;

      int m_iQueueID;
   };

}
