// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceGroups.h"

#include "..\Common\BO\Group.h"
#include "..\Common\BO\Groups.h"

#include "InterfaceGroup.h"

void 
InterfaceGroups::Attach(std::shared_ptr<HM::Groups> pBA) 
{ 
   groups_ = pBA; 
}

STDMETHODIMP 
InterfaceGroups::Refresh()
{
   try
   {
      if (!groups_)
         return GetAccessDenied();

      if (!groups_)
         return S_FALSE;
   
      groups_->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceGroups::get_Count(long *pVal)
{
   try
   {
      if (!groups_)
         return GetAccessDenied();

      *pVal = groups_->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGroups::get_Item(long Index, IInterfaceGroup **pVal)
{
   try
   {
      if (!groups_)
         return GetAccessDenied();

      CComObject<InterfaceGroup>* pInterfaceGroup = new CComObject<InterfaceGroup>();
      pInterfaceGroup->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Group> pBA = groups_->GetItem(Index);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceGroup->AttachItem(pBA);
      pInterfaceGroup->AttachParent(groups_, true);
      pInterfaceGroup->AddRef();
      *pVal = pInterfaceGroup;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGroups::DeleteByDBID(long DBID)
{
   try
   {
      if (!groups_)
         return GetAccessDenied();

      groups_->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGroups::get_ItemByDBID(long lDBID, IInterfaceGroup **pVal)
{
   try
   {
      if (!groups_)
         return GetAccessDenied();

      CComObject<InterfaceGroup>* pInterfaceGroup = new CComObject<InterfaceGroup>();
      pInterfaceGroup->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Group> pBA = groups_->GetItemByDBID(lDBID);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceGroup->AttachItem(pBA);
      pInterfaceGroup->AttachParent(groups_, true);
      pInterfaceGroup->AddRef();
   
      *pVal = pInterfaceGroup;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGroups::get_ItemByName(BSTR sName, IInterfaceGroup **pVal)
{
   try
   {
      if (!groups_)
         return GetAccessDenied();

      CComObject<InterfaceGroup>* pInterfaceGroup = new CComObject<InterfaceGroup>();
      pInterfaceGroup->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Group> pBA = groups_->GetItemByName(sName);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceGroup->AttachItem(pBA);
      pInterfaceGroup->AttachParent(groups_, true);
      pInterfaceGroup->AddRef();
   
      *pVal = pInterfaceGroup;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGroups::Add(IInterfaceGroup **pVal)
{
   try
   {
      if (!groups_)
         return GetAccessDenied();

      if (!groups_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceGroup>* pInterfaceGroup = new CComObject<InterfaceGroup>();
      pInterfaceGroup->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::Group> pBA = std::shared_ptr<HM::Group>(new HM::Group);
   
      pInterfaceGroup->AttachItem(pBA);
      pInterfaceGroup->AttachParent(groups_, false);
   
      pInterfaceGroup->AddRef();
   
      *pVal = pInterfaceGroup;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


