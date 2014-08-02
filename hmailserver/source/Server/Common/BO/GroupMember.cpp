// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "GroupMember.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   GroupMember::GroupMember(void) :
      group_id_(0),
      account_id_(0)
   {
      
   }

   GroupMember::~GroupMember(void)
   {
   }

   String
   GroupMember::GetName()
   {
      return "N/A";
   }

   __int64 
   GroupMember::GetGroupID()
   {
      return group_id_;
   }

   void 
   GroupMember::SetGroupID(__int64 iNewVal)
   {
      group_id_ = iNewVal;
   }

   __int64 
   GroupMember::GetAccountID()
   {
      return account_id_;
   }

   void 
   GroupMember::SetAccountID(__int64 iNewVal)
   {
      account_id_ = iNewVal; 
   }


   bool 
   GroupMember::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Member"));

      // Taken care of by parent.
      return true;
   }

   bool 
   GroupMember::XMLLoad(XNode *pNode, int iOptions)
   {
      //

      return true;
   }

}