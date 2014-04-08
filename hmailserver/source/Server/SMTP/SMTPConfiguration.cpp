// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "SMTPConfiguration.h"
#include "SMTPDeliveryManager.h"

#include "../Common/Application/Property.h"

#include "../Common/BO/Routes.h"
#include "../Common/BO/DNSBlackLists.h"
#include "../Common/BO/SURBLServers.h"
#include "../Common/BO/BlockedAttachments.h"

#include "../Common/Util/BlowFish.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SMTPConfiguration::SMTPConfiguration()
   { 
   
   }

   SMTPConfiguration::~SMTPConfiguration()
   {
   
   }

   bool 
   SMTPConfiguration::Load()
   {
      _incomingRelays = shared_ptr<IncomingRelays>(new IncomingRelays());
      if (!_incomingRelays->Refresh())
         return false;

      _routes = shared_ptr<Routes> (new Routes());
      _routes->Refresh();

      return true;
   }

   shared_ptr<PropertySet>
   SMTPConfiguration::_GetSettings() const
   {
      return Configuration::Instance()->GetSettings();
   }

   void
   SMTPConfiguration::SetMaxSMTPConnections(int newVal)
   {
      _GetSettings()->SetLong(PROPERTY_MAXSMTPCONNECTIONS, newVal);
   }

   int
   SMTPConfiguration::GetMaxSMTPConnections()
   {
      return _GetSettings()->GetLong(PROPERTY_MAXSMTPCONNECTIONS);
   }

   void
   SMTPConfiguration::SetAuthAllowPlainText(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_AUTHALLOWPLAINTEXT, newVal);
   }

   bool 
   SMTPConfiguration::GetAuthAllowPlainText()
   {
      return _GetSettings()->GetBool(PROPERTY_AUTHALLOWPLAINTEXT);
   }

   void
   SMTPConfiguration::SetAllowMailFromNull(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_ALLOWMAILFROMNULL, newVal);
   }

   
   bool 
   SMTPConfiguration::GetAllowMailFromNull()
   {
      return _GetSettings()->GetBool(PROPERTY_ALLOWMAILFROMNULL);
   }

   void
   SMTPConfiguration::SetNoOfRetries(long lNoOfRetries)
   {
      _GetSettings()->SetLong(PROPERTY_SMTPNOOFTRIES, lNoOfRetries);
   }

   long
   SMTPConfiguration::GetNoOfRetries()
   {
      return _GetSettings()->GetLong(PROPERTY_SMTPNOOFTRIES);
   }


   void 
   SMTPConfiguration::SetMinutesBetweenTry(long lMinutesBetween)
   {
      _GetSettings()->SetLong(PROPERTY_SMTPMINUTESBETWEEN, lMinutesBetween);
   }

   long
   SMTPConfiguration::GetMinutesBetweenTry()
   {
      return _GetSettings()->GetLong(PROPERTY_SMTPMINUTESBETWEEN);
   }


   String 
   SMTPConfiguration::GetWelcomeMessage() const
   {
      return _GetSettings()->GetString(PROPERTY_WELCOMESMTP);
   }

   void 
   SMTPConfiguration::SetWelcomeMessage(const String &sMessage)
   {
      _GetSettings()->SetString(PROPERTY_WELCOMESMTP, sMessage);
   }

   String 
   SMTPConfiguration::GetSMTPDeliveryBindToIP() const
   {
      return _GetSettings()->GetString(PROPERTY_SMTPDELIVERYBINDTOIP);
   }

   void 
   SMTPConfiguration::SetSMTPDeliveryBindToIP(const String &sMessage)
   {
      _GetSettings()->SetString(PROPERTY_SMTPDELIVERYBINDTOIP, sMessage);
   }


   String 
   SMTPConfiguration::GetSMTPRelayer() const
   {
      return _GetSettings()->GetString(PROPERTY_SMTPRELAYER);
   }

   void 
   SMTPConfiguration::SetSMTPRelayer(const String &sRelayer)
   {
      _GetSettings()->SetString(PROPERTY_SMTPRELAYER, sRelayer);
   }

   void 
   SMTPConfiguration::SetSMTPRelayerPort(long lPortID)
   {
      _GetSettings()->SetLong(PROPERTY_SMTPRELAYER_PORT, lPortID);
   }

   long
   SMTPConfiguration::GetSMTPRelayerPort()
   {
      return _GetSettings()->GetLong(PROPERTY_SMTPRELAYER_PORT);
   }

   void 
   SMTPConfiguration::SetSMTPRelayerUseSSL(bool bNewwValue)
   {
      _GetSettings()->SetBool(PROPERTY_SMTPRELAYERUSESSL, bNewwValue);
   }

   bool
   SMTPConfiguration::GetSMTPRelayerUseSSL()
   {
      return _GetSettings()->GetBool(PROPERTY_SMTPRELAYERUSESSL);
   }

   int
   SMTPConfiguration::GetMaxNoOfDeliveryThreads()
   {
      return _GetSettings()->GetLong(PROPERTY_MAXDELIVERYTHREADS);
   }

   void 
   SMTPConfiguration::SetMaxNoOfDeliveryThreads(int lNewValue)
   {
      _GetSettings()->SetLong(PROPERTY_MAXDELIVERYTHREADS, lNewValue);
   }
   
   int
   SMTPConfiguration::GetMaxMessageSize()
   {
      return _GetSettings()->GetLong(PROPERTY_MAXMESSAGESIZE);
   }

   void 
   SMTPConfiguration::SetMaxMessageSize(int lNewValue)
   {
      _GetSettings()->SetLong(PROPERTY_MAXMESSAGESIZE, lNewValue);
   }

   int
   SMTPConfiguration::GetMaxSMTPRecipientsInBatch()
   {
      return _GetSettings()->GetLong(PROPERTY_MAXSMTPRECIPIENTSINBATCH);
   }

   void 
   SMTPConfiguration::SetMaxSMTPRecipientsInBatch(int lNewValue)
   {
      _GetSettings()->SetLong(PROPERTY_MAXSMTPRECIPIENTSINBATCH, lNewValue);
   }


   int
   SMTPConfiguration::GetRuleLoopLimit()
   {
      return _GetSettings()->GetLong(PROPERTY_RULELOOPLIMIT);
   }

   void 
   SMTPConfiguration::SetRuleLoopLimit(int lNewValue)
   {
      _GetSettings()->SetLong(PROPERTY_RULELOOPLIMIT, lNewValue);
   }


   int
   SMTPConfiguration::GetMaxNumberOfMXHosts()
   {
      return _GetSettings()->GetLong(PROPERTY_MAX_NUMBER_OF_MXHOSTS);
   }

   void 
   SMTPConfiguration::SetMaxNumberOfMXHosts(int lNewValue)
   {
      _GetSettings()->SetLong(PROPERTY_MAX_NUMBER_OF_MXHOSTS, lNewValue);
   }

   bool
   SMTPConfiguration::GetSMTPRelayerRequiresAuthentication()
   {
      return _GetSettings()->GetBool(PROPERTY_SMTPRELAYER_USEAUTH);
   }

   void
   SMTPConfiguration::SetSMTPRelayerRequiresAuthentication(bool bNewVal)
   {
      _GetSettings()->SetBool(PROPERTY_SMTPRELAYER_USEAUTH, bNewVal);
   }

   void 
   SMTPConfiguration::SetSMTPRelayerUsername(const String & sValue)
   {
      _GetSettings()->SetString(PROPERTY_SMTPRELAYER_USERNAME, sValue);
   }

   String
   SMTPConfiguration::GetSMTPRelayerUsername() const
   {
      return _GetSettings()->GetString(PROPERTY_SMTPRELAYER_USERNAME);
   }

   void 
   SMTPConfiguration::SetSMTPRelayerPassword(const String & sValue)
   {
      _GetSettings()->SetString(PROPERTY_SMTPRELAYER_PASSWORD, sValue);
   }

   String
   SMTPConfiguration::GetSMTPRelayerPassword() const
   {
      return _GetSettings()->GetString(PROPERTY_SMTPRELAYER_PASSWORD);
   }




   void
   SMTPConfiguration::SetAllowIncorrectLineEndings(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_ALLOWINCORRECTLINEENDINGS, newVal);
   }

   bool 
   SMTPConfiguration::GetAllowIncorrectLineEndings() 
   {
      return _GetSettings()->GetBool(PROPERTY_ALLOWINCORRECTLINEENDINGS);
   }

   void
   SMTPConfiguration::SetAddDeliveredToHeader(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_ADDDELIVEREDTOHEADER, newVal);
   }

   bool 
   SMTPConfiguration::GetAddDeliveredToHeader() 
   {
      return _GetSettings()->GetBool(PROPERTY_ADDDELIVEREDTOHEADER);
   }

   void 
   SMTPConfiguration::OnPropertyChanged(shared_ptr<Property> pProperty)
   {
      String sPropertyName = pProperty->GetName();

      if (sPropertyName == PROPERTY_MAXDELIVERYTHREADS)
      {
         shared_ptr<SMTPDeliveryManager> pDeliveryManager = Application::Instance()->GetSMTPDeliveryManager();
         if (!pDeliveryManager)
            return;

         pDeliveryManager->OnPropertyChanged(pProperty);
      }
   }

   bool 
   SMTPConfiguration::XMLStore(XNode *pBackupNode, int iOptions)
   {
      if (!_routes->XMLStore(pBackupNode, iOptions))
         return false;

      if (!_incomingRelays->XMLStore(pBackupNode, iOptions))
         return false;

      return true;
   }

   bool
   SMTPConfiguration::XMLLoad(XNode *pBackupNode, int iRestoreOptions)
   {
      _routes->Refresh();
      _routes->XMLLoad(pBackupNode, iRestoreOptions);
 
      _incomingRelays->Refresh();
      _incomingRelays->XMLLoad(pBackupNode, iRestoreOptions);

      return true;
   }


}
