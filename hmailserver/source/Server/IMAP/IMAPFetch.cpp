// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPFetch.h"
#include "IMAPFetchParser.h"
#include "IMAPConnection.h"

#include "../Common/Application/FolderManager.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/Message.h"
#include "../Common/Util/Charset.h"
#include "../Common/Mime/MimeCode.h"
#include "../Common/Util/Time.h"
#include "../Common/Util/Parsing/AddressListParser.h"
#include "../Common/Util/ByteBuffer.h"
#include "../Common/BO/ACLPermission.h"

#include "../Common/Persistence/PersistentMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   IMAPFetch::IMAPFetch() :
      m_bAppendSpace(false)
   {
      
   }

   IMAPFetch::~IMAPFetch()
   {

   }

   IMAPResult
   IMAPFetch::DoAction(shared_ptr<IMAPConnection> pConnection, int messageIndex, shared_ptr<Message> pMessage, const shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pArgument || !pMessage)
         return IMAPResult(IMAPResult::ResultBad, "Invalid parameters");

      const String messageFileName = PersistentMessage::GetFileName(pConnection->GetAccount(), pMessage);

      m_bAppendSpace = false;

      // Parse the command
      if (!m_pParser)
      {
         m_pParser = shared_ptr<IMAPFetchParser>(new IMAPFetchParser());
         String sTemp = pArgument->Command();
         m_pParser->ParseCommand(sTemp);
      }

      // If we're going to touch the file, make sure it's there.
      bool willReadMessageFile =
         m_pParser->GetShowEnvelope() ||
         m_pParser->GetShowBodyStructure() ||
         m_pParser->GetShowBodyStructureNonExtensible()||
         m_pParser->GetPartsToLookAt().size() > 0;

      if (willReadMessageFile)
      {
         // Ensure that the data file exists.
         PersistentMessage::EnsureFileExistance(pConnection->GetAccount(), pMessage);
      }

      MimeHeader oMimeHeader;

      // Time to output this...
      String sOutput;
   
      sOutput.Format(_T("* %d FETCH ("), messageIndex);

      // We should always show UID when client is issuing UID fetch..
      if (m_pParser->GetShowUID() || GetIsUID()) 
      {
         String sUIDTag;
         sUIDTag.Format(_T("UID %u"), pMessage->GetUID());
         
         _AppendOutput(sOutput, sUIDTag);
      }

      if (m_pParser->GetShowRFCSize())
      {
         String sTemp;
         sTemp.Format(_T("RFC822.SIZE %d"), pMessage->GetSize());

         _AppendOutput(sOutput, sTemp);
      }

      if (m_pParser->GetShowFlags())
      {
         String sTemp = "FLAGS (";

         std::set<String> setFlags;
         
         if (pMessage->GetFlagSeen())
            setFlags.insert("Seen");
         if (pMessage->GetFlagDeleted())
            setFlags.insert("Deleted");
         if (pMessage->GetFlagFlagged())
            setFlags.insert("Flagged");
         if (pMessage->GetFlagDraft())
            setFlags.insert("Draft");
         if (pMessage->GetFlagAnswered())
            setFlags.insert("Answered");
            
         std::set<String>::iterator iter = setFlags.begin();
         std::set<String>::iterator iterEnd = setFlags.end();

         for (; iter != iterEnd; iter++)
         {
            if (iter != setFlags.begin())
               sTemp += " ";

            sTemp += "\\" + (*iter);
         }

         sTemp += ")";

         _AppendOutput(sOutput, sTemp);
      }


      if (m_pParser->GetShowInternalDate())
      {
         String sTemp = "INTERNALDATE \"";

         String sCreateTime = pMessage->GetCreateTime();
         
         if (!Time::SeemsToBeValidYear(sCreateTime))
            sCreateTime = Time::GetCurrentDateTime();

         String sMIMECreateTime = Time::GetIMAPInternalDate(sCreateTime);

         sTemp += sMIMECreateTime + "\"";

         _AppendOutput(sOutput, sTemp);
      }

      
      AnsiString sMessageHeader;

      if (m_pParser->GetShowEnvelope())
      {
         // Parse the MIME header
         if (sMessageHeader.IsEmpty())
         {
            sMessageHeader = PersistentMessage::LoadHeader(messageFileName);

            try
            {
               oMimeHeader.Load(sMessageHeader, sMessageHeader.length());
            }
            catch (...)
            {
               _ReportCriticalError(messageFileName, "ERROR HM10003 - IMAP FETCH: Error when loading MIME header for message.");
            }
         }

         try
         {
            String sTemp;
            sTemp = "ENVELOPE " + _CreateEnvelopeStructure(oMimeHeader);
            _AppendOutput(sOutput, sTemp);
         }
         catch (...)
         {
            _ReportCriticalError(messageFileName, "ERROR HM10004 - IMAP FETCH: Error when creating ENVELOPE structure for message.");
         }
         
      }



      // Sometimes we need to load the entire mail into memory.
      // If the user want's to download a specific attachment or
      // if he wants to look at the structure of the mime message.
      shared_ptr<MimeBody> pMimeBody = _LoadMimeBody(m_pParser, messageFileName);
      
      if (m_pParser->GetShowBodyStructure() ||
          m_pParser->GetShowBodyStructureNonExtensible())
      {
         try
         {
            String sResult = "";
            if (m_pParser->GetShowBodyStructure())
               sResult = "BODYSTRUCTURE " + _IteratePartRecursive(pMimeBody, true, 0);
            else
               sResult = "BODY " + _IteratePartRecursive(pMimeBody, false, 0);

            _AppendOutput(sOutput, sResult);
         }
         catch (...)
         {
            _ReportCriticalError(messageFileName, "ERROR HM10002 - IMAP FETCH: Error when creating body structure for message.");
         }
      }

      std::vector<IMAPFetchParser::BodyPart> vecPartsToPeekAt = m_pParser->GetPartsToLookAt();
      
      if (vecPartsToPeekAt.size() > 0)
      {
         std::vector<IMAPFetchParser::BodyPart>::iterator iter = vecPartsToPeekAt.begin();

         // First we should only print the part sizes..
         while (iter != vecPartsToPeekAt.end())   
         {
            IMAPFetchParser::BodyPart oPart = (*iter);

            int iOctetStart = oPart.m_iOctetStart;
            int iOctetCount = oPart.m_iOctetCount;
           
            const String messageFileName = PersistentMessage::GetFileName(pConnection->GetAccount(), pMessage);
            shared_ptr<ByteBuffer> pBuffer = _GetByteBufferByBodyPart(messageFileName, pMimeBody, oPart);
            
            String sPartIdentifier;
            if (iOctetStart == -1 && iOctetCount == -1)
               sPartIdentifier.Format(_T("%s"), oPart.GetDescription());
            else
               sPartIdentifier.Format(_T("%s<%d>"), oPart.GetDescription(), iOctetStart, iOctetCount);

            if (pBuffer->GetSize() > 0)
            {
               // Send part size information
               String sTemp;
               sTemp.Format(_T("%s {%d}\r\n"), sPartIdentifier, pBuffer->GetSize()); // Add 2 for trailing newline.
               
               _AppendOutput(sOutput, sTemp);
               _SendAndReset(pConnection, sOutput);

               // Send the actual part
               pConnection->SendData(pBuffer);
            }
            else
            {
               // The client requested a part that does not exist.
               // Just give the client an empty string for that part.
               String sTemp;
               sTemp.Format(_T("%s \"\""), sPartIdentifier);
               _AppendOutput(sOutput, sTemp);
            }

            iter++;
         }
      }
     
      if (m_pParser->GetSetSeenFlag() && !pConnection->GetCurrentFolderReadOnly())
      {
         // Check if ther user has access to set the Seen flag, otherwise 
         bool bMayChangeSeen = pConnection->CheckPermission(pConnection->GetCurrentFolder(), ACLPermission::PermissionWriteSeen);

         // Since the user has looked at the email, we should set the Seen flag.
         if (bMayChangeSeen && !pMessage->GetFlagSeen())
         {  
            // Update seen flag in the cached copy of the message...
            pMessage->SetFlagSeen(true);

            Application::Instance()->GetFolderManager()->UpdateMessageFlags(
               (int) pConnection->GetCurrentFolder()->GetAccountID(), 
               (int) pConnection->GetCurrentFolder()->GetID(),
               pMessage->GetID(), pMessage->GetFlags());
         }
      }

      sOutput += ")\r\n";
      _SendAndReset(pConnection, sOutput);

      return IMAPResult();
   }

   void
   IMAPFetch::_GetBytesToSend(int iBufferSize, IMAPFetchParser::BodyPart &oPart, int &iOutStart, int &iOutCount)
   {
      int iOctetStart = oPart.m_iOctetStart;
      int iOctetCount = oPart.m_iOctetCount;

      if (iOctetStart == -1 && iOctetCount == -1)
      {
         iOctetStart = 0;
         iOctetCount = iBufferSize;
      }
      else
      {
         // Jump forward to the start of the buffer.
         iBufferSize -= iOctetStart;

         // Check if block size is within buffer.
         if (iOctetCount > iBufferSize)
         {
            // The requested total goes beyond the
            // end of the buffer. We need to truncate
            // the requested length so that it fits
            // within the buffer.
            iOctetCount = iBufferSize;
         }
      }

      iOutStart = iOctetStart;
      iOutCount = iOctetCount;
   }

   shared_ptr<MimeBody> 
   IMAPFetch::_GetBodyPartByRecursiveIdentifier(shared_ptr<MimeBody> pBody, const String &sName)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns a body part by a given identifier. An identifier can be 1, 2, 1.2 etc.
   //---------------------------------------------------------------------------()
   {
      if (!pBody || sName.IsEmpty())
      {
         shared_ptr<MimeBody> pEmpty;
         return pEmpty;
      }

      String sTempName = sName;


      // First split the string from 1, 2.1 and so on.
      std::vector<String> vecPartPath = StringParser::SplitString(sTempName, ".");

      // Iterate over the vector to find the part.
      std::vector<String>::iterator iterPart = vecPartPath.begin();

      while (iterPart != vecPartPath.end())
      {
         String sSubPart = *iterPart;

         if (StringParser::IsNumeric(sSubPart))
         {
            int iRequestPartNo = _ttoi(sSubPart);

            pBody = _GetMessagePartByPartNo(pBody, iRequestPartNo);

            if (!pBody)
            {
               // Client is requesting an non-existant body.
               return pBody;
            }

            if (pBody->IsEncapsulatedRFC822Message())
            {
               try
               {
                  pBody = pBody->LoadEncapsulatedMessage();
               }
               catch (...)
               {
                  String sErrorMessage = "Error when creating ENVELOPE structure for encapsulated message.";

                  ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5060, "IMAPFetch::_GetBodyPartByRecursiveIdentifier", sErrorMessage);

                  shared_ptr<MimeBody> empty;
                  return empty;
               }
            }

         }
     
         iterPart++;
      }

      return pBody;
   }


   shared_ptr<ByteBuffer> 
   IMAPFetch::_GetByteBufferByBodyPart(const String &messageFileName, shared_ptr<MimeBody> pBodyPart, IMAPFetchParser::BodyPart &oPart)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns a buffer containing the data for the given body/part
   //---------------------------------------------------------------------------()
   {
      shared_ptr<ByteBuffer> pOutBuf = shared_ptr<ByteBuffer>(new ByteBuffer);

      if (!pBodyPart)
      {
         // No body provided. We'll have to return
         // an empty buffer.
         return pOutBuf;
      }

      // First make sure that we got the right part of it.
      if (!oPart.GetName().IsEmpty())
      {
         String sMimePart;
         pBodyPart  = _GetBodyPartByRecursiveIdentifier(pBodyPart, oPart.GetName());

         if (!pBodyPart)
            return pOutBuf;
      }

      int iByteStart = 0;
      int iByteCount = 0;

      if (oPart.GetShowBodyHeader())
      {
         // Add HEADER
         AnsiString sHeaderContents = pBodyPart->GetHeaderContents();
         _GetBytesToSend(sHeaderContents.GetLength(), oPart, iByteStart, iByteCount);
         sHeaderContents.Mid(iByteStart, iByteCount);
         pOutBuf->Add((BYTE*) sHeaderContents.GetBuffer(0), sHeaderContents.GetLength());
      }      
      else if (oPart.GetShowBodyText())
      {
         // RFC: The TEXT part specifier refers to the text body of the message,
         // omitting the [RFC-2822] header.
         // Martin: All sub-bodies in raw format, except for the current header
         AnsiString body;
         pBodyPart->Store(body, false);


         //  Does this do anything since start is always 0 at this point?
         //  Assume this should be removed..
         //  body = body.Mid(iByteStart);

         _GetBytesToSend(body.GetLength(), oPart, iByteStart, iByteCount);

	 // Fix for Apple Mail BODY PEEK with start + size issue
	 // Start was being ignored before as it was never used
	 // Changed to mimic 4.4.4 method since iByteStart is set in _GetBytesToSend
         pOutBuf->Add((BYTE*) body.GetBuffer() + iByteStart, iByteCount);
      }
      else if (oPart.GetShowBodyFull())
      {
         /*
         BODY[]
         An empty section specification refers to the entire message, 
         including the header.

         RFC822 is
         Functionally equivalent to BODY[], differing in the syntax of
         the resulting untagged FETCH data (RFC822 is returned).
         */

         int iSize = FileUtilities::FileSize(messageFileName);
         _GetBytesToSend(iSize, oPart, iByteStart, iByteCount);

         BYTE *pBuf = new BYTE[iByteCount];
         FileUtilities::ReadFileToBuf(messageFileName, pBuf, iByteStart, iByteCount);
         pOutBuf->Add(pBuf, iByteCount);
         delete [] pBuf;
      }
      else if (oPart.GetShowBodyHeaderFields())
      {
         std::vector<String> vecHeaderFields = oPart.GetHeaderFields();
         std::vector<String>::iterator iterHeader = vecHeaderFields.begin();
         String sResponse;
         String sFields;

         while (iterHeader != vecHeaderFields.end())
         {
            String sHeaderField = (*iterHeader);
            String sValue;

            sValue = pBodyPart->GetRawFieldValue(sHeaderField);

            if (sValue.GetLength() > 0)
            {
               if (!sFields.IsEmpty())
                  sFields += " ";

               sFields += sHeaderField;

               sResponse += sHeaderField + ": " + sValue + "\r\n";

            }

            iterHeader++;
         }

         sResponse += "\r\n";

         _GetBytesToSend(sResponse.GetLength(), oPart, iByteStart, iByteCount);
         sResponse.Mid(iByteStart, iByteCount);

         AnsiString sAS = sResponse;
         pOutBuf->Add((BYTE*) sAS.GetBuffer(0), sAS.GetLength());

      }
      else if (oPart.GetShowBodyHeaderFieldsNOT())
      {
         std::vector<String> vecHeaderFieldsNOT = oPart.GetHeaderFieldsNOT();

         String sResponse;

         String sFields;

         vector<MimeField> lstFields = pBodyPart->Fields();
         vector<MimeField>::iterator iterHeader = lstFields.begin();

         while (iterHeader != lstFields.end())
         {
            MimeField oField = (*iterHeader);

            String sHeaderName = oField.GetName();

            // Should we send this header?
            vector<String>::iterator iterExclude = vecHeaderFieldsNOT.begin();
            bool bSendThisHeader = true;
            while (iterExclude != vecHeaderFieldsNOT.end())
            {
               if ((*iterExclude).CompareNoCase(sHeaderName) == 0)
               {
                  bSendThisHeader = false;
                  break;
               }
               iterExclude++;
            }

            // Check whether we should include this header.
            if (bSendThisHeader)
            {
               String sHeaderValue = oField.GetValue();

               if (sHeaderValue.GetLength() > 0)
               {
                  if (!sFields.IsEmpty())
                     sFields += " ";

                  sFields += sHeaderName;

                  sResponse += sHeaderName + ": " + sHeaderValue + "\r\n";

               }
            }

            iterHeader++;
         }


         sResponse += "\r\n";

         _GetBytesToSend(sResponse.GetLength(), oPart, iByteStart, iByteCount);
         sResponse.Mid(iByteStart,iByteCount);

         AnsiString sAS = sResponse;
         pOutBuf->Add((BYTE*) sAS.GetBuffer(0), sAS.GetLength());

      }

      return pOutBuf;
   }

   shared_ptr<MimeBody>
   IMAPFetch::_GetMessagePartByPartNo(shared_ptr<MimeBody> pBody, long iPartNo)
   {
      
      if (!pBody)
      {
         shared_ptr<MimeBody> pEmpty;
         return pEmpty;
      }
      
      // Fetch the first part of the message.
      shared_ptr<MimeBody> oPart = pBody->FindFirstPart();

      if (!oPart)
      {
         // This is a single part message. Should we send the first part?
         if (iPartNo == 1)
            return pBody;
      }

      int iCurIdx = 1;

      String sResponse;
      while (oPart)
      {
         if (iPartNo == iCurIdx)
            return oPart;

         iCurIdx++;
         oPart = pBody->FindNextPart();
      }      

      shared_ptr<MimeBody> pEmpty;
      return pEmpty;
   }

   String
   IMAPFetch::_CreateEnvelopeStructure(MimeHeader& oHeader)
   {
      // BEGIN ENVELOPE
      String sResult;
      sResult += "(";
      
      MimeField *pField = 0;

      {  // BEGIN DATE
         
         pField = oHeader.GetField("Date");
         String sDate;
         if (pField)
         {
            sDate = pField->GetValue();

            // Time for some ugly code. Outlook Express 6.0
            // doesn't seem to base64-decode the Date-part
            // of the envelope structure even though it should
            // do that according to the RFC. Since getting help
            // from MS is pretty much impossible, we clean
            // the date a bit so that it doesn't contain any
            // "unallowed" characters.
            //
            // Before this, the date may look like this:
            // Thu, 20 Jan 2005 10:58:55 -0200 (Horário brasileiro de verão)
            // Afterwards, it should be:
            // Thu, 20 Jan 2005 10:58:55 -0200

            int iStartPara = sDate.Find(_T("("));
            if (iStartPara >= 0)
               sDate = sDate.Mid(0, iStartPara-1);
      
            // Make sure no quotes are included in the date.

            sDate.Replace(_T("\""), _T(""));

            sDate = Charset::Encode(sDate);

            
         }

         sResult += "\"" + sDate + "\"";
      }  // END DATE

      {  // BEGIN SUBJECT

         pField = oHeader.GetField("Subject");
         String sSubject;
         if (pField)
            sSubject = Charset::GetIMAPEncoded(pField->GetValue());

         sResult += " \"" + sSubject +"\"";
      }  //END SUBJECT

      {  // BEGIN FROM
         pField = oHeader.GetField("From");
         String sFrom;
         if (pField)
            sFrom = pField->GetValue();

         if (sFrom.IsEmpty())
            sResult += " NIL";
         else
            sResult += " " + _CreateEmailStructure(sFrom);
      }  //END FROM

      {  // BEGIN SENDER
         pField = oHeader.GetField("Sender");
         if (!pField)
            pField = oHeader.GetField("From");

         String sSender;
         if (pField)
            sSender = pField->GetValue();

         if (sSender.IsEmpty())
            sResult += " NIL";
         else
            sResult += " " + _CreateEmailStructure(sSender);
      }  //END SENDER

      {  // BEGIN REPLY-TO

         String sReplyTo;

         pField = oHeader.GetField("Reply-To");
         if (!pField)
            pField = oHeader.GetField("From");
         
         if (pField)
            sReplyTo = pField->GetValue();
          
         if (sReplyTo.IsEmpty())
            sResult += " NIL";
         else
            sResult += " " + _CreateEmailStructure(sReplyTo);

      }  //END REPLY-TO

      {  // BEGIN TO
         String sTo;

         pField = oHeader.GetField("To");
         if (pField)
            sTo = pField->GetValue();

         if (sTo.IsEmpty())
            sResult += " NIL";
         else 
            sResult += " " +  _CreateEmailStructure(sTo);

      }  //END TO

      {  // BEGIN CC
         
         String sCC;

         pField = oHeader.GetField("CC");
         if (pField)
            sCC = pField->GetValue();

         if (sCC.IsEmpty())
            sResult += " NIL";
         else         
            sResult += " " + _CreateEmailStructure(sCC);

      }  //END CC

      {  // BEGIN BCC
         String sBCC;

         pField = oHeader.GetField("BCC");
         if (pField)
            sBCC = pField->GetValue();

         if (sBCC.IsEmpty())
            sResult += " NIL";
         else
            sResult += " " + _CreateEmailStructure(sBCC);

      }  //END BCC

      {  // BEGIN IN-REPLY-TO
         String sInReplyTo;

         pField = oHeader.GetField("IN-REPLY-TO");
         if (pField)
            sInReplyTo = Charset::Encode(pField->GetValue());

         if (sInReplyTo.IsEmpty())
            sResult += " NIL";
         else
            sResult += " \"" + sInReplyTo +"\"";

      }  //END IN-REPLY-TO

      {  // BEGIN MESSAGEID
         String sMessageID;

         pField = oHeader.GetField("Message-ID");
         if (pField)
            sMessageID = Charset::Encode(pField->GetValue());

         if (sMessageID.IsEmpty())
            sResult += " NIL";
         else
            sResult += " \"" + sMessageID +"\"";

      }  //END MESSAGEID

      // END ENVELOPE
      sResult += ")";

      return sResult;


   }

   String
   IMAPFetch::_IteratePartRecursive(shared_ptr<MimeBody> oPart, bool includeExtensionData, int iRecursion)
   {
      iRecursion++;

      if (iRecursion > 100)
         return "";

      if (oPart->IsMultiPart())
      {
         // This part has many parts. Iterate those.


         String sResult = "(";

         shared_ptr<MimeBody> pSubPart = oPart->FindFirstPart();

         while (pSubPart)
         {
            sResult += _IteratePartRecursive(pSubPart, includeExtensionData, iRecursion);

            pSubPart = oPart->FindNextPart();
         }

         // Add subtype stamp.
			String sType = oPart->GetSubType();
			sType.ToUpper();

			String sTemp;
			sTemp.Format(_T(" \"%s\""), sType);
			sResult += sTemp;

			if (includeExtensionData)
			{
				String sBoundary = oPart->GetBoundary();
				sTemp.Format(_T(" (\"BOUNDARY\" \"%s\") NIL NIL"), sBoundary);
				sResult += sTemp;
			}
         
         sResult += ")";

         return sResult;

      }
      else
      {
         return _GetPartStructure(oPart, includeExtensionData, iRecursion);
      }
   }

   String 
   IMAPFetch::_GetPartStructure(shared_ptr<MimeBody> oPart, bool includeExtensionData, int iRecursion)   
   {
      String sMainType = oPart->GetMainType();
      sMainType.ToUpper();
      String sSubType = oPart->GetSubType();
      sSubType.ToUpper();

      String sBodyParams;

      if (oPart->IsAttachment())
         sBodyParams.Format(_T("(\"NAME\" \"%s\")"), oPart->GetRawFilename());
      else
      {
			String sCharset = oPart->GetCharset();
			
			if (sCharset.IsEmpty())
         {
            // Fallback to default. One could think that the client
            // should assume this if we don't tell it, but at least
            // JavaMail doesn't handle an empty string here.
            sCharset = "US-ASCII";
         }

         sBodyParams.Format(_T("(\"CHARSET\" \"%s\")"), sCharset);
      }
      
      // Make sure that the subject is encoded. If the subject isn't
      // encoded, and it contains a ", it will not be parsed properly.
      MimeField *pField = oPart->GetField("Subject");
      String sSubject;
      if (pField)
         sSubject = Charset::GetIMAPEncoded(pField->GetValue());

      String sEncoding = oPart->GetTransferEncoding();
      String sContentID = oPart->GetRawFieldValue("Content-ID");

      String sSize;
      sSize.Format(_T("%d"), oPart->GetContentLength());

      String sResult;
      sResult += "(";
      sResult += "\"" + sMainType + "\"";
      sResult += " \"" + sSubType + "\"";
      sResult += " " + sBodyParams;
      
      if (sContentID.IsEmpty())
         sResult += " NIL";
      else
         sResult += " \"" + sContentID + "\"";
      
      if (sSubject.IsEmpty())
         sResult += " NIL";
      else
         sResult += " \"" + sSubject + "\"";

      // RFC 3501 states 7bit should have quotes
      // Missing quotes around 7bit causes problems with Thunderbird
      // http://www.hmailserver.com/forum/viewtopic.php?f=10&t=22887
      if (sEncoding.IsEmpty())
         sResult += " \"7bit\"";
      else
         sResult += " \"" + sEncoding + "\"";

      sResult += " " + sSize;

      if (oPart->IsEncapsulatedRFC822Message())
      {
         /*
            A body type of type MESSAGE and subtype RFC822 contains,
            immediately after the basic fields, the envelope structure,
            body structure, and size in text lines of the encapsulated
            message.
         */
      
         try
         {
            shared_ptr<MimeBody> pBody = oPart->LoadEncapsulatedMessage();

            AnsiString sMessageHeader = pBody->GetHeaderContents();
            MimeHeader oHeader;
            oHeader.Load(sMessageHeader, sMessageHeader.GetLength());
            
            String sEnvelope = _CreateEnvelopeStructure(oHeader);
            String sBodyStructure = _IteratePartRecursive(pBody, includeExtensionData, iRecursion);

            AnsiString contentString = (const char*) pBody->GetContent();

            long lLines = contentString.NumberOf("\n") + 1;

            String sLines;
            sLines.Format(_T("%d"), lLines);

            sResult += " " + sEnvelope;
            sResult += " " + sBodyStructure;
            sResult += " " + sLines;
         }
         catch (...)
         {
            String sErrorMessage = "Error when creating ENVELOPE structure for encapsulated message.";

            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5061, "IMAPFetch::_GetPartStructure", sErrorMessage);
            
            return "";
         }

      }

      if (sMainType.CompareNoCase(_T("TEXT")) == 0)
      {
         /*
         A body type of type TEXT contains, immediately
         after the basic fields, the size of the body in
         text lines.  Note that this size is the size in its
         content transfer encoding and not the resulting
         size after any decoding.
         */

         AnsiString sData = (const char*) oPart->GetContent();
         long lLines = sData.NumberOf("\n") + 1;

         String sTemp;
         sTemp.Format(_T(" %d"), lLines);

         sResult += sTemp;
      }


      if (oPart->IsAttachment())
      {
         String sDisposition = oPart->GetDisposition();
         int iSemiPos = sDisposition.Find(_T(";"));
         if (iSemiPos > 0)
            sDisposition = sDisposition.Mid(0, iSemiPos);

         // Fall back to default if content disposition
         // isn't specified for the part.
         if (sDisposition.IsEmpty())
            sDisposition = "ATTACHMENT";
    
         String sTemp;
         sTemp.Format(_T("NIL (\"%s\" (\"FILENAME\" \"%s\"))"), sDisposition, oPart->GetRawFilename());
         sResult += " " + sTemp;
      }

      sResult += ")";
      return sResult;
   }
   
   String 
   IMAPFetch::_CreateEmailStructure(const String &sField)
   {
      // The input will (hopefully) look like this:
      // "Martin Knafve" <martin@halvar.com>
      // The output should look like:
      // ((\"personal name\" NIL \"ey\" \"\"))
      // 

      if (sField.IsEmpty())
      {
         return "(\"\" NIL \"\" \"\")";
      }

      String sResult = "(";

      // First remove the newline characters.
      
      shared_ptr<AddresslistParser> pParser = shared_ptr<AddresslistParser>(new AddresslistParser());
      std::vector<shared_ptr<Address> > vecAddresses = pParser->ParseList(sField);
      std::vector<shared_ptr<Address> >::iterator iterElement = vecAddresses.begin();

      while (iterElement != vecAddresses.end())
      {
         String sName = (*iterElement)->sPersonalName;
         String sMailboxName = (*iterElement)->sMailboxName;
         String sDomainName = (*iterElement)->sDomainName;

         String sThisAddress;

         if (sName.IsEmpty() && sMailboxName.IsEmpty() && sDomainName.IsEmpty())
         {
            sThisAddress = "NIL";
            if (vecAddresses.size() == 1)
            {
               // Empty address specified. Strange, but according to RFC.
               return sThisAddress;
            }

         }
         else
         {
            String sOutName = Charset::GetIMAPEncoded(sName);
            String sOutMailboxName = Charset::GetIMAPEncoded(sMailboxName);
            String sOutDomainName = Charset::GetIMAPEncoded(sDomainName);

            sThisAddress.Format(_T("(\"%s\" NIL \"%s\" \"%s\")"), sOutName, sOutMailboxName, sOutDomainName);
         }

         if (sResult.GetLength() > 1)
            sResult += " ";

         sResult += sThisAddress;

         iterElement++;
      }

      sResult += ")";

      return sResult;

   }

   shared_ptr<MimeBody> 
   IMAPFetch::_LoadMimeBody(shared_ptr<IMAPFetchParser> pParser, const String &fileName)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Loads the part of a message needed. 
   //---------------------------------------------------------------------------()
   {
      shared_ptr<MimeBody> pMimeBody;
      
      if (pParser->GetPartsToLookAt().size() == 0 &&
          !pParser->GetShowBodyStructure() &&
          !pParser->GetShowBodyStructureNonExtensible())
      {
         // The message body is not needed.
         return pMimeBody;
      }

      // We should check whether we need to load the entire email
      // or if it's enough to only load the header. If the client
      // only requests part from the header, we should only load
      // just that.

      bool bLoadFullMail = _GetMessageBodyNeeded(m_pParser);

      try
      {
         pMimeBody = shared_ptr<MimeBody>(new MimeBody);

         if (bLoadFullMail)
            pMimeBody->LoadFromFile(fileName);
         else
         {
            // We should only load the header.
            AnsiString sHeader = PersistentMessage::LoadHeader(fileName);
            pMimeBody->MimeHeader::Load(sHeader, sHeader.GetLength());
         }

      }
      catch (...)
      {
         _ReportCriticalError(fileName, "ERROR HM10001 - IMAP FETCH: Error when loading MIME message.");
      }

      return pMimeBody;

   }

   bool 
   IMAPFetch::_GetMessageBodyNeeded(shared_ptr<IMAPFetchParser> pParser)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // This method determines whether we need to load the entire body, or if
   // the header contains enough information.
   //---------------------------------------------------------------------------()
   {  
      if (m_pParser->GetShowBodyStructure() ||
          m_pParser->GetShowBodyStructureNonExtensible())
         return true;

      // Check if we only got one part.
      std::vector<IMAPFetchParser::BodyPart> vecPartsToLookAt = m_pParser->GetPartsToLookAt();
      if (vecPartsToLookAt.size() == 1)
      {
         IMAPFetchParser::BodyPart oBodyPart = vecPartsToLookAt[0];

         if (!oBodyPart.GetName().IsEmpty())
         {
            // If we're loading something else than the first main part.
            return true;
         }

         if (!oBodyPart.GetBodyTextNeeded())
         {
            // We only need the header.
            return false;
         }
      }

      return true;
   }

   void
   IMAPFetch::_AppendOutput(String &sOutput, const String &sAppend)
   {
      if (m_bAppendSpace)
         sOutput += " ";

      sOutput += sAppend;

      m_bAppendSpace = true;
   }

   void
   IMAPFetch::_SendAndReset(shared_ptr<IMAPConnection> pConnection, String &sOutput)
   {
      pConnection->SendAsciiData(sOutput);
      sOutput = "";
   }

   void
   IMAPFetch::_ReportCriticalError(const String &messageFileName, const String &sMessage)
   // ----
   // Reports an error to the ERROR log and then throws an exception.
   // ---
   {
      String sErrorMessage = sMessage + " - " + messageFileName;
      ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5062, "IMAPFetch::_ReportCriticalError", sErrorMessage);
   }

}

