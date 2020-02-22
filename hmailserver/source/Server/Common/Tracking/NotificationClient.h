// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ChangeNotification;

   class NotificationClient
   {
   public:
      NotificationClient();

      virtual void OnNotification(std::shared_ptr<ChangeNotification> notification) = 0;
   };
}
