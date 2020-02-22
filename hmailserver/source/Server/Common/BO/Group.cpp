// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Group.h"

#include "GroupMembers.h"
#include "Groups.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Group::Group(void)
   {
      
   }

   Group::~Group(void)
   {
   }

   String 
   Group::GetName() const
   {
      return name_;
   }

   void 
   Group::SetName(const String &sName)
   {
      name_ = sName;
   }

   std::shared_ptr<GroupMembers> 
   Group::GetMembers()
   {
      std::shared_ptr<GroupMembers> pGroupMembers = std::shared_ptr<GroupMembers>(new GroupMembers(id_));
      pGroupMembers->Refresh();

      return pGroupMembers;
   }

   bool 
   Group::UserIsMember(__int64 iAccountID)
   {
      return GetMembers()->UserIsMember(iAccountID);
   }

   bool 
   Group::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Group"));

      pNode->AppendAttr(_T("Name"), name_);

      GetMembers()->XMLStore(pNode, iOptions);

      return true;
   }

   bool
   Group::XMLLoad(XNode *pNode, int iRestoreOptions)
   {
      name_ = pNode->GetAttrValue(_T("Name"));

      return true;
   }


   bool
   Group::XMLLoadSubItems(XNode *pNode, int iRestoreOptions)
   {
      std::shared_ptr<GroupMembers> pGroupMembers = GetMembers();
      return pGroupMembers->XMLLoad(pNode, iRestoreOptions);
   }
}