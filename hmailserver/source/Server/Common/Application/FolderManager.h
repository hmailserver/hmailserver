// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class Account;
   class IMAPFolder;

   class FolderManager
   {
   public:
      FolderManager(void);
      ~FolderManager(void);

      bool GetInboxMessages(int accountID, vector<shared_ptr<Message> > &result);
      bool DeleteInboxMessages(int accountID, set<int> uids, const boost::function<void()> &func);

      bool UpdateMessageFlags(int accountID, int folderID, __int64 messageID, int flags);

	private:


   };
}