#pragma once

#include "../TCPIP/SocketConstants.h"

namespace HM
{
   class RouteAddresses;

   class Route : public BusinessObject<Route>
   {
   public:
	   Route();
	   virtual ~Route();

      String GetName() const {return domain_name_; }

      void DomainName(const String &sVal) {domain_name_ = sVal; }
      String DomainName() const {return domain_name_; } 

      void SetDescription(const String &sVal) {description_ = sVal; }
      String GetDescription() const {return description_; } 


      void TargetSMTPHost(const String &sVal) {target_smtphost_ = sVal; }
      String TargetSMTPHost() const {return target_smtphost_; } 

      void TargetSMTPPort(long lVal) {target_smtpport_ = lVal; }
      long TargetSMTPPort() const {return target_smtpport_; } 

      void NumberOfTries(long lVal) {number_of_tries_ = lVal; }
      long NumberOfTries() const {return number_of_tries_; } 
   
      void MinutesBetweenTry(long lVal) {minutes_between_try_ = lVal; }
      long MinutesBetweenTry() const {return minutes_between_try_; } 

      bool ToAllAddresses() const {return to_all_addresses_; }
      void ToAllAddresses(bool bNewVal) {to_all_addresses_ = bNewVal; }

      bool GetRelayerRequiresAuth() const {return relayer_requires_authentication_; }
      void SetRelayerRequiresAuth(bool bNewVal) {relayer_requires_authentication_ = bNewVal; }

      String GetRelayerAuthUsername() const {return relayer_auth_username_; }
      String GetRelayerAuthPassword()const {return relayer_auth_password_; }
      void SetRelayerAuthUsername(const String &sNewVal) {relayer_auth_username_ = sNewVal; }
      void SetRelayerAuthPassword(const String &sNewVal) {relayer_auth_password_ = sNewVal; }

      bool GetTreatRecipientAsLocalDomain() const  {return treat_recipient_as_local_domain_; }
      void SetTreatRecipientAsLocalDomain(bool bNewVal) {treat_recipient_as_local_domain_ = bNewVal; }

      bool GetTreatSenderAsLocalDomain() const  {return treat_sender_as_local_domain_; }
      void SetTreatSenderAsLocalDomain(bool bNewVal) {treat_sender_as_local_domain_ = bNewVal; }


      ConnectionSecurity GetConnectionSecurity() const  {return connection_security_; }
      void SetConnectionSecurity(ConnectionSecurity connectio_security) {connection_security_ = connectio_security; }

      std::shared_ptr<RouteAddresses> GetAddresses();
   
      bool XMLStore(XNode *pRoutesNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions);

   protected:

      String domain_name_;
      String description_;
      String target_smtphost_;
      long target_smtpport_;
      long number_of_tries_;
      long minutes_between_try_;
      bool to_all_addresses_;
      
      bool relayer_requires_authentication_;
      String relayer_auth_username_;
      String relayer_auth_password_;
      ConnectionSecurity connection_security_;
      bool treat_recipient_as_local_domain_;
      bool treat_sender_as_local_domain_;

      std::shared_ptr<RouteAddresses> addresses_;
   };

}
