// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "MessageIndexer.h"
#include "../BO/Message.h"
#include "../BO/MessageMetaData.h"
#include "../MIME/MIME.h"
#include "../Util/Time.h"
#include "../Threading/WorkQueue.h"
#include "../Persistence/PersistentMessageMetaData.h"
#include "../Persistence/PersistentMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   CriticalSection MessageIndexer::_starterLock;

   MessageIndexer::MessageIndexer() :
      Task("MessageIndexer")
   {
      
   }

   MessageIndexer::~MessageIndexer(void)
   {

   }

   void
   MessageIndexer::Start()
   {
      CriticalSectionScope scope(_starterLock);

      shared_ptr<MessageIndexer> indexer = _GetRunningIndexer();

      if (indexer)
      {
         // already started.
         return;
      }

      shared_ptr<WorkQueue> pWorkQueue = Application::Instance()->GetRandomWorkQueue();

      // Start the indexer now.
      LOG_DEBUG("Starting message indexing thread...");
      shared_ptr<MessageIndexer> messageIndexer = shared_ptr<MessageIndexer>(new MessageIndexer);
      pWorkQueue->AddTask(messageIndexer);
   }

   void
   MessageIndexer::Stop()
   {
      LOG_DEBUG("Stopping message indexing thread...");

      CriticalSectionScope scope(_starterLock);

      shared_ptr<WorkQueue> pWorkQueue = Application::Instance()->GetRandomWorkQueue();
      pWorkQueue->StopTask("MessageIndexer");

   }

   shared_ptr<MessageIndexer>
   MessageIndexer::_GetRunningIndexer()
   {
      try
      {
         // This is a very expensive operation. Luckily we don't need to do it very often.
         shared_ptr<WorkQueue> pWorkQueue = Application::Instance()->GetRandomWorkQueue();
         bool queued = false;
         shared_ptr<Task> task = pWorkQueue->GetTaskByName("MessageIndexer", queued);

         if (!task)
         {
            shared_ptr<MessageIndexer> nothing;
            return nothing;
         }

         shared_ptr<MessageIndexer> result = shared_static_cast<MessageIndexer>(task);
         return result;
      }
      catch (...)
      {
         shared_ptr<MessageIndexer> nothing;
         return nothing;
      }

   }

   void
   MessageIndexer::DoWork()
   {
      LOG_DEBUG("Indexing messages...");

      try
      {
         PersistentMessageMetaData persistentMetaData;
         persistentMetaData.DeleteOrphanedItems();

         while (true)
         {
            _IndexMessages();

            // We are currently not fetching anything
            // Sit here and wait a minute 
            const int iSize = 2;
            HANDLE handles[iSize];

            handles[0] = _stopRunning.GetHandle();
            handles[1] = _indexNow.GetHandle();

            DWORD dwWaitResult = WaitForMultipleObjects(iSize, handles, FALSE, 60000);

            int iEvent = dwWaitResult - WAIT_OBJECT_0;

            switch (iEvent)
            {
            case 0:
               LOG_DEBUG("Message indexing thread exiting...");
               _stopRunning.Reset();
               return;
            case 1:
               _indexNow.Reset();
               break;
            }
         }
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5158, "MessageIndexer::DoWork", "An error occured while indexing messages. The indexing was aborted.");
      }
   }

   void 
   MessageIndexer::IndexNow()
   {
      shared_ptr<MessageIndexer> indexer = _GetRunningIndexer();

      if (indexer)
      {
         indexer->_indexNow.Set();
      }
   }

   void
   MessageIndexer::StopWork()
   {
      _stopRunning.Set();
   }

   void 
   MessageIndexer::_IndexMessages()
   {
      // Default is 720
      int iIndexerFullMin = IniFileSettings::Instance()->GetIndexerFullMinutes();

      PersistentMessageMetaData persistentMetaData;
      // set quickIndex here so the loop will allow loading of an incomplete index
      bool bDoQuickIndex = true;
      if ((iIndexRunCount > iIndexerFullMin) || (iIndexRunCount == 0)){
        bDoQuickIndex = false;
        iIndexRunCount = 1;
      }

      while (true)
      {
         // added the boolean quickIndex to tell the funciton to use the quick index or the full index
         set<shared_ptr<PersistentMessageMetaData::MessageInfo> > messagesToIndex = persistentMetaData.GetMessagesToIndex(bDoQuickIndex);
         if (messagesToIndex.size() == 0)
         {
            LOG_DEBUG("No messages to index.");
            iIndexRunCount++;  //JDR
            // No messages to index found.
            return;
         }

         boost_foreach(shared_ptr<PersistentMessageMetaData::MessageInfo> messageToIndex, messagesToIndex)
         {
            if (_stopRunning.IsSet())
               return;

            AnsiString headerText = PersistentMessage::LoadHeader(messageToIndex->FileName, false);

            MimeHeader header;
            header.Load(headerText, headerText.GetLength(), true);


            String dateString = header.GetUnicodeFieldValue("Date");
            DateTime date = Time::GetDateTimeFromMimeHeader(dateString);

            String from = header.GetUnicodeFieldValue("From");
            String subject = header.GetUnicodeFieldValue("Subject");
            String cc = header.GetUnicodeFieldValue("CC");
            String to = header.GetUnicodeFieldValue("TO");

            shared_ptr<MessageMetaData> metaData = shared_ptr<MessageMetaData>(new MessageMetaData);

            metaData->SetAccountID(messageToIndex->AccountID);
            metaData->SetFolderID(messageToIndex->FolderID);
            metaData->SetMessageID(messageToIndex->MessageID);

            metaData->SetDate(date);
            metaData->SetFrom(from);
            metaData->SetSubject(subject);
            metaData->SetCC(cc);
            metaData->SetTo(to);

            if (!persistentMetaData.SaveObject(metaData))
            {
               LOG_DEBUG("Error saving the index.")
               // Error saving. Abort now...
               return;
            }
         }
      }
      iIndexRunCount++;
   }
}
