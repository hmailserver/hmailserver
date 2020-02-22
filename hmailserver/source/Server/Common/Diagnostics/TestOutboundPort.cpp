// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "TestOutboundPort.h"
#include "TestConnect.h"

#include "../../SMTP/SMTPConfiguration.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TestOutboundPort::TestOutboundPort(const String &TestDomainName) :
      local_test_domain_name_(TestDomainName)
   {

   }

   TestOutboundPort::~TestOutboundPort()
   {

   }

   DiagnosticResult
   TestOutboundPort::PerformTest()
   {
      DiagnosticResult diagResult;
      diagResult.SetName("Test outbound port");
      diagResult.SetDescription("Tests to connect to a remote SMTP server.");

      String runDetails;


      auto smtp_configuration = Configuration::Instance()->GetSMTPConfiguration();
      String localAddressStr = smtp_configuration->GetSMTPDeliveryBindToIP();
      String smtpHost = smtp_configuration->GetSMTPRelayer();
      int smtpPort = smtp_configuration->GetSMTPRelayerPort();
      auto connection_security = smtp_configuration->GetSMTPRelayerConnectionSecurity();

      if (smtpHost.GetLength() == 0)
      {
         smtpHost = local_test_domain_name_;
         smtpPort = 25;

         String formattedString;
         formattedString.Format(_T("SMTP relayer not in use. Attempting %s:%i...\r\n"), smtpHost.c_str(), smtpPort);

         // Test to connect to mail.hmailserver.com or local_test_domain_name_
         runDetails = formattedString;

      }
      else
      {
         runDetails.AppendFormat(_T("SMTP relayer is in use.\r\n"));
      }
      
      TestConnect connTest;
      
      if (connTest.PerformTest(connection_security, localAddressStr, smtpHost, smtpPort, runDetails))
      {
         diagResult.SetDetails((runDetails));
         diagResult.SetSuccess(true);
      }
      else
      {
         diagResult.SetDetails((runDetails));
         diagResult.SetSuccess(false);
      }

      return diagResult;
   }


   
      
}
