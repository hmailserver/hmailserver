// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SpamTestHeloHost.h"

#include "SpamTestData.h"
#include "SpamTestResult.h"

#include "AntiSpamConfiguration.h"

#include "../TCPIP/DNSResolver.h"
#include "../TCPIP/IPAddress.h"
#include "../Util/TLD.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String 
   SpamTestHeloHost::GetName() const
   {
      return "SpamTestHeloHost";
   }

   bool 
   SpamTestHeloHost::GetIsEnabled()
   {
      if (Configuration::Instance()->GetAntiSpamConfiguration().GetCheckHostInHelo())
         return true;
      else
         return false;
   }

   set<shared_ptr<SpamTestResult> >
   SpamTestHeloHost::RunTest(shared_ptr<SpamTestData> pTestData)
   {
      set<shared_ptr<SpamTestResult> > setSpamTestResults;

      const IPAddress &iIPAdress = pTestData->GetConnectingIP();
      String sHeloHost = pTestData->GetHeloHost();

      if (!_CheckHostInHelo(sHeloHost, iIPAdress))
      {
         // Incorrect host in helo
         String sMessage = "The host name specified in HELO does not match IP address.";
         int iScore = Configuration::Instance()->GetAntiSpamConfiguration().GetCheckHostInHeloScore();;

         shared_ptr<SpamTestResult> pResult = shared_ptr<SpamTestResult>(new SpamTestResult(GetName(), SpamTestResult::Fail, iScore, sMessage));
         setSpamTestResults.insert(pResult);   

      }
      
      return setSpamTestResults;
   }


   bool 
   SpamTestHeloHost::_CheckHostInHelo(const String &sHeloHost, const IPAddress &address)
   {
      String sIPAddress = address.ToString();

      bool bMatch = false;

      if (sHeloHost.Left(1) == _T("["))
      {
         String sTempHost = sHeloHost;
         sTempHost.TrimLeft(_T("["));
         sTempHost.TrimRight(_T("]"));

         if (sTempHost == sIPAddress)
         {
            return true;
         }
      }
      else
      {
         // Check that the host name given in helo actually matches
         // the senders IP address.
         std::vector<String> saFoundNames;
         DNSResolver resolver;
         if (!resolver.GetARecords(sHeloHost, saFoundNames))
         {
            // DNS failure. Assume it's not spam.
            return true;
         }

         // Check that the IP address is one of these A records.

         std::vector<String>::iterator iter;
         for (iter = saFoundNames.begin(); iter < saFoundNames.end(); iter++)
         {
            if ((*iter) == sIPAddress)
            {
               return true;
            }
         }
      }

      return false;

   }

}