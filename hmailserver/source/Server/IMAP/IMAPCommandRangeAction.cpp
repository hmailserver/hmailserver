// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandRangeAction.h"
#include "IMAPConnection.h"
#include "IMAPFolderView.h"
#include "../Common/BO/Messages.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/IMAPFolder.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCommandRangeAction::IMAPCommandRangeAction() :
      is_uid_(false)
   {

   }

   IMAPCommandRangeAction::~IMAPCommandRangeAction()
   {

   }

   void
   IMAPCommandRangeAction::SetIsUID(bool bIsUID)
   {
      is_uid_ = bIsUID;
   }

   bool
   IMAPCommandRangeAction::GetIsUID()
   {
      return is_uid_;
   }

   IMAPResult
   IMAPCommandRangeAction::DoForMails(std::shared_ptr<IMAPConnection> pConnection, const String &sMailNos, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      auto view = pConnection->GetCurrentFolderView();

      if (!view)
         return IMAPResult(IMAPResult::ResultNo, "No folder selected.");

      // The message numbers are resolved against this session's view, so they mean the same
      // messages they meant when the client was told about them, regardless of what other
      // sessions have done to the folder since.
      auto targets = ResolveTargets_(view, sMailNos);

      if (targets.empty())
         return IMAPResult();

      std::set<__int64> message_ids;
      for (const auto &target : targets)
         message_ids.insert(target.second.message_id);

      auto messages = pConnection->GetCurrentFolder()->GetMessages();

      std::map<__int64, std::shared_ptr<Message>> resolved_messages;

      if (UsesLiveMessages())
      {
         // The caller updates the message, so it needs the objects the collection holds.
         resolved_messages = messages->GetItemsByIds(message_ids);
      }
      else
      {
         resolved_messages = messages->GetCopyByIds(message_ids);
      }

      /*
         A message which is in this session's view but no longer in the folder has been
         expunged by another session. The client hasn't been told yet - it's told the next
         time we're allowed to send an EXPUNGE - so how the command reacts depends on how it's
         addressed. The UID variants ignore it (RFC 3501 6.4.8), the others report it.
      */
      MissingMessagePolicy policy = is_uid_ ? MissingMessagePolicy::Ignore : GetMissingMessagePolicy();

      bool any_missing = false;

      for (const auto &target : targets)
      {
         if (resolved_messages.find(target.second.message_id) != resolved_messages.end())
            continue;

         view->MarkVanished(target.second.message_id);
         any_missing = true;
      }

      if (any_missing && policy == MissingMessagePolicy::FailBeforeActing)
         return IMAPResult(IMAPResult::ResultNo, "[EXPUNGEISSUED] Some of the messages no longer exist.");

      for (const auto &target : targets)
      {
         auto iter = resolved_messages.find(target.second.message_id);

         if (iter == resolved_messages.end())
            continue;

         IMAPResult result = DoAction(pConnection, target.first, (*iter).second, pArgument);

         if (result.GetResult() != IMAPResult::ResultOK)
            return result;
      }

      if (any_missing && policy == MissingMessagePolicy::ReportAfterActing)
         return IMAPResult(IMAPResult::ResultNo, "[EXPUNGEISSUED] Some of the messages no longer exist.");

      return IMAPResult();
   }

   std::vector<std::pair<int, IMAPViewEntry>>
   IMAPCommandRangeAction::ResolveTargets_(std::shared_ptr<IMAPFolderView> view, const String &sMailNos)
   {
      std::vector<std::pair<int, IMAPViewEntry>> targets;

      std::vector<String> sSplitted = StringParser::SplitString(sMailNos, ",");

      for (String sCur : sSplitted)
      {
         long lColonPos = sCur.Find(_T(":"));

         String sFirstPart = lColonPos >= 0 ? sCur.Mid(0, lColonPos) : sCur;
         String sSecondPart = lColonPos >= 0 ? sCur.Mid(lColonPos + 1) : sCur;

         bool endIsWildcard = sSecondPart == _T("*");

         if (is_uid_)
         {
            unsigned int startUID = _ttoi(sFirstPart);
            unsigned int endUID = endIsWildcard ? UINT_MAX : _ttoi(sSecondPart);

            if (lColonPos >= 0)
            {
               for (const auto &entry : view->GetEntriesByUIDRange(startUID, endUID))
                  targets.push_back(entry);
            }
            else
            {
               int sequence = 0;
               IMAPViewEntry entry;

               if (view->GetEntryByUID(startUID, sequence, entry))
                  targets.push_back(std::make_pair(sequence, entry));
            }
         }
         else
         {
            int startIndex = _ttoi(sFirstPart);
            int endIndex = endIsWildcard ? -1 : _ttoi(sSecondPart);

            if (lColonPos >= 0)
            {
               for (const auto &entry : view->GetEntriesBySequenceRange(startIndex, endIndex))
                  targets.push_back(entry);
            }
            else
            {
               IMAPViewEntry entry;

               if (view->GetEntryBySequence(startIndex, entry))
                  targets.push_back(std::make_pair(startIndex, entry));
            }
         }
      }

      return targets;
   }
}
