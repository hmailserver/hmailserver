// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "InterfaceIMAPFolder.h"
#include "InterfaceIMAPFolders.h"
#include "InterfaceIMAPFolderPermissions.h"

#include "InterfaceMessages.h"


#include "../Common/Util/Encoding/ModifiedUTF7.h"
#include "../Common/Util/Time.h"
#include "../Common/Persistence/PersistentIMAPFolder.h"

#include "../Common/Tracking/ChangeNotification.h"
#include "../Common/Tracking/NotificationServer.h"


#include "COMError.h"


STDMETHODIMP 
InterfaceIMAPFolder::InterfaceSupportsErrorInfo(REFIID riid)
{
   try
   {
      static const IID* arr[] = 
      {
         &IID_IInterfaceIMAPFolder,
      };
   
      for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
      {
         if (InlineIsEqualGUID(*arr[i],riid))
            return S_OK;
      }
      return S_FALSE;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolder::Save()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (HM::PersistentIMAPFolder::SaveObject(object_))
      {
         // Add to parent collection
         AddToParentCollection();
         return S_OK;
      }
   
      return COMError::GenerateError("Failed to save object. See hMailServer error log.");
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}   
   
void 
InterfaceIMAPFolder::Attach(std::shared_ptr<HM::IMAPFolder> pFolder)
{
   object_ = pFolder;
}

STDMETHODIMP 
InterfaceIMAPFolder::get_ParentID(LONG *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetParentFolderID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolder::get_Name(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      HM::String sUnicode = HM::ModifiedUTF7::Decode(object_->GetFolderName());
      *pVal = sUnicode.AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceIMAPFolder::put_Name(BSTR newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetFolderName(HM::ModifiedUTF7::Encode(newVal));
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolder::get_Subscribed(VARIANT_BOOL *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetIsSubscribed() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolder::put_Subscribed(VARIANT_BOOL newVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      object_->SetIsSubscribed(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolder::get_Messages(IInterfaceMessages **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

   
      CComObject<InterfaceMessages>* pItem = new CComObject<InterfaceMessages>();
      pItem->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Messages> pMessages = object_->GetMessages();
   
      if (pMessages)
      {
         pItem->Attach(pMessages);
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

STDMETHODIMP 
InterfaceIMAPFolder::get_SubFolders(IInterfaceIMAPFolders **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceIMAPFolders>* pItem = new CComObject<InterfaceIMAPFolders >();
      pItem->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::IMAPFolders> pFolders = object_->GetSubFolders();
   
      if (pFolders)
      {
         pItem->Attach(pFolders);
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

STDMETHODIMP 
InterfaceIMAPFolder::get_Permissions(IInterfaceIMAPFolderPermissions **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!object_->IsPublicFolder())
      {
         // This is not a public  folder. Not possible to modify permissions.
         return COMError::GenerateError("It is only possible to modify permissions for public folders.");
      }
   
      CComObject<InterfaceIMAPFolderPermissions>* pItem = new CComObject<InterfaceIMAPFolderPermissions >();
      pItem->SetAuthentication(authentication_);
   
      if (object_)
      {
         pItem->AttachItem(object_);
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

STDMETHODIMP InterfaceIMAPFolder::Delete()
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      if (!parent_collection_)
         return HM::PersistentIMAPFolder::DeleteObject(object_) ? S_OK : S_FALSE;
      
      parent_collection_->DeleteItemByDBID(object_->GetID());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolder::get_CurrentUID(LONG *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = (long) object_->GetCurrentUID();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolder::get_CreationTime(BSTR *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = HM::Time::GetTimeStampFromDateTime(object_->GetCreationTime()).AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

