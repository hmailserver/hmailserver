// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Route.h"

#include "RouteAddresses.h"
#include "../Util/Crypt.h"
#include "../Persistence/PersistentRouteAddress.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Route::Route() : 
      m_bUseSSL(false),
      m_bToAllAddresses(true),
      m_bRelayerRequiresAuthentication(false),
      m_bTreatRecipientAsLocalDomain(false),
      m_bTreatSenderAsLocalDomain(false),
      m_lTargetSMTPPort(0),
      m_lNumberOfTries(0),
      m_lMinutesBetweenTry(0)
   {
      
   }

   Route::~Route()
   {

   }

   shared_ptr<RouteAddresses>
   Route::GetAddresses()
   {
       if (!m_pAddresses)
       {
          assert(m_iID);
          m_pAddresses = shared_ptr<RouteAddresses>(new RouteAddresses(m_iID));

          m_pAddresses->Refresh();
       }

       return m_pAddresses;
   }

   bool 
   Route::XMLStore(XNode *pRoutesNode, int iOptions)
   {
      XNode *pNode = pRoutesNode->AppendChild(_T("Route"));

      pNode->AppendAttr(_T("Name"), m_sDomainName);
      pNode->AppendAttr(_T("Description"), m_sDescription);
      pNode->AppendAttr(_T("TargetHost"), m_sTargetSMTPHost);
      pNode->AppendAttr(_T("TargetPort"), StringParser::IntToString(m_lTargetSMTPPort));
      pNode->AppendAttr(_T("NumberOfTries"), StringParser::IntToString(m_lNumberOfTries));
      pNode->AppendAttr(_T("MinutesBetweenTry"), StringParser::IntToString(m_lMinutesBetweenTry));
      pNode->AppendAttr(_T("ToAllAddresses"), m_bToAllAddresses ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("RequiresAuthentication"), m_bRelayerRequiresAuthentication ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("Username"), m_sRelayerAuthUsername);
      pNode->AppendAttr(_T("Password"), Crypt::Instance()->EnCrypt(m_sRelayerAuthPassword, Crypt::ETBlowFish));
      pNode->AppendAttr(_T("TreatRecipientAsLocalDomain"), m_bTreatRecipientAsLocalDomain ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("TreatSenderAsLocalDomain"), m_bTreatSenderAsLocalDomain ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("UseSSL"), m_bUseSSL ? _T("1") : _T("0"));

      return GetAddresses()->XMLStore(pNode, iOptions);

   }
   
   bool 
   Route::XMLLoad(XNode *pNode, int iOptions)
   {
      m_sDomainName = pNode->GetAttrValue(_T("Name"));
      m_sDescription = pNode->GetAttrValue(_T("Description"));
      m_sTargetSMTPHost = pNode->GetAttrValue(_T("TargetHost"));
      m_lTargetSMTPPort = _ttoi(pNode->GetAttrValue(_T("TargetPort")));
      m_lNumberOfTries = _ttoi(pNode->GetAttrValue(_T("NumberOfTries")));
      m_lMinutesBetweenTry = _ttoi(pNode->GetAttrValue(_T("MinutesBetweenTry")));
      m_bToAllAddresses = pNode->GetAttrValue(_T("ToAllAddresses")) == _T("1");
      m_bRelayerRequiresAuthentication = pNode->GetAttrValue(_T("RequiresAuthentication")) == _T("1");
      m_sRelayerAuthUsername = pNode->GetAttrValue(_T("Username"));
      m_sRelayerAuthPassword = Crypt::Instance()->DeCrypt(pNode->GetAttrValue(_T("Password")), Crypt::ETBlowFish);
      m_bTreatRecipientAsLocalDomain = pNode->GetAttrValue(_T("TreatRecipientAsLocalDomain")) == _T("1");
      m_bTreatSenderAsLocalDomain = pNode->GetAttrValue(_T("TreatSenderAsLocalDomain")) == _T("1");
      m_bUseSSL = pNode->GetAttrValue(_T("UseSSL")) == _T("1");

      return true;
   }

   bool 
   Route::XMLLoadSubItems(XNode *pNode, int iOptions)
   {
      return GetAddresses()->XMLLoad(pNode, iOptions);
   }
}

