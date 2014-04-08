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
      m_iAccountID(iAccountID),
      m_sUID(sUID),
      m_sDate(sDate)
   {
      SetID(iID);
   }

   FetchAccountUID::FetchAccountUID() :
      m_iAccountID(0)
   {
      SetID(0);
   }

   FetchAccountUID::~FetchAccountUID()
   {

   }

   DateTime
   FetchAccountUID::GetCreationDate() const
   {
      return Time::GetDateFromSystemDate(m_sDate);
   }

   bool 
   FetchAccountUID::XMLStore(XNode *pUIDsNode, int iOptions)
   {  
      XNode *pUIDNode = pUIDsNode->AppendChild(_T("UID"));
      
      pUIDNode->AppendAttr(_T("UID"), m_sUID);
      pUIDNode->AppendAttr(_T("Date"), m_sDate);

      return true;
   }

   bool
   FetchAccountUID::XMLLoad(XNode *pUIDNode, int iRestoreOptions)
   {
      m_sUID = pUIDNode->GetAttrValue(_T("UID"));
      m_sDate = pUIDNode->GetAttrValue(_T("Date"));

      return true;
   }

   void 
   FetchAccountUID::SetAccountID(__int64 accountID)
   {
      m_iAccountID = accountID;
   }
}