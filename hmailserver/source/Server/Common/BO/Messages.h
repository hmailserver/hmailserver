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

      std::shared_ptr<Message> GetItemByUID(unsigned int uid);
      std::shared_ptr<Message> GetItemByUID(unsigned int uid, unsigned int &foundIndex);

      void DeleteMessages(std::function<bool(int, std::shared_ptr<Message>)> &filter);

      void Refresh(bool update_recent_flags);

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
