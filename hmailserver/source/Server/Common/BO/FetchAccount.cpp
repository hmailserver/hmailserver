// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Fetchaccount.h"
#include "FetchAccountUIDs.h"
#include "FetchAccountUID.h"
#include "../Util/Crypt.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{


   FetchAccount::FetchAccount(void) :
      m_iAccountID(0),
      m_iPort(0),
      m_iMinutes(30),
      m_iDaysToKeep(0),
      m_bIsActive(1),
      m_bProcessMIMERecipients(false),
      m_bProcessMIMEDate(false),
      m_iServerType(POP3),
      m_bUseSSL(false),
      _useAntiSpam(false),
      _useAntiVirus(false),
      _enableRouteRecipients(false)
   {
 
   }

   FetchAccount::~FetchAccount(void)
   {

   }

   shared_ptr<FetchAccountUIDs>
   FetchAccount::GetUIDs()
   {
      if (!m_pUIDs)
      {
         m_pUIDs = shared_ptr<FetchAccountUIDs> (new FetchAccountUIDs);
         m_pUIDs->Refresh(m_iID);
      }

      return m_pUIDs;
   }

   bool 
   FetchAccount::XMLStore(XNode *pRuleNode, int iOptions)
   {
      XNode *pNode = pRuleNode->AppendChild(_T("FetchAccount"));

      pNode->AppendAttr(_T("Name"), m_sName);
      pNode->AppendAttr(_T("ServerAddress"), m_sServerAddress);
      pNode->AppendAttr(_T("ServerType"), StringParser::IntToString(m_iServerType));
      pNode->AppendAttr(_T("Port"), StringParser::IntToString(m_iPort));
      pNode->AppendAttr(_T("Username"), m_sUsername);
      pNode->AppendAttr(_T("Password"), Crypt::Instance()->EnCrypt(m_sPassword, Crypt::ETBlowFish));
      pNode->AppendAttr(_T("Minutes"), StringParser::IntToString(m_iMinutes));
      pNode->AppendAttr(_T("DaysToKeep"), StringParser::IntToString(m_iDaysToKeep));
      pNode->AppendAttr(_T("Active"), m_bIsActive ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("ProcessMIMERecipients"), m_bProcessMIMERecipients ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("ProcessMIMEDate"), m_bProcessMIMEDate ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("UseSSL"), m_bUseSSL ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("UseAntiSpam"), _useAntiSpam ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("UseAntiVirus"), _useAntiVirus ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("EnableRouteRecipients"), _enableRouteRecipients ? _T("1") : _T("0"));

      if (!GetUIDs()->XMLStore(pNode, iOptions))
         return false;

      return true;
   }

   bool
   FetchAccount::XMLLoad(XNode *pNode, int iOptions)
   {
      m_sName = pNode->GetAttrValue(_T("Name"));
      m_sServerAddress = pNode->GetAttrValue(_T("ServerAddress"));
      m_iServerType = (ServerType)_ttoi(pNode->GetAttrValue(_T("ServerType")));
      m_iPort =  _ttoi(pNode->GetAttrValue(_T("Port")));
      m_sUsername = pNode->GetAttrValue(_T("Username"));
      m_sPassword = Crypt::Instance()->DeCrypt(pNode->GetAttrValue(_T("Password")), Crypt::ETBlowFish);
      m_iMinutes = _ttoi(pNode->GetAttrValue(_T("Minutes")));
      m_iDaysToKeep = _ttoi(pNode->GetAttrValue(_T("DaysToKeep")));
      m_bIsActive = (pNode->GetAttrValue(_T("Active")) == _T("1"));
      m_bProcessMIMERecipients = (pNode->GetAttrValue(_T("ProcessMIMERecipients")) == _T("1"));
      m_bProcessMIMEDate = (pNode->GetAttrValue(_T("ProcessMIMEDate")) == _T("1"));
      m_bUseSSL = (pNode->GetAttrValue(_T("UseSSL")) == _T("1"));
      _useAntiSpam = (pNode->GetAttrValue(_T("UseAntiSpam")) == _T("1"));
      _useAntiVirus = (pNode->GetAttrValue(_T("UseAntiVirus")) == _T("1"));
      _enableRouteRecipients = (pNode->GetAttrValue(_T("EnableRouteRecipients")) == _T("1"));

      return true;
   }

   bool 
   FetchAccount::XMLLoadSubItems(XNode *pNode, int iRestoreOptions)
   {
      if (!GetUIDs()->XMLLoad(pNode, iRestoreOptions))
         return false;

      return true;
   }
}