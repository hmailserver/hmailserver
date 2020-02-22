// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "InterfaceIMAPFolders.h"
#include "InterfaceIMAPFolder.h"

#include "../Common/BO/IMAPFolders.h"
#include "../Common/BO/IMAPFolder.h"

#include "COMError.h"

#include "../Common/Util/Encoding/ModifiedUTF7.h"

STDMETHODIMP 
InterfaceIMAPFolders::InterfaceSupportsErrorInfo(REFIID riid)
{
   static const IID* arr[] = 
   {
      &IID_IInterfaceIMAPFolders,
   };

   for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
   {
      if (InlineIsEqualGUID(*arr[i],riid))
         return S_OK;
   }
   return S_FALSE;
}

void
InterfaceIMAPFolders::Attach(std::shared_ptr<HM::IMAPFolders> pFolders)
{
   object_ = pFolders;
}

STDMETHODIMP 
InterfaceIMAPFolders::get_Count(long *pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      *pVal = object_->GetCount();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolders::get_Item(long Index, IInterfaceIMAPFolder **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceIMAPFolder>* pIMAPFolderInt = new CComObject<InterfaceIMAPFolder>();
      pIMAPFolderInt->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::IMAPFolder> pIMAPFolder = object_->GetItem(Index);
   
      if (!pIMAPFolder)
         return DISP_E_BADINDEX;  
   
      pIMAPFolderInt->Attach(pIMAPFolder);
      pIMAPFolderInt->AttachParent(object_, true);
      pIMAPFolderInt->AddRef();
      *pVal = pIMAPFolderInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolders::get_ItemByDBID(long DBID, IInterfaceIMAPFolder **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceIMAPFolder>* pIMAPFolderInt = new CComObject<InterfaceIMAPFolder>();
      pIMAPFolderInt->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::IMAPFolder> pIMAPFolder = object_->GetItemByDBID(DBID);
   
      if (!pIMAPFolder)
         return DISP_E_BADINDEX;  
   
      pIMAPFolderInt->Attach(pIMAPFolder);
      pIMAPFolderInt->AttachParent(object_, true);
      pIMAPFolderInt->AddRef();
      *pVal = pIMAPFolderInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolders::get_ItemByName(BSTR sName, IInterfaceIMAPFolder **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      CComObject<InterfaceIMAPFolder>* pIMAPFolderInt = new CComObject<InterfaceIMAPFolder>();
      pIMAPFolderInt->SetAuthentication(authentication_);
   
      HM::String sUnicode (sName);
      HM::AnsiString sFolderName = HM::ModifiedUTF7::Encode(sUnicode);
   
      std::shared_ptr<HM::IMAPFolder> pIMAPFolder = object_->GetFolderByName(sFolderName);
   
      if (!pIMAPFolder)
         return DISP_E_BADINDEX;
   
      pIMAPFolderInt->Attach(pIMAPFolder);
      pIMAPFolderInt->AttachParent(object_, true);
      pIMAPFolderInt->AddRef();
      *pVal = pIMAPFolderInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolders::DeleteByDBID(long lDBID)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      // Check that it does not exist.
      if (!object_->DeleteItemByDBID(lDBID))
         return DISP_E_BADINDEX;
      
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceIMAPFolders::Add(BSTR sName, IInterfaceIMAPFolder **pVal)
{
   try
   {
      if (!object_)
         return GetAccessDenied();

      // Convert the modified UTF7 string into unicode.
      // Since hMailServer core don't know unicode, we
      // have to do it out here in the interface.
      HM::String sUnicode (sName);
      HM::AnsiString sFolderName = HM::ModifiedUTF7::Encode(sUnicode);
   
      CComObject<InterfaceIMAPFolder>* pIMAPFolderInt = new CComObject<InterfaceIMAPFolder>();
      pIMAPFolderInt->SetAuthentication(authentication_);
   
      // Check that it does not exist.
      std::shared_ptr<HM::IMAPFolder> pIMAPFolder = object_->GetFolderByName(sFolderName);
   
      if (pIMAPFolder)
         return COMError::GenerateError("Folder with specified name already exists");
   
      pIMAPFolder = std::shared_ptr<HM::IMAPFolder>(new HM::IMAPFolder(object_->GetAccountID(), object_->GetParentID()));
      pIMAPFolder->SetFolderName(sFolderName);
   
      // We auto-subscribe to public folders.
      if (object_->GetAccountID() == 0)
         pIMAPFolder->SetIsSubscribed(true);
   
      if (!HM::PersistentIMAPFolder::SaveObject(pIMAPFolder))
      {
         return COMError::GenerateError("Folder could not be saved.");
      }
   
      // Add the folder ot the collection.
      object_->AddItem(pIMAPFolder);
   
      pIMAPFolderInt->AttachParent(object_, true);
      pIMAPFolderInt->Attach(pIMAPFolder);
      pIMAPFolderInt->AddRef();
   
      *pVal = pIMAPFolderInt;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

