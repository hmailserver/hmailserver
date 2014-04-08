// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ServerStatus : public Singleton<ServerStatus>
   {
   public:
	   ServerStatus();
	   virtual ~ServerStatus();

      enum ServerState
      {
         StateUnknown = 0,
         StateStopped = 1,
         StateStarting = 2,
         StateRunning = 3,
         StateStopping = 4
      };

      String GetUnsortedMessageStatus()  const;

      void OnMessageProcessed();
      int GetNumberOfProcessedMessages()  const;

      void OnSpamMessageDetected();
      int GetNumberOfDetectedSpamMessages() const;
      
      void OnVirusRemoved();
      int GetNumberOfRemovedViruses() const;

      void SetState(ServerState i);
      int GetState() const;

      int GetNumberOfSessions(int iSessionType);

   private:

      int m_iProcessedMessages;
      int m_iNumberOfSpamMessagesDetected;
      int m_iNumberOfVirusesRemoved;

      CriticalSection m_oCSSpamMessgeDropped;
      CriticalSection m_oCSVirusesRemoved;

      ServerState m_eState;
   };
}
