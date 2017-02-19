// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceMessages.h"
#include "InterfaceMessage.h"

#include "../IMAP/MessagesContainer.h"
#include "../Common/BO/Message.h"
#include "../Common/Persistence/PersistentMessage.h"

#include "../Common/Tracking/ChangeNotification.h"
#include "../Common/Tracking/NotificationServer.h"

STDMETHODIMP InterfaceMessages::get_Count(long *pVal)
{
   try
   {
      if (!messages_)
         return GetAccessDenied();

      *pVal = messages_->GetCount();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

void
InterfaceMessages::Attach(std::shared_ptr<HM::Messages> pMessages)
{
   messages_ = pMessages;
}

STDMETHODIMP InterfaceMessages::get_Item(long Index, IInterfaceMessage **pVal)
{
   try
   {
      if (!messages_)
         return GetAccessDenied();

      CComObject<InterfaceMessage>* pInterfaceMessage = new CComObject<InterfaceMessage>();
      pInterfaceMessage->SetAuthentication(authentication_);

      std::shared_ptr<HM::Message> pMsg = messages_->GetItem(Index);

      if (pMsg)
      {
         pInterfaceMessage->AttachItem(pMsg);
         pInterfaceMessage->AddRef();
         *pVal = pInterfaceMessage;
      }
      else
      {
         return DISP_E_BADINDEX;
      }

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessages::get_ItemByDBID(hyper DBID, IInterfaceMessage **pVal)
{
   try
   {
      if (!messages_)
         return GetAccessDenied();

      //	

      CComObject<InterfaceMessage>* pInterfaceMessage = new CComObject<InterfaceMessage>();
      pInterfaceMessage->SetAuthentication(authentication_);

      std::shared_ptr<HM::Message> pMsg = messages_->GetItemByDBID(DBID);

      if (pMsg)
      {
         pInterfaceMessage->AttachItem(pMsg);
         pInterfaceMessage->AddRef();
         *pVal = pInterfaceMessage;
      }
      else
      {
         return DISP_E_BADINDEX;
      }

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessages::Add(IInterfaceMessage **pVal)
{
   try
   {
      if (!messages_)
         return GetAccessDenied();

      __int64 iAccountID = messages_->GetAccountID();
      __int64 iFolderID = messages_->GetFolderID();

      if (iFolderID == -1)
      {
         // We can't add a new message when we're not browsing the
         // message cache.
         return DISP_E_BADINDEX;
      }

      std::shared_ptr<HM::Message> pNewMsg = std::shared_ptr<HM::Message>(new HM::Message);
      pNewMsg->SetAccountID(iAccountID);
      pNewMsg->SetFolderID(iFolderID);
      pNewMsg->SetState(HM::Message::Created);

      CComObject<InterfaceMessage>* pInterfaceMessage = new CComObject<InterfaceMessage>();

      pInterfaceMessage->AttachParent(messages_, false);
      pInterfaceMessage->SetAuthentication(authentication_);
      pInterfaceMessage->AttachItem(pNewMsg);
      pInterfaceMessage->AddRef();
      *pVal = pInterfaceMessage;

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessages::DeleteByDBID(hyper lDBID)
{
   try
   {
      if (!messages_)
         return GetAccessDenied();

      std::shared_ptr<HM::Message> pMsg = messages_->GetItemByDBID(lDBID);

      if (!pMsg)
      {
         // No such message exists
         return DISP_E_BADINDEX;
      }

      // Expunge the mailbox. Will cause the message to be
      // deleted from disk and database.
      std::function<bool(int, std::shared_ptr<HM::Message>)> filter = [lDBID](int index, std::shared_ptr<HM::Message> message)
      {
         if (message->GetID() == lDBID)
         {
            return true;
         }

         return false;
      };

      messages_->DeleteMessages(filter);

      // If we're aren't browsing in the message cache already, 
      // we need to delete the message from the cache now. This
      // is needed if the messages are accessed using the
      // Account.Messages property. When messages are accessed
      // via Account.IMAPFolders, we access the message cache
      // directly, and when we don't need to remove message here.
      // (Since it was done like 10 lines up).

      __int64 iFolderID = messages_->GetFolderID();
      if (iFolderID == -1)
      {
         // We're not browsing the message cache.
         __int64 iAccountID = messages_->GetAccountID();


         std::vector<__int64> affectedMessages;
         affectedMessages.push_back(lDBID);

         // Notify clients that the message has been dropped.
         std::shared_ptr<HM::ChangeNotification> notification =
            std::shared_ptr<HM::ChangeNotification>(new HM::ChangeNotification(messages_->GetAccountID(), messages_->GetFolderID(), HM::ChangeNotification::NotificationMessageDeleted, affectedMessages));

         HM::Application::Instance()->GetNotificationServer()->SendNotification(notification);
      }

      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceMessages::Clear()
{
   try
   {
      if (!messages_)
         return GetAccessDenied();

      messages_->DeleteAll();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


