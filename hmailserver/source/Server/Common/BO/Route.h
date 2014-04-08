#pragma once

namespace HM
{
   class RouteAddresses;

   class Route : public BusinessObject<Route>
   {
   public:
	   Route();
	   virtual ~Route();

      String GetName() const {return m_sDomainName; }

      void DomainName(const String &sVal) {m_sDomainName = sVal; }
      String DomainName() const {return m_sDomainName; } 

      void SetDescription(const String &sVal) {m_sDescription = sVal; }
      String GetDescription() const {return m_sDescription; } 


      void TargetSMTPHost(const String &sVal) {m_sTargetSMTPHost = sVal; }
      String TargetSMTPHost() const {return m_sTargetSMTPHost; } 

      void TargetSMTPPort(long lVal) {m_lTargetSMTPPort = lVal; }
      long TargetSMTPPort() const {return m_lTargetSMTPPort; } 

      void NumberOfTries(long lVal) {m_lNumberOfTries = lVal; }
      long NumberOfTries() const {return m_lNumberOfTries; } 
   
      void MinutesBetweenTry(long lVal) {m_lMinutesBetweenTry = lVal; }
      long MinutesBetweenTry() const {return m_lMinutesBetweenTry; } 

      bool ToAllAddresses() const {return m_bToAllAddresses; }
      void ToAllAddresses(bool bNewVal) {m_bToAllAddresses = bNewVal; }

      bool GetRelayerRequiresAuth() const {return m_bRelayerRequiresAuthentication; }
      void SetRelayerRequiresAuth(bool bNewVal) {m_bRelayerRequiresAuthentication = bNewVal; }

      String GetRelayerAuthUsername() const {return m_sRelayerAuthUsername; }
      String GetRelayerAuthPassword()const {return m_sRelayerAuthPassword; }
      void SetRelayerAuthUsername(const String &sNewVal) {m_sRelayerAuthUsername = sNewVal; }
      void SetRelayerAuthPassword(const String &sNewVal) {m_sRelayerAuthPassword = sNewVal; }

      bool GetTreatRecipientAsLocalDomain() const  {return m_bTreatRecipientAsLocalDomain; }
      void SetTreatRecipientAsLocalDomain(bool bNewVal) {m_bTreatRecipientAsLocalDomain = bNewVal; }

      bool GetTreatSenderAsLocalDomain() const  {return m_bTreatSenderAsLocalDomain; }
      void SetTreatSenderAsLocalDomain(bool bNewVal) {m_bTreatSenderAsLocalDomain = bNewVal; }


      bool GetUseSSL() const  {return m_bUseSSL; }
      void SetUseSSL(bool bNewVal) {m_bUseSSL = bNewVal; }

      shared_ptr<RouteAddresses> GetAddresses();
   
      bool XMLStore(XNode *pRoutesNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions);

   protected:

      String m_sDomainName;
      String m_sDescription;
      String m_sTargetSMTPHost;
      long m_lTargetSMTPPort;
      long m_lNumberOfTries;
      long m_lMinutesBetweenTry;
      bool m_bToAllAddresses;
      
      bool m_bRelayerRequiresAuthentication;
      String m_sRelayerAuthUsername;
      String m_sRelayerAuthPassword;
      bool m_bUseSSL;
      bool m_bTreatRecipientAsLocalDomain;
      bool m_bTreatSenderAsLocalDomain;

      shared_ptr<RouteAddresses> m_pAddresses;
   };

}
