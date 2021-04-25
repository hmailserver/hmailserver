// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "SMTPConfiguration.h"
#include "SMTPDeliveryManager.h"

#include "../Common/Application/Property.h"

#include "../Common/BO/Routes.h"

#include "../Common/TCPIP/SocketConstants.h"

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
      incoming_relays_ = std::shared_ptr<IncomingRelays>(new IncomingRelays());
      if (!incoming_relays_->Refresh())
         return false;

      routes_ = std::shared_ptr<Routes> (new Routes());
      routes_->Refresh();

      return true;
   }

   std::shared_ptr<PropertySet>
   SMTPConfiguration::GetSettings_() const
   {
      return Configuration::Instance()->GetSettings();
   }

   void
   SMTPConfiguration::SetMaxSMTPConnections(int newVal)
   {
      GetSettings_()->SetLong(PROPERTY_MAXSMTPCONNECTIONS, newVal);
   }

   int
   SMTPConfiguration::GetMaxSMTPConnections()
   {
      return GetSettings_()->GetLong(PROPERTY_MAXSMTPCONNECTIONS);
   }

   void
   SMTPConfiguration::SetAuthAllowPlainText(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_AUTHALLOWPLAINTEXT, newVal);
   }

   bool 
   SMTPConfiguration::GetAuthAllowPlainText()
   {
      return GetSettings_()->GetBool(PROPERTY_AUTHALLOWPLAINTEXT);
   }

   void
   SMTPConfiguration::SetAllowMailFromNull(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_ALLOWMAILFROMNULL, newVal);
   }

   
   bool 
   SMTPConfiguration::GetAllowMailFromNull()
   {
      return GetSettings_()->GetBool(PROPERTY_ALLOWMAILFROMNULL);
   }

   void
   SMTPConfiguration::SetNoOfRetries(long lNoOfRetries)
   {
      GetSettings_()->SetLong(PROPERTY_SMTPNOOFTRIES, lNoOfRetries);
   }

   long
   SMTPConfiguration::GetNoOfRetries()
   {
      return GetSettings_()->GetLong(PROPERTY_SMTPNOOFTRIES);
   }


   void 
   SMTPConfiguration::SetMinutesBetweenTry(long lMinutesBetween)
   {
      GetSettings_()->SetLong(PROPERTY_SMTPMINUTESBETWEEN, lMinutesBetween);
   }

   long
   SMTPConfiguration::GetMinutesBetweenTry()
   {
      return GetSettings_()->GetLong(PROPERTY_SMTPMINUTESBETWEEN);
   }


   String 
   SMTPConfiguration::GetWelcomeMessage() const
   {
      return GetSettings_()->GetString(PROPERTY_WELCOMESMTP);
   }

   void 
   SMTPConfiguration::SetWelcomeMessage(const String &sMessage)
   {
      GetSettings_()->SetString(PROPERTY_WELCOMESMTP, sMessage);
   }

   String 
   SMTPConfiguration::GetSMTPDeliveryBindToIP() const
   {
      return GetSettings_()->GetString(PROPERTY_SMTPDELIVERYBINDTOIP);
   }

   void 
   SMTPConfiguration::SetSMTPDeliveryBindToIP(const String &sMessage)
   {
      GetSettings_()->SetString(PROPERTY_SMTPDELIVERYBINDTOIP, sMessage);
   }


   String 
   SMTPConfiguration::GetSMTPRelayer() const
   {
      return GetSettings_()->GetString(PROPERTY_SMTPRELAYER);
   }

   void 
   SMTPConfiguration::SetSMTPRelayer(const String &sRelayer)
   {
      GetSettings_()->SetString(PROPERTY_SMTPRELAYER, sRelayer);
   }

   void 
   SMTPConfiguration::SetSMTPRelayerPort(long lPortID)
   {
      GetSettings_()->SetLong(PROPERTY_SMTPRELAYER_PORT, lPortID);
   }

   long
   SMTPConfiguration::GetSMTPRelayerPort()
   {
      return GetSettings_()->GetLong(PROPERTY_SMTPRELAYER_PORT);
   }

   void 
   SMTPConfiguration::SetSMTPRelayerConnectionSecurity(ConnectionSecurity connection_security)
   {
      GetSettings_()->SetLong(PROPERTY_SMTPRELAYERCONNECTIONSECURITY, connection_security);
   }

   ConnectionSecurity
   SMTPConfiguration::GetSMTPRelayerConnectionSecurity()
   {
      return (ConnectionSecurity) GetSettings_()->GetLong(PROPERTY_SMTPRELAYERCONNECTIONSECURITY);
   }

   void 
   SMTPConfiguration::SetSMTPConnectionSecurity(ConnectionSecurity connection_security)
   {
      GetSettings_()->SetLong(PROPERTY_SMTPCONNECTIONSECURITY, connection_security);
   }

   ConnectionSecurity
   SMTPConfiguration::GetSMTPConnectionSecurity()
   {
      return (ConnectionSecurity) GetSettings_()->GetLong(PROPERTY_SMTPCONNECTIONSECURITY);
   }

   int
   SMTPConfiguration::GetMaxNoOfDeliveryThreads()
   {
      return GetSettings_()->GetLong(PROPERTY_MAXDELIVERYTHREADS);
   }

   void 
   SMTPConfiguration::SetMaxNoOfDeliveryThreads(int lNewValue)
   {
      GetSettings_()->SetLong(PROPERTY_MAXDELIVERYTHREADS, lNewValue);
   }
   
   int
   SMTPConfiguration::GetMaxMessageSize()
   {
      return GetSettings_()->GetLong(PROPERTY_MAXMESSAGESIZE);
   }

   void 
   SMTPConfiguration::SetMaxMessageSize(int lNewValue)
   {
      GetSettings_()->SetLong(PROPERTY_MAXMESSAGESIZE, lNewValue);
   }

   int
   SMTPConfiguration::GetMaxSMTPRecipientsInBatch()
   {
      return GetSettings_()->GetLong(PROPERTY_MAXSMTPRECIPIENTSINBATCH);
   }

   void 
   SMTPConfiguration::SetMaxSMTPRecipientsInBatch(int lNewValue)
   {
      GetSettings_()->SetLong(PROPERTY_MAXSMTPRECIPIENTSINBATCH, lNewValue);
   }


   int
   SMTPConfiguration::GetRuleLoopLimit()
   {
      return GetSettings_()->GetLong(PROPERTY_RULELOOPLIMIT);
   }

   void 
   SMTPConfiguration::SetRuleLoopLimit(int lNewValue)
   {
      GetSettings_()->SetLong(PROPERTY_RULELOOPLIMIT, lNewValue);
   }


   int
   SMTPConfiguration::GetMaxNumberOfMXHosts()
   {
      return GetSettings_()->GetLong(PROPERTY_MAX_NUMBER_OF_MXHOSTS);
   }

   void 
   SMTPConfiguration::SetMaxNumberOfMXHosts(int lNewValue)
   {
      GetSettings_()->SetLong(PROPERTY_MAX_NUMBER_OF_MXHOSTS, lNewValue);
   }

   bool
   SMTPConfiguration::GetSMTPRelayerRequiresAuthentication()
   {
      return GetSettings_()->GetBool(PROPERTY_SMTPRELAYER_USEAUTH);
   }

   void
   SMTPConfiguration::SetSMTPRelayerRequiresAuthentication(bool bNewVal)
   {
      GetSettings_()->SetBool(PROPERTY_SMTPRELAYER_USEAUTH, bNewVal);
   }

   void 
   SMTPConfiguration::SetSMTPRelayerUsername(const String & sValue)
   {
      GetSettings_()->SetString(PROPERTY_SMTPRELAYER_USERNAME, sValue);
   }

   String
   SMTPConfiguration::GetSMTPRelayerUsername() const
   {
      return GetSettings_()->GetString(PROPERTY_SMTPRELAYER_USERNAME);
   }

   void 
   SMTPConfiguration::SetSMTPRelayerPassword(const String & sValue)
   {
      GetSettings_()->SetString(PROPERTY_SMTPRELAYER_PASSWORD, sValue);
   }

   String
   SMTPConfiguration::GetSMTPRelayerPassword() const
   {
      return GetSettings_()->GetString(PROPERTY_SMTPRELAYER_PASSWORD);
   }




   void
   SMTPConfiguration::SetAllowIncorrectLineEndings(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_ALLOWINCORRECTLINEENDINGS, newVal);
   }

   bool 
   SMTPConfiguration::GetAllowIncorrectLineEndings() 
   {
      return GetSettings_()->GetBool(PROPERTY_ALLOWINCORRECTLINEENDINGS);
   }

   void
   SMTPConfiguration::SetAddDeliveredToHeader(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_ADDDELIVEREDTOHEADER, newVal);
   }

   bool 
   SMTPConfiguration::GetAddDeliveredToHeader() 
   {
      return GetSettings_()->GetBool(PROPERTY_ADDDELIVEREDTOHEADER);
   }

   void 
   SMTPConfiguration::OnPropertyChanged(std::shared_ptr<Property> pProperty)
   {
      String sPropertyName = pProperty->GetName();

      if (sPropertyName == PROPERTY_MAXDELIVERYTHREADS)
      {
         std::shared_ptr<SMTPDeliveryManager> pDeliveryManager = Application::Instance()->GetSMTPDeliveryManager();
         if (!pDeliveryManager)
            return;

         pDeliveryManager->OnPropertyChanged(pProperty);
      }
   }

   bool 
   SMTPConfiguration::XMLStore(XNode *pBackupNode, int iOptions)
   {
      if (!routes_->XMLStore(pBackupNode, iOptions))
         return false;

      if (!incoming_relays_->XMLStore(pBackupNode, iOptions))
         return false;

      return true;
   }

   bool
   SMTPConfiguration::XMLLoad(XNode *pBackupNode, int iRestoreOptions)
   {
      routes_->Refresh();
      routes_->XMLLoad(pBackupNode, iRestoreOptions);
 
      incoming_relays_->Refresh();
      incoming_relays_->XMLLoad(pBackupNode, iRestoreOptions);

      return true;
   }


}
