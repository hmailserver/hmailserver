// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "..\BO\MessageMetaData.h"

namespace HM
{
   class Message;

   class PersistentMessageMetaData
   {
   public:

      class MessageInfo
      {
      public:
         __int64 MessageID;
         int AccountID;
         int FolderID;
         String FileName;
      };

      PersistentMessageMetaData(void);
      ~PersistentMessageMetaData(void);

      // quickIndex param added to choose new faster/limiting option
      set<shared_ptr<MessageInfo> > GetMessagesToIndex(bool quickIndex);

      bool DeleteForMessage(shared_ptr<Message> message);
      void GetMetaData(int accountID, int folderID, const String &headerField, map<__int64, String > &result);
      bool SaveObject(shared_ptr<MessageMetaData> metaData);
      bool DeleteOrphanedItems();
      int GetTotalMessageCount();
      void Clear();
   };
}