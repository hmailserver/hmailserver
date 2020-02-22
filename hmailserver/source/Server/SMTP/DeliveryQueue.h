// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-08-02

#pragma once

#include "..\Common\Threading\Task.h"

namespace HM
{
   class DeliveryQueueClearer : public Task
   {
   public:
      DeliveryQueueClearer(void);
      ~DeliveryQueueClearer(void);

      virtual void DoWork();

   private:

   };

   class DeliveryQueue
   {
   public:
      DeliveryQueue(void);
      ~DeliveryQueue(void);

      static void Clear();
      static void ResetDeliveryTime(__int64 iMessageID);
      static void Remove(__int64 iMessageID);
      static void StartDelivery();

      static void OnDeliveryQueueCleared();
   private:

      static bool is_clearing_;
   };
}