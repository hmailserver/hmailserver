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
      std::vector<std::pair<int, IMAPViewEntry>> targets;

      if (!ResolveTargets(view, sMailNos, is_uid_, targets))
         return IMAPResult(IMAPResult::ResultBad, "Incorrect message set.");

      IMAPResult prepare_result = Prepare(pConnection, pArgument);
      if (prepare_result.GetResult() != IMAPResult::ResultOK)
         return prepare_result;

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
         {
            RollBack(pConnection);
            return result;
         }
      }

      if (any_missing && policy == MissingMessagePolicy::ReportAfterActing)
         return IMAPResult(IMAPResult::ResultNo, "[EXPUNGEISSUED] Some of the messages no longer exist.");

      return IMAPResult();
   }

   bool
   IMAPCommandRangeAction::ParseNumber_(const String &value, unsigned int highest, unsigned int &number)
   {
      if (value == _T("*"))
      {
         number = highest;
         return true;
      }

      if (value.IsEmpty() || value.GetLength() > 10 || !StringParser::ValidateString(value, "0123456789"))
         return false;

      __int64 parsed = _ttoi64(value);

      // Sequence numbers and UIDs are nz-number (RFC 3501 9).
      if (parsed == 0 || parsed > UINT_MAX)
         return false;

      number = (unsigned int) parsed;
      return true;
   }

   bool
   IMAPCommandRangeAction::ResolveTargets(std::shared_ptr<IMAPFolderView> view, const String &sMailNos, bool isUID, std::vector<std::pair<int, IMAPViewEntry>> &targets)
   {
      targets.clear();

      // * is the highest number in use (RFC 3501 9).
      unsigned int highest = isUID ? view->GetHighestUID() : (unsigned int) view->GetMessageCount();

      // Parse the whole set first, so that a malformed set resolves to nothing.
      std::vector<std::pair<unsigned int, unsigned int>> ranges;

      int start = 0;

      while (true)
      {
         int comma = sMailNos.Find(_T(","), start);
         String part = comma >= 0 ? sMailNos.Mid(start, comma - start) : sMailNos.Mid(start);

         int colon = part.Find(_T(":"));
         String first_part = colon >= 0 ? part.Mid(0, colon) : part;
         String last_part = colon >= 0 ? part.Mid(colon + 1) : part;

         unsigned int first = 0;
         unsigned int last = 0;

         if (!ParseNumber_(first_part, highest, first) || !ParseNumber_(last_part, highest, last))
            return false;

         // A range may be given in either order.
         if (first > last)
            std::swap(first, last);

         ranges.push_back(std::make_pair(first, last));

         if (comma < 0)
            break;

         start = comma + 1;
      }

      // A message named more than once is acted on once, where it first appears.
      std::set<__int64> seen_message_ids;

      for (const auto &range : ranges)
      {
         auto entries = isUID ?
            view->GetEntriesByUIDRange(range.first, range.second) :
            view->GetEntriesBySequenceRange((int) std::min<unsigned int>(range.first, INT_MAX), (int) std::min<unsigned int>(range.second, INT_MAX));

         for (const auto &entry : entries)
         {
            if (seen_message_ids.insert(entry.second.message_id).second)
               targets.push_back(entry);
         }
      }

      return true;
   }
}
