// SMTPCommandHelp.cpp: implementation of the SMTPCommandHelp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SMTPCommandHelp.h"
#include "../SMTPConnection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace HM
{
   SMTPCommandHelp::SMTPCommandHelp()
   {

   }

   SMTPCommandHelp::~SMTPCommandHelp()
   {

   }

   void
   SMTPCommandHelp::ExecuteCommand(SMTPConnection* pSMTPConnection  )
   {
      pSMTPConnection->SendData("211 DATA HELO EHLO MAIL NOOP QUIT RCPT RSET SAML TURN VRFY\r\n");

   }
}
