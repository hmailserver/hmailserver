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
      if (!m_pObject)
         return GetAccessDenied();

      if (HM::PersistentIMAPFolder::SaveObject(m_pObject))
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
InterfaceIMAPFolder::Attach(shared_ptr<HM::IMAPFolder> pFolder)
{
   m_pObject = pFolder;
}

STDMETHODIMP 
InterfaceIMAPFolder::get_ParentID(LONG *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = (long) m_pObject->GetParentFolderID();
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
      if (!m_pObject)
         return GetAccessDenied();

      HM::String sUnicode = HM::ModifiedUTF7::Decode(m_pObject->GetFolderName());
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
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetFolderName(HM::ModifiedUTF7::Encode(newVal));
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
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetIsSubscribed() ? VARIANT_TRUE : VARIANT_FALSE;
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
      if (!m_pObject)
         return GetAccessDenied();

      m_pObject->SetIsSubscribed(newVal == VARIANT_TRUE);
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
      if (!m_pObject)
         return GetAccessDenied();

   
      CComObject<InterfaceMessages>* pItem = new CComObject<InterfaceMessages>();
      pItem->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::Messages> pMessages = m_pObject->GetMessages();
   
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
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceIMAPFolders>* pItem = new CComObject<InterfaceIMAPFolders >();
      pItem->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IMAPFolders> pFolders = m_pObject->GetSubFolders();
   
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
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pObject->IsPublicFolder())
      {
         // This is not a public  folder. Not possible to modify permissions.
         return COMError::GenerateError("It is only possible to modify permissions for public folders.");
      }
   
      CComObject<InterfaceIMAPFolderPermissions>* pItem = new CComObject<InterfaceIMAPFolderPermissions >();
      pItem->SetAuthentication(m_pAuthentication);
   
      if (m_pObject)
      {
         pItem->AttachItem(m_pObject);
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
      if (!m_pObject)
         return GetAccessDenied();

      if (!m_pParentCollection)
         return HM::PersistentIMAPFolder::DeleteObject(m_pObject) ? S_OK : S_FALSE;
      
      m_pParentCollection->DeleteItemByDBID(m_pObject->GetID());
   
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
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = (long) m_pObject->GetCurrentUID();
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
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = HM::Time::GetTimeStampFromDateTime(m_pObject->GetCreationTime()).AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

