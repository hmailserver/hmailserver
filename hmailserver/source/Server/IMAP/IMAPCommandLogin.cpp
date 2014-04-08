// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandLogin.h"
#include "IMAPConnection.h"
#include "IMAPSimpleCommandParser.h"

#include "../common/Util/AccountLogon.h"
#include "../common/BO/Account.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPResult
   IMAPCommandLOGIN::ExecuteCommand(shared_ptr<HM::IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument)
   {
      shared_ptr<IMAPSimpleCommandParser> pParser = shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());
      
      pParser->Parse(pArgument);
      
      if (pParser->ParamCount() != 2)
      {
         return IMAPResult(IMAPResult::ResultBad, "Command requires 2 parameters");
      }

      // The folder wildcard could be sent in a seperate buffer.
      String sUsername = pParser->GetParamValue(pArgument, 0);
      String sPassword = pParser->GetParamValue(pArgument, 1);

      AccountLogon accountLogon;
      bool disconnect = false;
      shared_ptr<const Account> pAccount = accountLogon.Logon(pConnection->GetIPAddress(), sUsername, sPassword, disconnect);

      if (disconnect)
      {
         String sResponse = "* Too many invalid logon attempts.\r\n"; 
         sResponse += pArgument->Tag() + " BAD Goodbye\r\n";
         pConnection->Logout(sResponse);   

         return IMAPResult();
      } 

      if (!pAccount)
      {
         if (sUsername.Find(_T("@")) == -1)
            return IMAPResult(IMAPResult::ResultNo, "Invalid user name or password. Please use full email address as user name.");
         else
            return IMAPResult(IMAPResult::ResultNo, "Invalid user name or password.");
      }
      
      // Load mail boxes
      pConnection->Login(pAccount);

      String sResponse = pArgument->Tag() + " OK LOGIN completed\r\n";
      pConnection->SendAsciiData(sResponse);   
      
      return IMAPResult();

   }
}
