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
InterfaceIMAPFolders::Attach(shared_ptr<HM::IMAPFolders> pFolders)
{
   m_pObject = pFolders;
}

STDMETHODIMP 
InterfaceIMAPFolders::get_Count(long *pVal)
{
   try
   {
      if (!m_pObject)
         return GetAccessDenied();

      *pVal = m_pObject->GetCount();
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
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceIMAPFolder>* pIMAPFolderInt = new CComObject<InterfaceIMAPFolder>();
      pIMAPFolderInt->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IMAPFolder> pIMAPFolder = m_pObject->GetItem(Index);
   
      if (!pIMAPFolder)
         return DISP_E_BADINDEX;  
   
      pIMAPFolderInt->Attach(pIMAPFolder);
      pIMAPFolderInt->AttachParent(m_pObject, true);
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
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceIMAPFolder>* pIMAPFolderInt = new CComObject<InterfaceIMAPFolder>();
      pIMAPFolderInt->SetAuthentication(m_pAuthentication);
   
      shared_ptr<HM::IMAPFolder> pIMAPFolder = m_pObject->GetItemByDBID(DBID);
   
      if (!pIMAPFolder)
         return DISP_E_BADINDEX;  
   
      pIMAPFolderInt->Attach(pIMAPFolder);
      pIMAPFolderInt->AttachParent(m_pObject, true);
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
      if (!m_pObject)
         return GetAccessDenied();

      CComObject<InterfaceIMAPFolder>* pIMAPFolderInt = new CComObject<InterfaceIMAPFolder>();
      pIMAPFolderInt->SetAuthentication(m_pAuthentication);
   
      HM::String sUnicode (sName);
      HM::AnsiString sFolderName = HM::ModifiedUTF7::Encode(sUnicode);
   
      shared_ptr<HM::IMAPFolder> pIMAPFolder = m_pObject->GetFolderByName(sFolderName);
   
      if (!pIMAPFolder)
         return DISP_E_BADINDEX;
   
      pIMAPFolderInt->Attach(pIMAPFolder);
      pIMAPFolderInt->AttachParent(m_pObject, true);
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
      if (!m_pObject)
         return GetAccessDenied();

      // Check that it does not exist.
      if (!m_pObject->DeleteItemByDBID(lDBID))
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
      if (!m_pObject)
         return GetAccessDenied();

      // Convert the modified UTF7 string into unicode.
      // Since hMailServer core don't know unicode, we
      // have to do it out here in the interface.
      HM::String sUnicode (sName);
      HM::AnsiString sFolderName = HM::ModifiedUTF7::Encode(sUnicode);
   
      CComObject<InterfaceIMAPFolder>* pIMAPFolderInt = new CComObject<InterfaceIMAPFolder>();
      pIMAPFolderInt->SetAuthentication(m_pAuthentication);
   
      // Check that it does not exist.
      shared_ptr<HM::IMAPFolder> pIMAPFolder = m_pObject->GetFolderByName(sFolderName);
   
      if (pIMAPFolder)
         return COMError::GenerateError("Folder with specified name already exists");
   
      pIMAPFolder = shared_ptr<HM::IMAPFolder>(new HM::IMAPFolder(m_pObject->GetAccountID(), m_pObject->GetParentID()));
      pIMAPFolder->SetFolderName(sFolderName);
   
      // We auto-subscribe to public folders.
      if (m_pObject->GetAccountID() == 0)
         pIMAPFolder->SetIsSubscribed(true);
   
      if (!HM::PersistentIMAPFolder::SaveObject(pIMAPFolder))
      {
         return COMError::GenerateError("Folder could not be saved.");
      }
   
      // Add the folder ot the collection.
      m_pObject->AddItem(pIMAPFolder);
   
      pIMAPFolderInt->AttachParent(m_pObject, true);
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

