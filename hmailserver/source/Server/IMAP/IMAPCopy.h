// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "IMAPCommandRangeAction.h"

namespace HM
{
   class IMAPFolder;

   class IMAPCopy  : public IMAPCommandRangeAction
   {
   public:
	   IMAPCopy();

      virtual IMAPResult DoAction(std::shared_ptr<IMAPConnection> pConnection, int messageIndex, std::shared_ptr<Message> pOldMessage, const std::shared_ptr<IMAPCommandArgument> pArgument);

      // COPY is atomic (RFC 3501 6.4.7), so nothing is copied unless every message exists.
      virtual MissingMessagePolicy GetMissingMessagePolicy() const { return MissingMessagePolicy::FailBeforeActing; }

      // The COPYUID response code (RFC 4315) followed by a space, or empty if nothing was copied
      // or the destination can't be read.
      virtual String GetResponseCode() const;

      // EXISTS and RECENT if messages were copied into the selected folder, since the delayed
      // notification is not sent to this session.
      virtual String GetUntaggedResponse(std::shared_ptr<IMAPConnection> pConnection);

   protected:

      // Removes the copies already made, so a failed COPY leaves the destination unchanged.
      virtual void RollBack(std::shared_ptr<IMAPConnection> pConnection);

   private:

      static String FormatUIDSet_(const std::vector<unsigned int> &uids);

      std::shared_ptr<IMAPFolder> destination_folder_;
      bool destination_readable_ = false;

      // In the order the messages were copied. Element n of each describes the same message.
      std::vector<unsigned int> source_uids_;
      std::vector<unsigned int> destination_uids_;

      std::set<__int64> copied_message_ids_;
   };
}
