// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "../BO/Message.h"
#include "../Persistence/PersistentMessage.h"

namespace HM
{

   class Messages : public Collection<Message, PersistentMessage>
   {
   public:
	   Messages(__int64 iAccountID, __int64 iFolderID);
	   virtual ~Messages();

      void Save();

      long GetSize() const;
      __int64 GetFirstUnseenUID() const;
      long GetNoOfSeen() const;
      
      std::vector<std::shared_ptr<Message>> GetCopy();

      // Copies of the messages with the given database ids, keyed on id. One pass over the
      // collection, so a caller resolving many ids doesn't scan the collection once per id.
      std::map<__int64, std::shared_ptr<Message>> GetCopyByIds(const std::set<__int64> &message_ids) const;
      std::shared_ptr<Message> GetCopyByDBID(__int64 message_id) const;

      void GetRecentMessages(std::set<__int64> &recent_messages) const;

      std::shared_ptr<Message> GetItemByUID(unsigned int uid);
      std::shared_ptr<Message> GetItemByUID(unsigned int uid, unsigned int &foundIndex);

      // Deletes the messages the filter selects. Returns the database ids of the messages
      // which were actually deleted, which is what change notifications carry.
      std::vector<__int64> DeleteMessages(const std::function<bool(std::shared_ptr<Message>)> &filter);
      std::vector<__int64> DeleteMessagesById(const std::set<__int64> &message_ids);

      // Returns false if the messages could not be loaded.
      bool Refresh(bool update_recent_flags);

      void AddToCollection(std::shared_ptr<DALRecordset> pRS);
      
      void Remove(__int64 iDBID);

      void RemoveRecentFlags();

      __int64 GetAccountID() {return account_id_; }
      __int64 GetFolderID() {return folder_id_; }

   protected:
      virtual String GetCollectionName() const {return "Messages"; }
      virtual bool PreSaveObject(std::shared_ptr<Message> pMessage, XNode *node);
   private:

      unsigned int last_refreshed_uid_;

      __int64 account_id_;
      __int64 folder_id_;
   };
}
