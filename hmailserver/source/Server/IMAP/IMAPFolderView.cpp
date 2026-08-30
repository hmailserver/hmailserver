// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPFolderView.h"

#include "../Common/BO/Message.h"
#include "../Common/BO/Messages.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPFolderView::IMAPFolderView(__int64 account_id, __int64 folder_id) :
      highest_uid_seen_(0),
      account_id_(account_id),
      folder_id_(folder_id)
   {

   }

   void
   IMAPFolderView::Initialize(std::shared_ptr<Messages> messages)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      entries_.clear();
      offset_by_message_id_.clear();
      vanished_.clear();
      highest_uid_seen_ = 0;

      AppendNewMessages(messages);
   }

   int
   IMAPFolderView::AppendNewMessages(std::shared_ptr<Messages> messages)
   {
      if (!messages)
         return 0;

      // GetSnapshot copies the pointers under the collection lock. GetCopy would clone every
      // message object, which is a lot of work for data we don't keep.
      auto snapshot = messages->GetSnapshot();

      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      int added = 0;

      /*
         Messages::Refresh only appends rows, ordered by message UID ascending, so anything
         with a UID above the highest one we've seen is new and belongs at the end. If Refresh
         ever starts reordering or reloading the collection, this no longer holds.
      */
      for (auto message : snapshot)
      {
         unsigned int uid = message->GetUID();

         if (uid <= highest_uid_seen_)
            continue;

         entries_.push_back(IMAPViewEntry(message->GetID(), uid));
         offset_by_message_id_[message->GetID()] = entries_.size() - 1;

         highest_uid_seen_ = uid;
         added++;
      }

      return added;
   }

   int
   IMAPFolderView::GetMessageCount() const
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      return (int) entries_.size();
   }

   bool
   IMAPFolderView::GetEntryBySequence(int sequence, IMAPViewEntry &entry) const
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      if (sequence < 1 || sequence >(int) entries_.size())
         return false;

      entry = entries_[sequence - 1];

      return true;
   }

   bool
   IMAPFolderView::GetEntryByUID(unsigned int uid, int &sequence, IMAPViewEntry &entry) const
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      for (size_t i = 0; i < entries_.size(); i++)
      {
         if (entries_[i].uid != uid)
            continue;

         sequence = (int) i + 1;
         entry = entries_[i];

         return true;
      }

      sequence = 0;

      return false;
   }

   bool
   IMAPFolderView::GetSequenceByMessageID(__int64 message_id, int &sequence) const
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      auto iter = offset_by_message_id_.find(message_id);

      if (iter == offset_by_message_id_.end())
      {
         sequence = 0;
         return false;
      }

      sequence = (int) (*iter).second + 1;

      return true;
   }

   std::vector<std::pair<int, IMAPViewEntry> >
   IMAPFolderView::GetEntriesBySequenceRange(int first, int last) const
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      std::vector<std::pair<int, IMAPViewEntry> > result;

      if (first < 1)
         first = 1;

      if (last < 0 || last >(int) entries_.size())
         last = (int) entries_.size();

      for (int sequence = first; sequence <= last; sequence++)
         result.push_back(std::make_pair(sequence, entries_[sequence - 1]));

      return result;
   }

   std::vector<std::pair<int, IMAPViewEntry> >
   IMAPFolderView::GetEntriesByUIDRange(unsigned int first, unsigned int last) const
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      std::vector<std::pair<int, IMAPViewEntry> > result;

      for (size_t i = 0; i < entries_.size(); i++)
      {
         unsigned int uid = entries_[i].uid;

         if (uid < first || uid > last)
            continue;

         result.push_back(std::make_pair((int) i + 1, entries_[i]));
      }

      return result;
   }

   std::vector<std::pair<int, IMAPViewEntry> >
   IMAPFolderView::GetAllEntries() const
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      std::vector<std::pair<int, IMAPViewEntry> > result;

      for (size_t i = 0; i < entries_.size(); i++)
         result.push_back(std::make_pair((int) i + 1, entries_[i]));

      return result;
   }

   std::vector<int>
   IMAPFolderView::RemoveMessages(const std::vector<__int64> &message_ids)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      std::vector<int> sequences;
      std::set<__int64> removed;

      // Collect the numbers before removing anything, so they all refer to the same view.
      for (__int64 message_id : message_ids)
      {
         auto iter = offset_by_message_id_.find(message_id);

         if (iter == offset_by_message_id_.end())
         {
            // The client was never told about this message, so there's nothing to expunge.
            continue;
         }

         sequences.push_back((int) (*iter).second + 1);
         removed.insert(message_id);
      }

      if (removed.empty())
         return sequences;

      std::vector<IMAPViewEntry> remaining;

      for (const IMAPViewEntry &entry : entries_)
      {
         if (removed.find(entry.message_id) == removed.end())
            remaining.push_back(entry);
      }

      entries_ = remaining;

      for (__int64 message_id : removed)
         vanished_.erase(message_id);

      Rebuild_();

      // Every EXPUNGE shifts the messages after it down one step, so a client applying the
      // responses in order needs the numbers adjusted for the removals reported before them.
      std::sort(sequences.begin(), sequences.end());

      for (size_t i = 0; i < sequences.size(); i++)
         sequences[i] -= (int) i;

      return sequences;
   }

   void
   IMAPFolderView::MarkVanished(__int64 message_id)
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      if (offset_by_message_id_.find(message_id) == offset_by_message_id_.end())
         return;

      vanished_.insert(message_id);
   }

   std::vector<__int64>
   IMAPFolderView::TakeVanished()
   {
      boost::lock_guard<boost::recursive_mutex> guard(mutex_);

      std::vector<__int64> result(vanished_.begin(), vanished_.end());

      vanished_.clear();

      return result;
   }

   void
   IMAPFolderView::Rebuild_()
   {
      offset_by_message_id_.clear();

      for (size_t i = 0; i < entries_.size(); i++)
         offset_by_message_id_[entries_[i].message_id] = i;
   }
}
