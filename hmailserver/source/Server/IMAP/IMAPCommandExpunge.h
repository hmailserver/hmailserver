// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"

namespace HM
{
   class IMAPCommandEXPUNGE : public IMAPCommand
   {
   public:
      // Expunges the \Deleted messages in the selected folder.
      IMAPCommandEXPUNGE();

      // Expunges only the \Deleted messages in the UID set (UID EXPUNGE, RFC 4315).
      explicit IMAPCommandEXPUNGE(const String &uid_set);

      virtual IMAPResult ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);

   private:

      const bool is_uid_;
      const String uid_set_;
   };


}

