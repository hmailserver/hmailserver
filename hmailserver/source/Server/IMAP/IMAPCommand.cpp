// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPCommand.h"
#include "IMAPConnection.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCommand::IMAPCommand()
   {

   }

   IMAPCommand::~IMAPCommand()
   {

   }

   bool
   IMAPCommand::HasUnsentExpunge(std::shared_ptr<IMAPConnection> pConnection)
   {
      return pConnection->GetFirstUnsentExpungeIndex() > 0;
   }

   bool
   IMAPCommand::IsMessageSequenceNumberStale(std::shared_ptr<IMAPConnection> pConnection, int messageIndex)
   {
      int first_unsent_expunge_index = pConnection->GetFirstUnsentExpungeIndex();

      if (first_unsent_expunge_index == 0)
         return false;

      // Messages below the expunged one still have the same sequence number as
      // when the client last synchronized with us.
      return messageIndex >= first_unsent_expunge_index;
   }

   IMAPResult
   IMAPCommand::GetExpungeIssuedResult()
   {
      // EXPUNGEISSUED is defined in RFC 5530.
      return IMAPResult(IMAPResult::ResultNo, "[EXPUNGEISSUED] Some of the messages have been expunged by another client. Message sequence numbers are out of date - issue a NOOP command and try again.");
   }

   String
   IMAPCommandArgument::Literal(unsigned int Index)
   {
      if (Index >= literals_.size())
         return "";
      else 
         return literals_[Index];
   }

}
