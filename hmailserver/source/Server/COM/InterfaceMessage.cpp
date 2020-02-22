// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "COMError.h"
#include "InterfaceMessage.h"
#include "InterfaceAttachments.h"
#include "InterfaceRecipients.h"
#include "InterfaceMessageHeaders.h"

#include "../IMAP/MessagesContainer.h"

#include "../common/Tracking/ChangeNotification.h"
#include "../common/Tracking/NotificationServer.h"

#include "../common/util/Utilities.h"
#include "../common/util/Time.h"
#include "../common/util/MessageUtilities.h"
#include "../SMTP/RecipientParser.h"
#include "../SMTP/SMTPDeliveryManager.h"

#include "../Common/Cache/CacheContainer.h"
#include "../Common/BO/Account.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/Messages.h"
#include "../Common/BO/MessageRecipients.h"
#include "../Common/Persistence/PersistentMessage.h"
#include "../Common/Persistence/PersistentAccount.h"

#include "../Common/Mime/Mime.h"

#include "atlsafe.h"

STDMETHODIMP 
InterfaceMessage::InterfaceSupportsErrorInfo(REFIID riid)
{
   static const IID* arr[] = 
   {
      &IID_IInterfaceMessage,
   };

   for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
   {
      if (InlineIsEqualGUID(*arr[i],riid))
         return S_OK;
   }
   return S_FALSE;
}
   
InterfaceMessage::InterfaceMessage()
{
   object_ = std::shared_ptr<HM::Message>(new HM::Message());
}

STDMETHODIMP InterfaceMessage::get_ID(hyper *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetID();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_UID(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetUID();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_InternalDate(VARIANT *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      HM::DateTime dt = HM::Time::GetDateFromSystemDate(object_->GetCreateTime());
   
      *pVal  = dt.GetVariant();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_Filename(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      std::shared_ptr<const HM::Account> account;
      
      if (object_->GetAccountID() > 0)
      {
         account = HM::CacheContainer::Instance()->GetAccount(object_->GetAccountID());
      }
   
      *pVal = HM::PersistentMessage::GetFileName(account, object_).AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_Subject(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = GetMessageData_()->GetSubject().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::put_Subject(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      GetMessageData_()->SetSubject(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_Charset(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = GetMessageData_()->GetCharset().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::put_Charset(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      GetMessageData_()->SetCharset(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_From(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = GetMessageData_()->GetFrom().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::put_From(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      GetMessageData_()->SetFrom(newVal);  
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_Date(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = GetMessageData_()->GetSentTime().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::put_Date(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      GetMessageData_()->SetSentTime(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_Body(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = GetMessageData_()->GetBody().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::put_Body(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      GetMessageData_()->SetBody(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_HTMLBody(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = GetMessageData_()->GetHTMLBody().AllocSysString();
      
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::put_HTMLBody(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      GetMessageData_()->SetHTMLBody(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_Attachments(IInterfaceAttachments **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceAttachments>* pItem = new CComObject<InterfaceAttachments>();
      pItem->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Attachments> pAttachments = GetMessageData_()->GetAttachments();
   
      if (pAttachments)
      {
         pItem->Attach(pAttachments);
         pItem->AddRef();
         *pVal = pItem;
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_Headers(IInterfaceMessageHeaders **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      std::shared_ptr<HM::MimeBody> pMimeBody = GetMessageData_()->GetMimeMessage();
   
      if (!pMimeBody)
         return DISP_E_BADINDEX;
   
      CComObject<InterfaceMessageHeaders>* pItem = new CComObject<InterfaceMessageHeaders>();
   
      pItem->AttachItem(pMimeBody);
      pItem->AddRef();
      *pVal = pItem;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceMessage::SaveNewMessageToIMAPFolder_()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      // Check which account this message belongs to.
      std::shared_ptr<const HM::Account> pAccount = HM::CacheContainer::Instance()->GetAccount(object_->GetAccountID());
   
      // Save the message to the database
      if (!HM::PersistentMessage::SaveObject(object_))
         return S_FALSE;
   
      // Add to parent collection
      HM::MessagesContainer::Instance()->SetFolderNeedsRefresh(object_->GetFolderID());
   
      // Notify...
      std::shared_ptr<HM::ChangeNotification> pNotification = 
         std::shared_ptr<HM::ChangeNotification>(new HM::ChangeNotification(object_->GetAccountID(), object_->GetFolderID(), HM::ChangeNotification::NotificationMessageAdded));
   
      HM::Application::Instance()->GetNotificationServer()->SendNotification(pNotification);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      // Check that the message has a valid date header.
      HM::String sDate = GetMessageData_()->GetSentTime();
      if (sDate.IsEmpty())
      {
         // Date was not specified. Specify it now.
         sDate = HM::Time::GetCurrentMimeDate();
         GetMessageData_()->SetSentTime(sDate);
      }
   
      std::shared_ptr<const HM::Account> account;
   
      if (object_->GetAccountID() > 0)
      {
         account = HM::CacheContainer::Instance()->GetAccount(object_->GetAccountID());
      }
   
      // Save the message to disk.
      const HM::String fileName = HM::PersistentMessage::GetFileName(account, object_);
   
      if (!GetMessageData_()->Write(fileName))
      {
         return COMError::GenerateError("Unable to write to message file.");
      }
   
      // A message can be in a number of different states:
      // Case 1) New message which should be delivered -> Save in file and add to database
      // Case 2) New message which should be added to an existing IMAP folder -> Update message file and save to database// 
      // case 3) Existing message which is being delivered. -> Only update the message fil
      // Case 4) Existing message in IMAP folder which should just be re-saved -> Update message file
   
      HM::Message::State state = object_->GetState();
   
      switch (state)
      {
      case HM::Message::Created:
         {
            // Handle new message. It can either be Case 1 or Case 2. If the message is already
            // connected to an account, it means that it should be stored in a specific IMAP folder
            if (object_->GetFolderID() == 0 && object_->GetAccountID() == 0)
            {
               // Case 1: The message should be delivered. Change the state to delivering
               object_->SetState(HM::Message::Delivering);
   
               if (!HM::PersistentMessage::SaveObject(object_))
               {
                  return COMError::GenerateError("Message could not be saved in database.");
               }
   
               HM::Application::Instance()->SubmitPendingEmail();
            }  
            else
            {
               // Case 2. It's a new message but it should be added to an existing IMAP folder.
               object_->SetState(HM::Message::Delivered);
               return SaveNewMessageToIMAPFolder_();
            }
   
            break;
         }
      case HM::Message::Delivering:
         {
            // Handle message which is being delivered. Saving in database will be taken
            // care about by the delivery process. Since the file has already been updated
            // on disk, there's not more for us to do here.
            break;
         }
      case HM::Message::Delivered:
         {
            // The message has already been delivered. It's placed inside an account mailbox.
            // All we need to do is to update it in the database.
            if (!HM::PersistentMessage::SaveObject(object_))
               return S_FALSE;
   
            break;
         }
      default:
         {
            // Unhandled case.
            return COMError::GenerateError("The message could not be saevd. It is in an unknown state.");
         }
      }
      
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::RefreshContent()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      // No need to actually refresh anything now, this
      // will be taken care of by the GetMessageData_()
      // when the client tries to access anything after
      // calling this function.
   
      msg_data_.reset();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::Copy(long lDestinationFolderID)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      // The source message must be located in a users account.
      if (object_->GetAccountID() == 0)
         return S_FALSE;
   
      bool bSuccess = HM::MessageUtilities::CopyToIMAPFolder(object_, lDestinationFolderID);
      if (!bSuccess)
      {
         return COMError::GenerateError("Unable to copy message.");
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_To(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = GetMessageData_()->GetTo().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_CC(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = GetMessageData_()->GetCC().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::ClearRecipients()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->GetRecipients()->Clear();
   
      GetMessageData_()->SetTo("");
      GetMessageData_()->SetCC("");
      GetMessageData_()->SetBCC("");
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::AddRecipient(BSTR bstrName, BSTR bstrAddress)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      // Add this recipent to the actual email.
      HM::String sAddress(bstrAddress);
      HM::String sName (bstrName);
   
      bool recipientOK = false;
      HM::RecipientParser recipientParser;
      recipientParser.CreateMessageRecipientList(sAddress, object_->GetRecipients(), recipientOK);
   
      // Add this recipient to the mime message.
      HM::String sThisAddress = "\"" + sName + "\"" + " <" + sAddress + ">";
      HM::String sTo = GetMessageData_()->GetTo();
   
      if (!sTo.IsEmpty())
         sTo += ",";
   
      sTo += sThisAddress;
   
      GetMessageData_()->SetTo(sTo);
      
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_FromAddress(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetFromAddress().AllocSysString();
   return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::put_FromAddress(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetFromAddress(newVal);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_State(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetState();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_DeliveryAttempt(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetNoOfRetries() + 1;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_Size(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetSize() / 1024;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_Recipients(IInterfaceRecipients**pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceRecipients>* pItem = new CComObject<InterfaceRecipients>();
      pItem->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Message> pMessage = GetMessageData_()->GetMessage();
   
      if (pMessage)
      {
         pItem->Attach(pMessage);
         pItem->AddRef();
         *pVal = pItem;
      }
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

std::shared_ptr<HM::MessageData> 
InterfaceMessage::GetMessageData_()
{
   if (!msg_data_)
   {
      std::shared_ptr<const HM::Account> account;

      if (object_->GetAccountID() > 0)
      {
         account = HM::CacheContainer::Instance()->GetAccount(object_->GetAccountID());
      }

      msg_data_ = std::shared_ptr<HM::MessageData>(new HM::MessageData());
      msg_data_->LoadFromMessage(account, object_);
   }

   return msg_data_;
}

STDMETHODIMP InterfaceMessage::get_HeaderValue(BSTR FieldName, BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = GetMessageData_()->GetFieldValue(FieldName).AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::put_HeaderValue(BSTR FieldName, BSTR FieldValue)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      GetMessageData_()->SetFieldValue(FieldName, FieldValue);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::HasBodyType(BSTR BodyType, VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = GetMessageData_()->GetHasBodyType(BodyType) ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::get_EncodeFields(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = GetMessageData_()->GetEncodeFields() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceMessage::put_EncodeFields(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      GetMessageData_()->SetEncodeFields(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceMessage::get_Flag(eMessageFlag iFlag, VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      int iFlags = object_->GetFlags();
      *pVal = (iFlags & iFlag) > 0 ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceMessage::put_Flag(eMessageFlag iFlag, VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      int iFlags = object_->GetFlags();
      
      if (newVal == VARIANT_TRUE)
         iFlags = iFlags | iFlag;
      else
         iFlags = iFlags & ~iFlag;
   
      object_->SetFlags(iFlags);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

