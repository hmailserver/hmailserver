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
      m_iAccountID(iAccountID),
      m_iFolderID(iFolderID),
      _lastRefreshedUID(0)
   {

   }

   Messages::~Messages()
   {
   
   }

   std::vector<shared_ptr<Message>>
   Messages::GetCopy()
   {
      CriticalSectionScope scope(_lock);

      std::vector<shared_ptr<Message>> result;

      boost_foreach(shared_ptr<Message> oMessage, vecObjects)
      {
         shared_ptr<Message> messageCopy = shared_ptr<Message>(new Message(*oMessage.get()));
         result.push_back(messageCopy);
      }

      return result;

   }

   long
   Messages::GetNoOfSeen() const
   {
      CriticalSectionScope scope(_lock);

      long lNoOfSeen = 0;

      boost_foreach(shared_ptr<Message> oMessage, vecObjects)
      {
         if (oMessage->GetFlagSeen()) 
            lNoOfSeen ++;
      }

      return lNoOfSeen;
   }

   long
   Messages::GetNoOfRecent() const
   {
      CriticalSectionScope scope(_lock);

      long lNoOfRecent = 0;

      boost_foreach(shared_ptr<Message> message, vecObjects)
      {
         if (message->GetFlagRecent()) 
            lNoOfRecent ++;

      }

      return lNoOfRecent;
   }

   long
   Messages::GetSize() const
   {
      CriticalSectionScope scope(_lock);

      long lSize = 0;

      boost_foreach(shared_ptr<Message> oMessage, vecObjects)
      {
         lSize += oMessage->GetSize();
      }

      return lSize;
   }

   __int64
   Messages::GetFirstUnseenUID() const
   {
      CriticalSectionScope scope(_lock);

      boost_foreach(shared_ptr<Message> message, vecObjects)
      {
         if (!message->GetFlagSeen())
            return message->GetUID();
      }

      return 0;
   }


   void
   Messages::Save()
   {
      CriticalSectionScope scope(_lock);

      LOG_DEBUG("Messages::Save()");

      boost_foreach(shared_ptr<Message> oMessage, vecObjects)
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
      CriticalSectionScope scope(_lock);

      boost::function<void()> func;
      std::set<int> uids;

      return Expunge(true, uids, func);
   }

   std::vector<int>
   Messages::Expunge(bool messagesMarkedForDeletion, const std::set<int> &uids, const boost::function<void()> &func)
   {
      CriticalSectionScope scope(_lock);

      std::vector<int> vecExpungedMessages;
      std::vector<shared_ptr<Message> >::iterator iterMessage = vecObjects.begin();

      long lIndex = 0;
      int expungedCount = 0;
      while (iterMessage != vecObjects.end())
      {
         lIndex++;

         shared_ptr<Message> pCurMsg = (*iterMessage);

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
            if (!func.empty())
               func();

            expungedCount = 0;
         }
      }

      return vecExpungedMessages;
   }

   std::vector<int>
   Messages::DeleteMessages()
   {
      CriticalSectionScope scope(_lock);

      std::vector<shared_ptr<Message> >::iterator iterMessage = vecObjects.begin();
      std::vector<int> vecExpungedMessages;

      long lIndex = 0;
      while (iterMessage != vecObjects.end())
      {
         lIndex++;

         shared_ptr<Message> pCurMsg = (*iterMessage);

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
      CriticalSectionScope scope(_lock);



   }

   void
   Messages::Refresh()
   {
      CriticalSectionScope scope(_lock);

	  // int startTime = GetTickCount();

      bool retrieveQueue = m_iAccountID == -1;

      if (retrieveQueue && _lastRefreshedUID > 0)
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
         command.AddParameter("@MESSAGEACCOUNTID", m_iAccountID); 
      }
  
      // Should we fetch a specific folder?
      if (m_iFolderID != -1)
      {
         sSQL.AppendFormat(_T(" and messagefolderid = @MESSAGEFOLDERID "));
         command.AddParameter("@MESSAGEFOLDERID", m_iFolderID); 
      }

      // Should we do an incremental refresh?
      if (_lastRefreshedUID > 0)
      {
         sSQL.AppendFormat(_T(" and messageuid > @MESSAGEUID "));
         command.AddParameter("@MESSAGEUID", _lastRefreshedUID); 
      }

      if (retrieveQueue)
         sSQL += " order by messageid asc";
      else
         sSQL += " order by messageuid asc";


      command.SetQueryString(sSQL);

      shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return;

	  // int timeAfterOpenRecordset = GetTickCount();
   
      // Do this before we actually read the messages, so that we does not
      // mark any unread message as recent. If we haven't actually read
      // any messages, there's nothing to mark as read...
      if (m_iAccountID != -1 && !pRS->IsEOF())
      {
         // Mark all messages as recent
         String sql = "update hm_messages set messageflags = messageflags & ~ @FLAGS where messageaccountid = @ACCOUNTID ";
         
         SQLCommand updateCommand;
         updateCommand.AddParameter("@FLAGS", Message::FlagRecent);
         updateCommand.AddParameter("@ACCOUNTID", m_iAccountID);

         if (m_iFolderID > 0)
         {
            sql.AppendFormat(_T(" and messagefolderid = @FOLDERID "), m_iFolderID);
            updateCommand.AddParameter("@FOLDERID", m_iFolderID);
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
   Messages::AddToCollection(shared_ptr<DALRecordset> pRS)
   {
      CriticalSectionScope scope(_lock);

      if (!pRS->IsEOF())
      {
         long lRecCount = pRS->RecordCount();
         if (lRecCount < 0)
            lRecCount = 0;

		 LOG_DEBUG("Reading messages from database.");

         vecObjects.reserve(vecObjects.size() + lRecCount);

         while (!pRS->IsEOF())
         {
            shared_ptr<Message> msg = shared_ptr<Message> (new Message(false));
            PersistentMessage::ReadObject(pRS, msg, false);
                  
            vecObjects.push_back(msg);

            lRecCount--;

            pRS->MoveNext();
         }

         shared_ptr<Message> pLastMessage = vecObjects[vecObjects.size() -1];
         _lastRefreshedUID = pLastMessage->GetUID();
      }
   }

   void 
   Messages::AddItem(shared_ptr<Message> pObject)
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
      CriticalSectionScope scope(_lock);

      boost_foreach(shared_ptr<Message> pCurMsg, vecObjects)
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
      CriticalSectionScope scope(_lock);

      boost_foreach(shared_ptr<Message> message, vecObjects)
      {
         message->SetFlagRecent(bRecent);
      }
   }

   bool
   Messages::PreSaveObject(shared_ptr<Message> pMessage, XNode *node)
   {
      CriticalSectionScope scope(_lock);

      pMessage->SetAccountID(m_iAccountID);
      pMessage->SetFolderID(m_iFolderID);
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
      CriticalSectionScope scope(_lock);

      std::vector<shared_ptr<Message> >::iterator iterMessage = vecObjects.begin();

      // Locate the message
      while (iterMessage != vecObjects.end() && (*iterMessage)->GetID() != iDBID)
         iterMessage++;

      // If the message is found, remove it from the list now
      if (iterMessage != vecObjects.end())
         vecObjects.erase(iterMessage);
   }

   shared_ptr<Message>
   Messages::GetItemByUID(unsigned int uid)
   {
      unsigned int dummy = 0;
      return GetItemByUID(uid, dummy);
   }


   shared_ptr<Message>
   Messages::GetItemByUID(unsigned int uid, unsigned int &foundIndex)
   {
      CriticalSectionScope scope(_lock);
      foundIndex = 0;
      boost_foreach(shared_ptr<Message> item, vecObjects)
      {
         foundIndex++;

         if (item->GetUID() == uid)
            return item;
      }

      shared_ptr<Message> empty;
      return empty;
   }

}
