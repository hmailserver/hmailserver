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

      void SetIsUID() { m_bIsUID = true; }

      IMAPResult ExecuteCommand(shared_ptr<IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument);

   private:

      bool _MatchesHeaderCriteria(const String &fileName,shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool _MatchesUIDCriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool _MatchesSequenceSetCriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria, int index);
      bool _MatchesTEXTCriteria(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool _MatchesBODYCriteria(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool _MatchesONCriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool _MatchesSENTONCriteria(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool _MatchesSENTBEFORECriteria(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool _MatchesSENTSINCECriteria(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool _MatchesSINCECriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      
      bool _MatchesBEFORECriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool _MatchesLARGERCriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);
      bool _MatchesSMALLERCriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria);

      String _GetHeaderValue(const String &fileName, shared_ptr<Message> pMessage, const String &sHeaderField);
      
      
      bool _DoesMessageMatch(shared_ptr<IMAPSearchCriteria> pParentCriteria, const String &fileName, shared_ptr<Message> pMessage, int index);

      shared_ptr<MessageData> m_pMessageData;
      shared_ptr<MimeHeader> m_pMimeHeader;

      bool m_bIsSort;
      bool m_bIsUID;
   };
}
