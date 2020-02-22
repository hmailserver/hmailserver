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
      connection_security_(CSNone),
      to_all_addresses_(true),
      relayer_requires_authentication_(false),
      treat_recipient_as_local_domain_(false),
      treat_sender_as_local_domain_(false),
      target_smtpport_(0),
      number_of_tries_(0),
      minutes_between_try_(0)
   {
      
   }

   Route::~Route()
   {

   }

   std::shared_ptr<RouteAddresses>
   Route::GetAddresses()
   {
       if (!addresses_)
       {
          assert(id_);
          addresses_ = std::shared_ptr<RouteAddresses>(new RouteAddresses(id_));

          addresses_->Refresh();
       }

       return addresses_;
   }

   bool 
   Route::XMLStore(XNode *pRoutesNode, int iOptions)
   {
      XNode *pNode = pRoutesNode->AppendChild(_T("Route"));

      pNode->AppendAttr(_T("Name"), domain_name_);
      pNode->AppendAttr(_T("Description"), description_);
      pNode->AppendAttr(_T("TargetHost"), target_smtphost_);
      pNode->AppendAttr(_T("TargetPort"), StringParser::IntToString(target_smtpport_));
      pNode->AppendAttr(_T("NumberOfTries"), StringParser::IntToString(number_of_tries_));
      pNode->AppendAttr(_T("MinutesBetweenTry"), StringParser::IntToString(minutes_between_try_));
      pNode->AppendAttr(_T("ToAllAddresses"), to_all_addresses_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("RequiresAuthentication"), relayer_requires_authentication_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("Username"), relayer_auth_username_);
      pNode->AppendAttr(_T("Password"), Crypt::Instance()->EnCrypt(relayer_auth_password_, Crypt::ETBlowFish));
      pNode->AppendAttr(_T("TreatRecipientAsLocalDomain"), treat_recipient_as_local_domain_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("TreatSenderAsLocalDomain"), treat_sender_as_local_domain_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("ConnectionSecurity"), StringParser::IntToString(connection_security_));

      return GetAddresses()->XMLStore(pNode, iOptions);

   }
   
   bool 
   Route::XMLLoad(XNode *pNode, int iOptions)
   {
      domain_name_ = pNode->GetAttrValue(_T("Name"));
      description_ = pNode->GetAttrValue(_T("Description"));
      target_smtphost_ = pNode->GetAttrValue(_T("TargetHost"));
      target_smtpport_ = _ttoi(pNode->GetAttrValue(_T("TargetPort")));
      number_of_tries_ = _ttoi(pNode->GetAttrValue(_T("NumberOfTries")));
      minutes_between_try_ = _ttoi(pNode->GetAttrValue(_T("MinutesBetweenTry")));
      to_all_addresses_ = pNode->GetAttrValue(_T("ToAllAddresses")) == _T("1");
      relayer_requires_authentication_ = pNode->GetAttrValue(_T("RequiresAuthentication")) == _T("1");
      relayer_auth_username_ = pNode->GetAttrValue(_T("Username"));
      relayer_auth_password_ = Crypt::Instance()->DeCrypt(pNode->GetAttrValue(_T("Password")), Crypt::ETBlowFish);
      treat_recipient_as_local_domain_ = pNode->GetAttrValue(_T("TreatRecipientAsLocalDomain")) == _T("1");
      treat_sender_as_local_domain_ = pNode->GetAttrValue(_T("TreatSenderAsLocalDomain")) == _T("1");

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
   Route::XMLLoadSubItems(XNode *pNode, int iOptions)
   {
      return GetAddresses()->XMLLoad(pNode, iOptions);
   }
}

