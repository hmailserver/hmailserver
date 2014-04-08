// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "../BO/Message.h"
#include "../Persistence/PersistentMessage.h"

namespace HM
{

   typedef std::vector<shared_ptr<Message> > MessagesVector;
   typedef std::vector<shared_ptr<Message> >::iterator MessagesIterator;

   class Messages : public Collection<Message, PersistentMessage>
   {
   public:
	   Messages(__int64 iAccountID, __int64 iFolderID);
	   virtual ~Messages();

      void Save();
      void UndeleteAll();

      long GetSize() const;
      __int64 GetFirstUnseenUID() const;
      long GetNoOfSeen() const;
      long GetNoOfRecent() const;
      
      std::vector<shared_ptr<Message>> GetCopy();
      std::vector<int> Expunge();
      std::vector<int> Expunge(bool messagesMarkedForDeletion, const std::set<int> &uids, const boost::function<void()> &func);
      std::vector<int> DeleteMessages();

      shared_ptr<Message> GetItemByUID(unsigned int uid);
      shared_ptr<Message> GetItemByUID(unsigned int uid, unsigned int &foundIndex);

      void Refresh();

      bool DeleteMessageByDBID(__int64 ID);

      void AddToCollection(shared_ptr<DALRecordset> pRS);
      
      void Remove(__int64 iDBID);

      void SetFlagRecentOnMessages(bool bRecent);
      void AddItem(shared_ptr<Message> pObject);

      __int64 GetAccountID() {return m_iAccountID; }
      __int64 GetFolderID() {return m_iFolderID; }

   protected:
      virtual String GetCollectionName() const {return "Messages"; }
      virtual bool PreSaveObject(shared_ptr<Message> pMessage, XNode *node);
   private:

      unsigned int _lastRefreshedUID;

      __int64 m_iAccountID;
      __int64 m_iFolderID;
   };
}
