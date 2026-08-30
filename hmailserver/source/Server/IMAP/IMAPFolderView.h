// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Messages;

   struct IMAPViewEntry
   {
      IMAPViewEntry() :
         message_id(0),
         uid(0)
      {
      }

      IMAPViewEntry(__int64 message_id, unsigned int uid) :
         message_id(message_id),
         uid(uid)
      {
      }

      __int64 message_id;
      unsigned int uid;
   };

   /*
      One per selected mailbox per IMAP session. Owns this session's mapping between message
      sequence numbers and messages. Sequence numbers must be stable within a session, so the
      view may only shrink where the server is allowed to send an untagged EXPUNGE.

      The view holds message ids only. MessagesContainer still owns the message objects.

      This class is a leaf: it never calls back into the connection, the notification server,
      the database or the socket.
   */
   class IMAPFolderView
   {
   public:

      IMAPFolderView(__int64 account_id, __int64 folder_id);

      __int64 GetAccountID() const { return account_id_; }
      __int64 GetFolderID() const { return folder_id_; }

      // Replaces the view with the current contents of the folder. SELECT and EXAMINE only.
      void Initialize(std::shared_ptr<Messages> messages);

      // Adds messages which have arrived since the view was last updated. Returns the number
      // of messages added. Appending never renumbers messages the client already knows about.
      int AppendNewMessages(std::shared_ptr<Messages> messages);

      int GetMessageCount() const;

      bool GetEntryBySequence(int sequence, IMAPViewEntry &entry) const;
      bool GetEntryByUID(unsigned int uid, int &sequence, IMAPViewEntry &entry) const;
      bool GetSequenceByMessageID(__int64 message_id, int &sequence) const;

      std::vector<std::pair<int, IMAPViewEntry> > GetEntriesBySequenceRange(int first, int last) const;
      std::vector<std::pair<int, IMAPViewEntry> > GetEntriesByUIDRange(unsigned int first, unsigned int last) const;
      std::vector<std::pair<int, IMAPViewEntry> > GetAllEntries() const;

      // Removes the given messages. Returns the sequence numbers to report to the client, in
      // the order they should be sent, each adjusted for the removals preceding it. May only
      // be called where an untagged EXPUNGE is allowed.
      std::vector<int> RemoveMessages(const std::vector<__int64> &message_ids);

      // A command found a message in this view which no longer exists. It's expunged from the
      // view the next time an untagged EXPUNGE may be sent.
      void MarkVanished(__int64 message_id);
      std::vector<__int64> TakeVanished();

   private:

      void Rebuild_();

      mutable boost::recursive_mutex mutex_;

      // Ordered by UID ascending, which is the IMAP message order.
      std::vector<IMAPViewEntry> entries_;
      std::map<__int64, size_t> offset_by_message_id_;

      std::set<__int64> vanished_;

      unsigned int highest_uid_seen_;

      __int64 account_id_;
      __int64 folder_id_;
   };
}
