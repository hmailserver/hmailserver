// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceGroupMembers.h"

#include "..\Common\BO\GroupMember.h"
#include "..\Common\BO\GroupMembers.h"

#include "InterfaceGroupMember.h"

void 
InterfaceGroupMembers::Attach(std::shared_ptr<HM::GroupMembers> pBA) 
{ 
   group_members_ = pBA; 
}

STDMETHODIMP 
InterfaceGroupMembers::Refresh()
{
   try
   {
      if (!group_members_)
         return GetAccessDenied();

      if (!group_members_)
         return S_FALSE;
   
      group_members_->Refresh();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceGroupMembers::get_Count(long *pVal)
{
   try
   {
      if (!group_members_)
         return GetAccessDenied();

      *pVal = group_members_->GetCount();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGroupMembers::get_Item(long Index, IInterfaceGroupMember **pVal)
{
   try
   {
      if (!group_members_)
         return GetAccessDenied();

      CComObject<InterfaceGroupMember>* pInterfaceGroupMember = new CComObject<InterfaceGroupMember>();
      pInterfaceGroupMember->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::GroupMember> pBA = group_members_->GetItem(Index);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceGroupMember->AttachItem(pBA);
      pInterfaceGroupMember->AttachParent(group_members_, true);
      pInterfaceGroupMember->AddRef();
      *pVal = pInterfaceGroupMember;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGroupMembers::DeleteByDBID(long DBID)
{
   try
   {
      if (!group_members_)
         return GetAccessDenied();

      group_members_->DeleteItemByDBID(DBID);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGroupMembers::get_ItemByDBID(long lDBID, IInterfaceGroupMember **pVal)
{
   try
   {
      if (!group_members_)
         return GetAccessDenied();

      CComObject<InterfaceGroupMember>* pInterfaceGroupMember = new CComObject<InterfaceGroupMember>();
      pInterfaceGroupMember->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::GroupMember> pBA = group_members_->GetItemByDBID(lDBID);
   
      if (!pBA)
         return DISP_E_BADINDEX;
   
      pInterfaceGroupMember->AttachItem(pBA);
      pInterfaceGroupMember->AttachParent(group_members_, true);
      pInterfaceGroupMember->AddRef();
   
      *pVal = pInterfaceGroupMember;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceGroupMembers::Add(IInterfaceGroupMember **pVal)
{
   try
   {
      if (!group_members_)
         return GetAccessDenied();

      if (!group_members_)
         return authentication_->GetAccessDenied();
   
      CComObject<InterfaceGroupMember>* pInterfaceGroupMember = new CComObject<InterfaceGroupMember>();
      pInterfaceGroupMember->SetAuthentication(authentication_);
   
      std::shared_ptr<HM::GroupMember> pBA = std::shared_ptr<HM::GroupMember>(new HM::GroupMember);
      pBA->SetGroupID(group_members_->GetGroupID());
   
      pInterfaceGroupMember->AttachItem(pBA);
      pInterfaceGroupMember->AttachParent(group_members_, false);
   
      pInterfaceGroupMember->AddRef();
   
      *pVal = pInterfaceGroupMember;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


