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
      account_id_(0),
      port_(0),
      minutes_(30),
      days_to_keep_(0),
      is_active_(1),
      process_mimerecipients_(false),
      process_mimedate_(false),
      server_type_(POP3),
      use_anti_spam_(false),
      use_anti_virus_(false),
      enable_route_recipients_(false),
      connection_security_(CSNone)      
   {
 
   }

   FetchAccount::~FetchAccount(void)
   {

   }

   std::shared_ptr<FetchAccountUIDs>
   FetchAccount::GetUIDs()
   {
      if (!uids_)
      {
         uids_ = std::shared_ptr<FetchAccountUIDs> (new FetchAccountUIDs);
         uids_->Refresh(id_);
      }

      return uids_;
   }

   bool 
   FetchAccount::XMLStore(XNode *pRuleNode, int iOptions)
   {
      XNode *pNode = pRuleNode->AppendChild(_T("FetchAccount"));

      pNode->AppendAttr(_T("Name"), name_);
      pNode->AppendAttr(_T("ServerAddress"), server_address_);
      pNode->AppendAttr(_T("ServerType"), StringParser::IntToString(server_type_));
      pNode->AppendAttr(_T("Port"), StringParser::IntToString(port_));
      pNode->AppendAttr(_T("Username"), username_);
      pNode->AppendAttr(_T("Password"), Crypt::Instance()->EnCrypt(password_, Crypt::ETBlowFish));
      pNode->AppendAttr(_T("Minutes"), StringParser::IntToString(minutes_));
      pNode->AppendAttr(_T("DaysToKeep"), StringParser::IntToString(days_to_keep_));
      pNode->AppendAttr(_T("Active"), is_active_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("MIMERecipientHeaders"), mime_recipient_headers_);
      pNode->AppendAttr(_T("ProcessMIMERecipients"), process_mimerecipients_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("ProcessMIMEDate"), process_mimedate_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("UseAntiSpam"), use_anti_spam_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("UseAntiVirus"), use_anti_virus_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("EnableRouteRecipients"), enable_route_recipients_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("ConnectionSecurity"), StringParser::IntToString(connection_security_));

      if (!GetUIDs()->XMLStore(pNode, iOptions))
         return false;

      return true;
   }

   bool
   FetchAccount::XMLLoad(XNode *pNode, int iOptions)
   {
      name_ = pNode->GetAttrValue(_T("Name"));
      server_address_ = pNode->GetAttrValue(_T("ServerAddress"));
      server_type_ = (ServerType)_ttoi(pNode->GetAttrValue(_T("ServerType")));
      port_ =  _ttoi(pNode->GetAttrValue(_T("Port")));
      username_ = pNode->GetAttrValue(_T("Username"));
      password_ = Crypt::Instance()->DeCrypt(pNode->GetAttrValue(_T("Password")), Crypt::ETBlowFish);
      minutes_ = _ttoi(pNode->GetAttrValue(_T("Minutes")));
      days_to_keep_ = _ttoi(pNode->GetAttrValue(_T("DaysToKeep")));
      is_active_ = (pNode->GetAttrValue(_T("Active")) == _T("1"));
      mime_recipient_headers_ = pNode->GetAttrValue(_T("MIMERecipientHeaders"));
      process_mimerecipients_ = (pNode->GetAttrValue(_T("ProcessMIMERecipients")) == _T("1"));
      process_mimedate_ = (pNode->GetAttrValue(_T("ProcessMIMEDate")) == _T("1"));
      use_anti_spam_ = (pNode->GetAttrValue(_T("UseAntiSpam")) == _T("1"));
      use_anti_virus_ = (pNode->GetAttrValue(_T("UseAntiVirus")) == _T("1"));
      enable_route_recipients_ = (pNode->GetAttrValue(_T("EnableRouteRecipients")) == _T("1"));

      // Backwards compatibiltiy
      if (pNode->GetAttrValue(_T("UseSSL")) == _T("1"))
      {
         connection_security_ = CSSSL;
      }
      else
      {
         connection_security_ = (ConnectionSecurity) _ttoi(pNode->GetAttrValue(_T("ConnectionSecurity")));
      }

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