// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;

   class DKIMSigner
   {
   public:
      DKIMSigner();

      void Sign(std::shared_ptr<Message> message);
   };

}