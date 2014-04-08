// OutboundPortConnection.cpp: implementation of the OutboundPortConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OutboundPortConnection.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace HM
{

   OutboundPortConnection::OutboundPortConnection() 
   {
      SetTimeout(10); 
   }

   OutboundPortConnection::~OutboundPortConnection()
   {
      
   }

   void
   OutboundPortConnection::OnConnected()
   {
      PostDisconnect();
   }

   AnsiString 
   OutboundPortConnection::GetCommandSeparator() const
   {
      return "\r\n";
   }

   void
   OutboundPortConnection::ParseData(const AnsiString &Request)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Parses a server SMTP cmmand.
   //---------------------------------------------------------------------------()
   {

   }

   void 
   OutboundPortConnection::OnCouldNotConnect(const AnsiString &sErrorDescription)
   {
      
   }


}
