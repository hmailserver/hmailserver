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
      _localTestDomainName(TestDomainName)
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


      String localAddressStr = Configuration::Instance()->GetSMTPConfiguration()->GetSMTPDeliveryBindToIP();
      String smtpHost = Configuration::Instance()->GetSMTPConfiguration()->GetSMTPRelayer();
      int smtpPort = Configuration::Instance()->GetSMTPConfiguration()->GetSMTPRelayerPort();

      if (smtpHost.GetLength() == 0)
      {
         smtpHost = _localTestDomainName;
         smtpPort = 25;

         String formattedString;
         formattedString.Format(_T("SMTP relayer not in use. Attempting %s:%i...\r\n"), smtpHost, smtpPort);

         // Test to connect to mail.hmailserver.com or _localTestDomainName
         runDetails = formattedString;

      }
      else
      {
         runDetails.AppendFormat(_T("SMTP relayer is in use.\r\n"));
      }
      
      TestConnect connTest;
      
      if (connTest.PerformTest(localAddressStr, smtpHost, smtpPort, runDetails))
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
