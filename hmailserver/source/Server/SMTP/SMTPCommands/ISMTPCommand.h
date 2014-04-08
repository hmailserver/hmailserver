// ISMTPCommand.h: interface for the ISMTPCommand class.
//
//////////////////////////////////////////////////////////////////////


#pragma once

namespace HM
{
   class SMTPConnection;

   class ISMTPCommand  
   {
   public:
	   ISMTPCommand();
	   virtual ~ISMTPCommand();

      virtual void ExecuteCommand(SMTPConnection* pSMTPConnection ) = 0;

   };
}
