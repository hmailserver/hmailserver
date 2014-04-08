// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPCommandRangeAction.h"
#include "../Common/MIME/Mime.h"
#include "IMAPFetchParser.h"

namespace HM
{

   class IMAPConnection;
   class Message;
   class ByteBuffer; 
   class IMAPFetchParser;
   

   class IMAPFetch : public IMAPCommandRangeAction
   {
   public:
	   IMAPFetch();
	   virtual ~IMAPFetch();

      virtual IMAPResult DoAction(shared_ptr<IMAPConnection> pConnection, int messageIndex, shared_ptr<Message> pMessage, const shared_ptr<IMAPCommandArgument> pArgument);

      
   private:
      
      String _CreateEnvelopeStructure(MimeHeader& oHeader);
      String _GetPartStructure(shared_ptr<MimeBody> oPart, bool includeExtensionData, int iRecursion);
      String _IteratePartRecursive(shared_ptr<MimeBody> oPart, bool includeExtensionData, int iRecursion);
      String _CreateEmailStructure(const String &sField);
      shared_ptr<MimeBody>_GetMessagePartByPartNo(shared_ptr<MimeBody>pBody, long iPartNo);

      shared_ptr<ByteBuffer> _GetByteBufferByBodyPart(const String &messageFileName, shared_ptr<MimeBody> pBodyPart, IMAPFetchParser::BodyPart &oPart);
      shared_ptr<MimeBody> _GetBodyPartByRecursiveIdentifier(shared_ptr<MimeBody> pBody, const String &sName);


      void _GetBytesToSend(int iBufferSize, IMAPFetchParser::BodyPart &oPart, int &iOutStart, int &iOutCount);

      void _ReportCriticalError(const String &messageFileName, const String &sMessage);

      void _AppendOutput(String &sOutput, const String &sAppend);
      void _SendAndReset(shared_ptr<IMAPConnection> pConnection, String &sOutput);

      shared_ptr<MimeBody> _LoadMimeBody(shared_ptr<IMAPFetchParser> pParser, const String &fileName);
      bool _GetMessageBodyNeeded(shared_ptr<IMAPFetchParser> pParser);

      bool m_bAppendSpace;

      shared_ptr<IMAPFetchParser> m_pParser;

   };

}
