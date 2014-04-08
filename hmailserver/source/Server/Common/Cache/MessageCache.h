// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

#include "..\Util\Singleton.h"

namespace HM
{
   class Message;

   class MessageCache : public Singleton<MessageCache>
   {
   public:
      MessageCache(void);
      ~MessageCache(void);

      void AddMessage(shared_ptr<Message> pMessage);
      shared_ptr<Message> GetMessage(__int64 iMessageID);


   private:

      CriticalSection m_csMessage;
      std::map<__int64, shared_ptr<Message> > m_mapMessage;

   };
}