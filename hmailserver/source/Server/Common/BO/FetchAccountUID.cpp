// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\FetchAccountUID.h"

#include "..\Util\Time.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   FetchAccountUID::FetchAccountUID (__int64 iID, __int64 iAccountID, const String &sUID, const String &sDate) :
      account_id_(iAccountID),
      uid_(sUID),
      date_(sDate)
   {
      SetID(iID);
   }

   FetchAccountUID::FetchAccountUID() :
      account_id_(0)
   {
      SetID(0);
   }

   FetchAccountUID::~FetchAccountUID()
   {

   }

   DateTime
   FetchAccountUID::GetCreationDate() const
   {
      return Time::GetDateFromSystemDate(date_);
   }

   bool 
   FetchAccountUID::XMLStore(XNode *pUIDsNode, int iOptions)
   {  
      XNode *pUIDNode = pUIDsNode->AppendChild(_T("UID"));
      
      pUIDNode->AppendAttr(_T("UID"), uid_);
      pUIDNode->AppendAttr(_T("Date"), date_);

      return true;
   }

   bool
   FetchAccountUID::XMLLoad(XNode *pUIDNode, int iRestoreOptions)
   {
      uid_ = pUIDNode->GetAttrValue(_T("UID"));
      date_ = pUIDNode->GetAttrValue(_T("Date"));

      return true;
   }

   void 
   FetchAccountUID::SetAccountID(__int64 accountID)
   {
      account_id_ = accountID;
   }
}