// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "SMTPConfiguration.h"
#include "SMTPDeliveryManager.h"

#include "SRS/SRS.h"

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

      EnsureSRSSecretExists_();

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
   SMTPConfiguration::SetSRSEnabled(bool newValue)
   {
      GetSettings_()->SetBool(PROPERTY_SRS_ENABLED, newValue);
   }

   bool
   SMTPConfiguration::GetSRSEnabled()
   {
      return GetSettings_()->GetBool(PROPERTY_SRS_ENABLED);
   }

   void
   SMTPConfiguration::SetSRSSecret(const String &newValue)
   {
      if (newValue.IsEmpty())
      {
         // Clearing the secret rotates it rather than switching SRS off: a server with
         // SRS enabled and no secret can neither rewrite nor reverse an address. Every
         // address handed out under the old secret stops being reversible.
         AnsiString generatedSecret = SRS::GenerateSecret();

         if (generatedSecret.IsEmpty())
            return;

         GetSettings_()->SetString(PROPERTY_SRS_SECRET, generatedSecret);

         return;
      }

      GetSettings_()->SetString(PROPERTY_SRS_SECRET, newValue);
   }

   String
   SMTPConfiguration::GetSRSSecret()
   {
      return GetSettings_()->GetString(PROPERTY_SRS_SECRET);
   }

   void
   SMTPConfiguration::SetSRSMaxAgeDays(int newValue)
   {
      GetSettings_()->SetLong(PROPERTY_SRS_MAXAGEDAYS, newValue);
   }

   int
   SMTPConfiguration::GetSRSMaxAgeDays()
   {
      int value = GetSettings_()->GetLong(PROPERTY_SRS_MAXAGEDAYS);

      if (value <= 0)
         return SRS::DefaultMaxAgeDays;

      // Clamped the way SRS clamps it, so that what is reported here is what the server
      // actually goes by. Anything below the minimum of one day has already selected the
      // default above.
      if (value > SRS::MaxMaxAgeDays)
         return SRS::MaxMaxAgeDays;

      return value;
   }

   void
   SMTPConfiguration::SetSRSHashLength(int newValue)
   {
      GetSettings_()->SetLong(PROPERTY_SRS_HASHLENGTH, newValue);
   }

   int
   SMTPConfiguration::GetSRSHashLength()
   {
      int value = GetSettings_()->GetLong(PROPERTY_SRS_HASHLENGTH);

      if (value <= 0)
         return SRS::DefaultHashLength;

      if (value < SRS::MinHashLength)
         return SRS::MinHashLength;

      if (value > SRS::MaxHashLength)
         return SRS::MaxHashLength;

      return value;
   }

   void
   SMTPConfiguration::EnsureSRSSecretExists_()
   {
      // The secret is what tells an address this server has handed out from one someone
      // else has made up, so every installation needs one of its own. It is generated
      // once, on the first start after the setting appears in the database, and then
      // left alone: changing it invalidates every address already out there.
      //
      // Servers sharing a database share the secret, which is what a set-up where mail
      // for one domain arrives at more than one of them needs.
      if (!GetSRSSecret().IsEmpty())
         return;

      SetSRSSecret("");

      // Two servers sharing a database and starting together both find it empty and both
      // generate one. The last write is the one the database keeps, so it is read back
      // here and both servers go on with the same secret rather than one of them signing
      // addresses the other cannot reverse.
      GetSettings_()->Refresh();
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
