// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Messages.h"

using namespace std;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{


   Messages::Messages(__int64 iAccountID, __int64 iFolderID) :
      account_id_(iAccountID),
      folder_id_(iFolderID),
      last_refreshed_uid_(0)
   {

   }

   Messages::~Messages()
   {
   
   }

   std::vector<std::shared_ptr<Message>>
   Messages::GetCopy()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      std::vector<std::shared_ptr<Message>> result;

      for(std::shared_ptr<Message> oMessage : vecObjects)
      {
         std::shared_ptr<Message> messageCopy = std::shared_ptr<Message>(new Message(*oMessage.get()));
         result.push_back(messageCopy);
      }

      return result;

   }

   long
   Messages::GetNoOfSeen() const
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      long lNoOfSeen = 0;

      for(std::shared_ptr<Message> oMessage : vecObjects)
      {
         if (oMessage->GetFlagSeen()) 
            lNoOfSeen ++;
      }

      return lNoOfSeen;
   }

   long
   Messages::GetNoOfRecent() const
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      long lNoOfRecent = 0;

      for(std::shared_ptr<Message> message : vecObjects)
      {
         if (message->GetFlagRecent()) 
            lNoOfRecent ++;

      }

      return lNoOfRecent;
   }

   long
   Messages::GetSize() const
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      long lSize = 0;

      for(std::shared_ptr<Message> oMessage : vecObjects)
      {
         lSize += oMessage->GetSize();
      }

      return lSize;
   }

   __int64
   Messages::GetFirstUnseenUID() const
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      for(std::shared_ptr<Message> message : vecObjects)
      {
         if (!message->GetFlagSeen())
            return message->GetUID();
      }

      return 0;
   }


   void
   Messages::Save()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      LOG_DEBUG("Messages::Save()");

      for(std::shared_ptr<Message> oMessage : vecObjects)
      {
         LOG_DEBUG("Messages::Save() - Iteration");

         if (oMessage->GetFlagDeleted()) 
            PersistentMessage::DeleteObject(oMessage);
         else
            PersistentMessage::SaveObject(oMessage);
      }

      LOG_DEBUG("Messages::~Save()");
   
   }

   std::vector<int>
   Messages::Expunge()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      std::function<void()> func;
      std::set<int> uids;

      return Expunge(true, uids, func);
   }

   std::vector<int>
   Messages::Expunge(bool messagesMarkedForDeletion, const std::set<int> &uids, const std::function<void()> &func)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      std::vector<int> vecExpungedMessages;
      std::vector<std::shared_ptr<Message> >::iterator iterMessage = vecObjects.begin();

      long lIndex = 0;
      int expungedCount = 0;
      while (iterMessage != vecObjects.end())
      {
         lIndex++;

         std::shared_ptr<Message> pCurMsg = (*iterMessage);

         if ((messagesMarkedForDeletion && pCurMsg->GetFlagDeleted()) ||
             uids.find(pCurMsg->GetUID()) != uids.end())
         {
            PersistentMessage::DeleteObject(pCurMsg);
            vecExpungedMessages.push_back(lIndex);
            iterMessage = vecObjects.erase(iterMessage);
            lIndex--;
            expungedCount++;
         }
         else
            iterMessage++;

         if (expungedCount > 1000)
         {
            if (func)
               func();

            expungedCount = 0;
         }
      }

      return vecExpungedMessages;
   }

   std::vector<int>
   Messages::DeleteMessages()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      std::vector<std::shared_ptr<Message> >::iterator iterMessage = vecObjects.begin();
      std::vector<int> vecExpungedMessages;

      long lIndex = 0;
      while (iterMessage != vecObjects.end())
      {
         lIndex++;

         std::shared_ptr<Message> pCurMsg = (*iterMessage);

         PersistentMessage::DeleteObject(pCurMsg);
         vecExpungedMessages.push_back(lIndex);
         iterMessage = vecObjects.erase(iterMessage);
         lIndex--;
      }

      return vecExpungedMessages;
   }

   void
   Messages::UndeleteAll()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

   }

   void
   Messages::Refresh()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

	  // int startTime = GetTickCount();

      bool retrieveQueue = account_id_ == -1;

      if (retrieveQueue && last_refreshed_uid_ > 0)
      {
         /*
            We can't do partial refreshes of messages in the queue. Why? 
            Because we use the message UID to determine which part of the
            queue we need to read from the database, and UID's aren't given
            to messages before they are inserted into the queue.
         */
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5204, "Messages::Refresh", "Refresh not supported on the current collection.");
         return;

      }

      SQLCommand command;

      // Build SQL statement that will be used to fetch list of messages.
      String sSQL;
      sSQL = "select * from hm_messages where "; 

      if (retrieveQueue)
         sSQL += " messagetype = 1 ";
      else
      {
         // Messages connected to a specific account
         sSQL += _T(" messageaccountid = @MESSAGEACCOUNTID ");
         command.AddParameter("@MESSAGEACCOUNTID", account_id_); 
      }
  
      // Should we fetch a specific folder?
      if (folder_id_ != -1)
      {
         sSQL.AppendFormat(_T(" and messagefolderid = @MESSAGEFOLDERID "));
         command.AddParameter("@MESSAGEFOLDERID", folder_id_); 
      }

      // Should we do an incremental refresh?
      if (last_refreshed_uid_ > 0)
      {
         sSQL.AppendFormat(_T(" and messageuid > @MESSAGEUID "));
         command.AddParameter("@MESSAGEUID", last_refreshed_uid_); 
      }

      if (retrieveQueue)
         sSQL += " order by messageid asc";
      else
         sSQL += " order by messageuid asc";


      command.SetQueryString(sSQL);

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return;

	  // int timeAfterOpenRecordset = GetTickCount();
   
      // Do this before we actually read the messages, so that we does not
      // mark any unread message as recent. If we haven't actually read
      // any messages, there's nothing to mark as read...
      if (account_id_ != -1 && !pRS->IsEOF())
      {
         // Mark all messages as recent
         String sql = "update hm_messages set messageflags = messageflags & ~ @FLAGS where messageaccountid = @ACCOUNTID ";
         
         SQLCommand updateCommand;
         updateCommand.AddParameter("@FLAGS", Message::FlagRecent);
         updateCommand.AddParameter("@ACCOUNTID", account_id_);

         if (folder_id_ > 0)
         {
            sql.AppendFormat(_T(" and messagefolderid = @FOLDERID "), folder_id_);
            updateCommand.AddParameter("@FOLDERID", folder_id_);
         }

         updateCommand.SetQueryString(sql);

         Application::Instance()->GetDBManager()->Execute(updateCommand);
      }

      AddToCollection(pRS);

	  // int timeAfterReadingRecordset = GetTickCount();

	  // 
	  //
	  // LOG_DEBUG(Formatter::Format("Messages::Refresh - Time to load recordset {0}", timeAfterOpenRecordset - startTime));
	  // LOG_DEBUG(Formatter::Format("Messages::Refresh - Time to read recordset {0}", timeAfterReadingRecordset - timeAfterOpenRecordset));
   }

   void
   Messages::AddToCollection(std::shared_ptr<DALRecordset> pRS)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      if (!pRS->IsEOF())
      {
         long lRecCount = pRS->RecordCount();
         if (lRecCount < 0)
            lRecCount = 0;

		 LOG_DEBUG("Reading messages from database.");

         vecObjects.reserve(vecObjects.size() + lRecCount);

         while (!pRS->IsEOF())
         {
            std::shared_ptr<Message> msg = std::shared_ptr<Message> (new Message(false));
            PersistentMessage::ReadObject(pRS, msg, false);
                  
            vecObjects.push_back(msg);

            lRecCount--;

            pRS->MoveNext();
         }

         std::shared_ptr<Message> pLastMessage = vecObjects[vecObjects.size() -1];
         last_refreshed_uid_ = pLastMessage->GetUID();
      }
   }

   void 
   Messages::AddItem(std::shared_ptr<Message> pObject)
   {
      // Rather than adding the message, refresh entire folder
      // content from database.
      Refresh();
   }

   bool
   Messages::DeleteMessageByDBID(__int64 ID)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Deletes a message from the collection with the given database identifier.
   //---------------------------------------------------------------------------()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      for(std::shared_ptr<Message> pCurMsg : vecObjects)
      {
         if (pCurMsg->GetID() == ID)
         {
            pCurMsg->SetFlagDeleted(true);
            return true;
         }
      }

      return false;
   }

   void  
   Messages::SetFlagRecentOnMessages(bool bRecent)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      for(std::shared_ptr<Message> message : vecObjects)
      {
         message->SetFlagRecent(bRecent);
      }
   }

   bool
   Messages::PreSaveObject(std::shared_ptr<Message> pMessage, XNode *node)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      pMessage->SetAccountID(account_id_);
      pMessage->SetFolderID(folder_id_);
      return true;
   }

   void 
   Messages::Remove(__int64 iDBID)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Removes a message from the message list. This function does not delete
   // the message from the actual database. It should only be used to remove
   // an object, that has already been deleted in the database.
   //---------------------------------------------------------------------------()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      std::vector<std::shared_ptr<Message> >::iterator iterMessage = vecObjects.begin();

      // Locate the message
      while (iterMessage != vecObjects.end() && (*iterMessage)->GetID() != iDBID)
         iterMessage++;

      // If the message is found, remove it from the list now
      if (iterMessage != vecObjects.end())
         vecObjects.erase(iterMessage);
   }

   std::shared_ptr<Message>
   Messages::GetItemByUID(unsigned int uid)
   {
      unsigned int dummy = 0;
      return GetItemByUID(uid, dummy);
   }


   std::shared_ptr<Message>
   Messages::GetItemByUID(unsigned int uid, unsigned int &foundIndex)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);
      foundIndex = 0;
      for(std::shared_ptr<Message> item : vecObjects)
      {
         foundIndex++;

         if (item->GetUID() == uid)
            return item;
      }

      std::shared_ptr<Message> empty;
      return empty;
   }

}
