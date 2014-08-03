// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommand.h"
#include "IMAPSearchParser.h"


namespace HM
{
   class IMAPSearchParser;
   class IMAPConnection;
   class MessageData;
   class MimeHeader;

   class IMAPCommandSEARCH : public IMAPCommand
   {
   public:
	   IMAPCommandSEARCH(bool bIsSort);
	   virtual ~IMAPCommandSEARCH();

      void SetIsUID() { is_uid_ = true; }

      IMAPResult ExecuteCommand(shared_ptr<IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument);

   private:

      bool MatchesHeaderCriteria_(const String &fileName,shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool MatchesUIDCriteria_(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool MatchesSequenceSetCriteria_(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria, int index);
      bool MatchesTEXTCriteria_(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool MatchesBODYCriteria_(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool MatchesONCriteria_(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool MatchesSENTONCriteria_(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool MatchesSENTBEFORECriteria_(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool MatchesSENTSINCECriteria_(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool MatchesSINCECriteria_(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      
      bool MatchesBEFORECriteria_(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool MatchesLARGERCriteria_(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool MatchesSMALLERCriteria_(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);

      String GetHeaderValue_(const String &fileName, shared_ptr<Message> pMessage, const String &sHeaderField);
      
      
      bool DoesMessageMatch_(shared_ptr<IMAPSearchCriteria> pParentCriteria, const String &fileName, shared_ptr<Message> pMessage, int index);

      shared_ptr<MessageData> message_data_;
      shared_ptr<MimeHeader> mime_header_;

      bool is_sort_;
      bool is_uid_;
   };
}
