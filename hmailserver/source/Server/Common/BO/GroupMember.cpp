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
      m_iGroupID(0),
      m_iAccountID(0)
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
      return m_iGroupID;
   }

   void 
   GroupMember::SetGroupID(__int64 iNewVal)
   {
      m_iGroupID = iNewVal;
   }

   __int64 
   GroupMember::GetAccountID()
   {
      return m_iAccountID;
   }

   void 
   GroupMember::SetAccountID(__int64 iNewVal)
   {
      m_iAccountID = iNewVal; 
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