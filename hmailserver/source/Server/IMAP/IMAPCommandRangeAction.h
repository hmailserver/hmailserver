// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"
#include "IMAPFolderView.h"

namespace HM
{
   class IMAPFolderView;

   // What to do when a message set names a message which another session has expunged.
   enum class MissingMessagePolicy
   {
      // Skip it. Required for the UID variants (RFC 3501 6.4.8).
      Ignore,
      // Act on the messages which do exist, then fail the command (RFC 2180 4.1.3).
      ReportAfterActing,
      // Fail without acting on any of the messages.
      FailBeforeActing,
   };

   class IMAPCommandRangeAction : public IMAPCommand  
   {
   public:
	   IMAPCommandRangeAction();
	   virtual ~IMAPCommandRangeAction();

      void SetIsUID(bool bIsUID);
      
      IMAPResult ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument) {return IMAPResult();}
      IMAPResult DoForMails(std::shared_ptr<IMAPConnection> pConnection, const String &sMailNos, const std::shared_ptr<IMAPCommandArgument> pArgument);

      // Translates a message set into this session's messages, as (sequence number, entry).
      // Returns false if the set is malformed.
      static bool ResolveTargets(std::shared_ptr<IMAPFolderView> view, const String &sMailNos, bool isUID, std::vector<std::pair<int, IMAPViewEntry>> &targets);

      // A response code for the tagged OK, followed by a space, or empty if there is none.
      virtual String GetResponseCode() const { return String(); }

      // Untagged responses to send before the tagged OK, or empty if there are none.
      virtual String GetUntaggedResponse(std::shared_ptr<IMAPConnection> pConnection) { return String(); }

   protected:

      bool GetIsUID();

      // Called once the message set is known to be valid, before any message is acted on.
      virtual IMAPResult Prepare(std::shared_ptr<IMAPConnection> pConnection, const std::shared_ptr<IMAPCommandArgument> pArgument) { return IMAPResult(); }

      virtual IMAPResult DoAction(std::shared_ptr<IMAPConnection> pConnection, int messageIndex, std::shared_ptr<Message> pMessage, const std::shared_ptr<IMAPCommandArgument> pArgument) = 0;

      // Called when DoAction fails. Override to undo the actions already taken.
      virtual void RollBack(std::shared_ptr<IMAPConnection> pConnection) {}

      // Override and return true if DoAction updates the message. Such commands are given the
      // objects the collection holds rather than copies of them.
      virtual bool UsesLiveMessages() const { return false; }

      virtual MissingMessagePolicy GetMissingMessagePolicy() const { return MissingMessagePolicy::Ignore; }

   private:

      // Parses a number or *, which is given as highest.
      static bool ParseNumber_(const String &value, unsigned int highest, unsigned int &number);

      bool is_uid_;
     
   };

}
