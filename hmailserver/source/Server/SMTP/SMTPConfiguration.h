#pragma once

#include "../Common/BO/IncomingRelays.h"

namespace HM
{
   class PropertySet;
   class Routes;
   class DNSBlackLists;
   class BlockedAttachments;
   enum ConnectionSecurity;

   class SMTPConfiguration
   {
   public:
	   SMTPConfiguration();
	   virtual ~SMTPConfiguration();

      bool Load();

      void SetMaxSMTPConnections(int newVal);
      void SetAuthAllowPlainText(bool newVal);
      void SetAllowMailFromNull(bool newVal);
      void SetLogSMTPConversations(bool bNewVal);
      void SetUseORDB(bool NewVal);
      void SetUseSpamhaus(bool NewVal);
      void SetNoOfRetries(long lNewVal);
      void SetMinutesBetweenTry(long lHoursBetween);
      void SetSMTPRelayer(const String &sRelayer);
      
      void SetSMTPRelayerPort(long lPort);
      long GetSMTPRelayerPort();

      void SetSMTPRelayerConnectionSecurity(ConnectionSecurity connection_security);
      ConnectionSecurity GetSMTPRelayerConnectionSecurity();

      void SetSMTPConnectionSecurity(ConnectionSecurity connection_security);
      ConnectionSecurity GetSMTPConnectionSecurity();

      void SetMaxNoOfDeliveryThreads(int lNewValue);

      String GetWelcomeMessage() const;
      void SetWelcomeMessage(const String &sMessage);

      String GetSMTPDeliveryBindToIP() const;
      void SetSMTPDeliveryBindToIP(const String &sIP);

      bool GetBlockBareLFs() const;

      int GetMaxSMTPConnections();
      bool GetAuthAllowPlainText();
      bool GetAllowMailFromNull();

      long GetMinutesBetweenTry();
      long GetNoOfRetries();
      String GetSMTPRelayer() const;

      int GetMaxNoOfDeliveryThreads();
      

      bool GetSMTPRelayerRequiresAuthentication();
      void SetSMTPRelayerRequiresAuthentication(bool bNewVal);
      String GetSMTPRelayerUsername() const;
      void SetSMTPRelayerUsername(const String & Value);
      
      String GetSMTPRelayerPassword() const;
      void SetSMTPRelayerPassword(const String & Value);

      bool GetAllowIncorrectLineEndings();
      void SetAllowIncorrectLineEndings(bool bNewVal);

      int GetMaxMessageSize();
      void SetMaxMessageSize(int iNewVal);

      int GetMaxSMTPRecipientsInBatch();
      void SetMaxSMTPRecipientsInBatch(int iNewVal);

      int GetRuleLoopLimit();
      void SetRuleLoopLimit(int iNewVal);

      int GetMaxNumberOfMXHosts();
      void SetMaxNumberOfMXHosts(int iNewVal);


      bool XMLStore(XNode *pBackupNode, int Options);
      bool XMLLoad(XNode *pBackupNode, int iRestoreOptions);

      bool GetAddDeliveredToHeader();
      void SetAddDeliveredToHeader(bool bNewVal);

      void OnPropertyChanged(std::shared_ptr<Property> pProperty);

      std::shared_ptr<IncomingRelays> GetIncomingRelays() {return incoming_relays_;}
      std::shared_ptr<Routes> GetRoutes() {return routes_;}

   private:

      std::shared_ptr<PropertySet> GetSettings_() const;
      std::shared_ptr<IncomingRelays> incoming_relays_;
      std::shared_ptr<Routes> routes_;
   };
}
