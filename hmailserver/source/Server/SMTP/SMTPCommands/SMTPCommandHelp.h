// SMTPCommandHelp.h: interface for the SMTPCommandHelp class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ISMTPCommand.h"

namespace HM
{
   class SMPTConnection;
   
   class SMTPCommandHelp : public ISMTPCommand
   {
   public:
	   SMTPCommandHelp();
	   virtual ~SMTPCommandHelp();


      virtual void ExecuteCommand(SMTPConnection* pSMTPConnection );

   };

}
