// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandAuthenticate.h"
#include "IMAPConnection.h"
#include "IMAPConfiguration.h"
#include "IMAPSimpleCommandParser.h"
#include "../common/Application/DefaultDomain.h"
#include "../common/Util/AccountLogon.h"
#include "../common/BO/Account.h"
#include "../common/BO/SecurityRange.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
	IMAPResult
   IMAPCommandAUTHENTICATE::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
	{
      if (!Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPSASLPlain())
         return IMAPResult(IMAPResult::ResultNo, "IMAP AUTHENTICATE is not enabled.");

      String imapmasteruser = Configuration::Instance()->GetIMAPConfiguration()->GetIMAPMasterUser();
		String sParam, authzid, authcid, password, sDecode64;
		String sDefaultDomain = Configuration::Instance()->GetDefaultDomain();

		if (pConnection->GetConnectionSecurity() == CSSTARTTLSRequired)
		{
			if (!pConnection->IsSSLConnection())
			{
				return IMAPResult(IMAPResult::ResultBad, "STARTTLS is required.");
			}
		}

		if (pConnection->GetSecurityRange()->GetRequireTLSForAuth() && !pConnection->IsSSLConnection())
		{
			return IMAPResult(IMAPResult::ResultBad, "A SSL/TLS-connection is required for authentication.");
		}

		std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());

		pParser->Parse(pArgument);

		size_t paramcount = pParser->ParamCount();

		if (paramcount < 1 || paramcount > 2)
			return IMAPResult(IMAPResult::ResultBad, "Unsupported Authenticate mechanism.");

		sParam = pParser->GetParamValue(pArgument, 0);
		if (sParam != _T("PLAIN"))
			return IMAPResult(IMAPResult::ResultBad, "Unsupported Authenticate mechanism.");

		if (paramcount == 1)
		{
			pConnection->SetCommandBuffer(pArgument->Tag() + " AUTHENTICATE PLAIN ");
			pConnection->SendAsciiData("+ \r\n");
			return IMAPResult();
		}

		sParam = pParser->GetParamValue(pArgument, 1);
		StringParser::Base64Decode(sParam, sDecode64);
		std::vector<String> plain_args = StringParser::SplitString(sDecode64, "\t");

		if (plain_args.size() != 3)
			return IMAPResult(IMAPResult::ResultBad, "Command has malformed base64 token.");

		authzid = plain_args[0];

		authcid = plain_args[1];
		if (plain_args[1].GetLength() == 0)
			return IMAPResult(IMAPResult::ResultBad, "Command is missing username.");

		password = plain_args[2];
		if (plain_args[2].GetLength() == 0)
			return IMAPResult(IMAPResult::ResultBad, "Command is missing password.");

		// we don't really need to canonicalize the username(s), but it makes it much
		// cleaner and safer to not have to worry about who has a domain name in their
		// user name

		if (authcid.Find(_T("@")) == -1)
		{
			if (sDefaultDomain.IsEmpty())
				return IMAPResult(IMAPResult::ResultNo, "Invalid user name. Please use full email address as user name.");

			authcid = DefaultDomain::ApplyDefaultDomain(authcid);
		}

		// if the client specified two usernames, the first is who we will be acting as,
		// the second is who we authenticate as.  make sure the client isn't trying to
		// pull a fast one (or is confused)

		if (authzid.GetLength())
		{
			if (imapmasteruser.GetLength() == 0)
				return IMAPResult(IMAPResult::ResultBad, "No master user defined.");

			if (authzid.Find(_T("@")) == -1)
			{
				if (sDefaultDomain.IsEmpty())
					return IMAPResult(IMAPResult::ResultNo, "Invalid user name. Please use full email address as user name.");

				imapmasteruser = DefaultDomain::ApplyDefaultDomain(imapmasteruser);
				authzid = DefaultDomain::ApplyDefaultDomain(authzid);
			}
			else
				imapmasteruser += "@" + StringParser::ExtractDomain(authcid);

			if (imapmasteruser.compare(authcid))
				return IMAPResult(IMAPResult::ResultBad, "Invalid master user.");
		}

		AccountLogon accountLogon;
		bool disconnect = false;
		std::shared_ptr<const Account> pAccount = accountLogon.Logon(pConnection->GetRemoteEndpointAddress(), authzid, authcid, password, disconnect);

		if (disconnect)
		{
			String sResponse = "* Too many invalid logon attempts.\r\n";
			sResponse += pArgument->Tag() + " BAD Goodbye\r\n";
			pConnection->Logout(sResponse);

			return IMAPResult(IMAPResult::ResultOKSupressRead, "");
		}

		if (!pAccount)
		{
			return IMAPResult(IMAPResult::ResultNo, "Invalid user name or password.");
		}

		// Load mail boxes
		pConnection->Login(pAccount);

		String sResponse = pArgument->Tag() + " OK LOGIN completed\r\n";

		pConnection->SendAsciiData(sResponse);

		return IMAPResult();
	}
}
