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
      append_space_(false)
   {
      
   }

   IMAPFetch::~IMAPFetch()
   {

   }

   IMAPResult
   IMAPFetch::DoAction(std::shared_ptr<IMAPConnection> pConnection, int messageIndex, std::shared_ptr<Message> pMessage, const std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pArgument || !pMessage)
         return IMAPResult(IMAPResult::ResultBad, "Invalid parameters");

      const String messageFileName = PersistentMessage::GetFileName(pConnection->GetAccount(), pMessage);

      append_space_ = false;

      // Parse the command
      if (!parser_)
      {
         parser_ = std::shared_ptr<IMAPFetchParser>(new IMAPFetchParser());
         String sTemp = pArgument->Command();
         parser_->ParseCommand(sTemp);
      }

      // If we're going to touch the file, make sure it's there.
      bool willReadMessageFile =
         parser_->GetShowEnvelope() ||
         parser_->GetShowBodyStructure() ||
         parser_->GetShowBodyStructureNonExtensible()||
         parser_->GetPartsToLookAt().size() > 0;

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
      if (parser_->GetShowUID() || GetIsUID()) 
      {
         String sUIDTag;
         sUIDTag.Format(_T("UID %u"), pMessage->GetUID());
         
         AppendOutput_(sOutput, sUIDTag);
      }

      if (parser_->GetShowRFCSize())
      {
         String sTemp;
         sTemp.Format(_T("RFC822.SIZE %d"), pMessage->GetSize());

         AppendOutput_(sOutput, sTemp);
      }

      if (parser_->GetShowFlags())
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
            
         auto iter = setFlags.begin();
         auto iterEnd = setFlags.end();

         for (; iter != iterEnd; iter++)
         {
            if (iter != setFlags.begin())
               sTemp += " ";

            sTemp += "\\" + (*iter);
         }

         sTemp += ")";

         AppendOutput_(sOutput, sTemp);
      }


      if (parser_->GetShowInternalDate())
      {
         String sTemp = "INTERNALDATE \"";

         String sCreateTime = pMessage->GetCreateTime();
         
         if (!Time::SeemsToBeValidYear(sCreateTime))
            sCreateTime = Time::GetCurrentDateTime();

         String sMIMECreateTime = Time::GetIMAPInternalDate(sCreateTime);

         sTemp += sMIMECreateTime + "\"";

         AppendOutput_(sOutput, sTemp);
      }

      
      AnsiString sMessageHeader;

      if (parser_->GetShowEnvelope())
      {
         // Parse the MIME header
         if (sMessageHeader.IsEmpty())
         {
            sMessageHeader = PersistentMessage::LoadHeader(messageFileName);

            oMimeHeader.Load(sMessageHeader, sMessageHeader.length());
         }

         String sTemp;
         sTemp = "ENVELOPE " + CreateEnvelopeStructure_(oMimeHeader);
         AppendOutput_(sOutput, sTemp);
         
      }



      // Sometimes we need to load the entire mail into memory.
      // If the user want's to download a specific attachment or
      // if he wants to look at the structure of the mime message.
      std::shared_ptr<MimeBody> pMimeBody = LoadMimeBody_(parser_, messageFileName);
      
      if (parser_->GetShowBodyStructure() ||
          parser_->GetShowBodyStructureNonExtensible())
      {
         String sResult = "";
         if (parser_->GetShowBodyStructure())
            sResult = "BODYSTRUCTURE " + IteratePartRecursive_(pMimeBody, true, 0);
         else
            sResult = "BODY " + IteratePartRecursive_(pMimeBody, false, 0);

         AppendOutput_(sOutput, sResult);
      }

      std::vector<IMAPFetchParser::BodyPart> vecPartsToPeekAt = parser_->GetPartsToLookAt();
      
      if (vecPartsToPeekAt.size() > 0)
      {
         auto iter = vecPartsToPeekAt.begin();

         // First we should only print the part sizes..
         while (iter != vecPartsToPeekAt.end())   
         {
            IMAPFetchParser::BodyPart oPart = (*iter);

            int iOctetStart = oPart.octet_start_;
            int iOctetCount = oPart.octet_count_;
           
            const String messageFileName = PersistentMessage::GetFileName(pConnection->GetAccount(), pMessage);
            std::shared_ptr<ByteBuffer> pBuffer = GetByteBufferByBodyPart_(messageFileName, pMimeBody, oPart);
            
            String sPartIdentifier;
            if (iOctetStart == -1 && iOctetCount == -1)
               sPartIdentifier.Format(_T("%s"), oPart.GetDescription().c_str());
            else
               sPartIdentifier.Format(_T("%s<%d>"), oPart.GetDescription().c_str(), iOctetStart, iOctetCount);

            if (pBuffer->GetSize() > 0)
            {
               // Send part size information
               String sTemp;
               sTemp.Format(_T("%s {%d}\r\n"), sPartIdentifier.c_str(), pBuffer->GetSize()); // Add 2 for trailing newline.
               
               AppendOutput_(sOutput, sTemp);
               SendAndReset_(pConnection, sOutput);

               // Send the actual part
               pConnection->EnqueueWrite(pBuffer);
            }
            else
            {
               // The client requested a part that does not exist.
               // Just give the client an empty string for that part.
               String sTemp;
               sTemp.Format(_T("%s \"\""), sPartIdentifier.c_str());
               AppendOutput_(sOutput, sTemp);
            }

            iter++;
         }
      }
     
      if (parser_->GetSetSeenFlag() && !pConnection->GetCurrentFolderReadOnly())
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
      SendAndReset_(pConnection, sOutput);

      return IMAPResult();
   }

   void
   IMAPFetch::GetBytesToSend_(int iBufferSize, IMAPFetchParser::BodyPart &oPart, int &iOutStart, int &iOutCount)
   {
      int iOctetStart = oPart.octet_start_;
      int iOctetCount = oPart.octet_count_;

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

   std::shared_ptr<MimeBody> 
   IMAPFetch::GetBodyPartByRecursiveIdentifier_(std::shared_ptr<MimeBody> pBody, const String &sName)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns a body part by a given identifier. An identifier can be 1, 2, 1.2 etc.
   //---------------------------------------------------------------------------()
   {
      if (!pBody || sName.IsEmpty())
      {
         std::shared_ptr<MimeBody> pEmpty;
         return pEmpty;
      }

      String sTempName = sName;


      // First split the string from 1, 2.1 and so on.
      std::vector<String> vecPartPath = StringParser::SplitString(sTempName, ".");

      // Iterate over the vector to find the part.
      auto iterPart = vecPartPath.begin();

      while (iterPart != vecPartPath.end())
      {
         String sSubPart = *iterPart;

         if (StringParser::IsNumeric(sSubPart))
         {
            int iRequestPartNo = _ttoi(sSubPart);

            pBody = GetMessagePartByPartNo_(pBody, iRequestPartNo);

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

                  ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5060, "IMAPFetch::GetBodyPartByRecursiveIdentifier_", sErrorMessage);

                  std::shared_ptr<MimeBody> empty;
                  return empty;
               }
            }

         }
     
         iterPart++;
      }

      return pBody;
   }


   std::shared_ptr<ByteBuffer> 
   IMAPFetch::GetByteBufferByBodyPart_(const String &messageFileName, std::shared_ptr<MimeBody> pBodyPart, IMAPFetchParser::BodyPart &oPart)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns a buffer containing the data for the given body/part
   //---------------------------------------------------------------------------()
   {
      std::shared_ptr<ByteBuffer> pOutBuf = std::shared_ptr<ByteBuffer>(new ByteBuffer);

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
         pBodyPart  = GetBodyPartByRecursiveIdentifier_(pBodyPart, oPart.GetName());

         if (!pBodyPart)
            return pOutBuf;
      }

      int iByteStart = 0;
      int iByteCount = 0;

      if (oPart.GetShowBodyHeader())
      {
         // Add HEADER
         AnsiString sHeaderContents = pBodyPart->GetHeaderContents();
         GetBytesToSend_(sHeaderContents.GetLength(), oPart, iByteStart, iByteCount);
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

         GetBytesToSend_(body.GetLength(), oPart, iByteStart, iByteCount);

	 // Fix for Apple Mail BODY PEEK with start + size issue
	 // Start was being ignored before as it was never used
	 // Changed to mimic 4.4.4 method since iByteStart is set in GetBytesToSend_
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
         GetBytesToSend_(iSize, oPart, iByteStart, iByteCount);

         BYTE *pBuf = new BYTE[iByteCount];
         FileUtilities::ReadFileToBuf(messageFileName, pBuf, iByteStart, iByteCount);
         pOutBuf->Add(pBuf, iByteCount);
         delete [] pBuf;
      }
      else if (oPart.GetShowBodyHeaderFields())
      {
         std::set<String> included_fields;
         std::vector<String> vecHeaderFields = oPart.GetHeaderFields();
         auto iterHeader = vecHeaderFields.begin();
         String sResponse;
         String sFields;

         while (iterHeader != vecHeaderFields.end())
         {
            String sHeaderField = (*iterHeader);

            String header_field_uppercase = sHeaderField; 
            header_field_uppercase.MakeUpper();

            if (included_fields.find(header_field_uppercase) == included_fields.end())
            {
               String sValue;

               sValue = pBodyPart->GetRawFieldValue(sHeaderField);

               if (sValue.GetLength() > 0)
               {
                  if (!sFields.IsEmpty())
                     sFields += " ";

                  sFields += sHeaderField;

                  sResponse += sHeaderField + ": " + sValue + "\r\n";
               }

               included_fields.insert(header_field_uppercase);
            }

            iterHeader++;
         }

         sResponse += "\r\n";

         GetBytesToSend_(sResponse.GetLength(), oPart, iByteStart, iByteCount);
         sResponse.Mid(iByteStart, iByteCount);

         AnsiString sAS = sResponse;
         pOutBuf->Add((BYTE*) sAS.GetBuffer(0), sAS.GetLength());

      }
      else if (oPart.GetShowBodyHeaderFieldsNOT())
      {
         std::vector<String> vecHeaderFieldsNOT = oPart.GetHeaderFieldsNOT();

         String sResponse;

         String sFields;

         std::vector<MimeField> lstFields = pBodyPart->Fields();
         auto iterHeader = lstFields.begin();

         while (iterHeader != lstFields.end())
         {
            MimeField oField = (*iterHeader);

            String sHeaderName = oField.GetName();

            // Should we send this header?
            auto iterExclude = vecHeaderFieldsNOT.begin();
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

         GetBytesToSend_(sResponse.GetLength(), oPart, iByteStart, iByteCount);
         sResponse.Mid(iByteStart,iByteCount);

         AnsiString sAS = sResponse;
         pOutBuf->Add((BYTE*) sAS.GetBuffer(0), sAS.GetLength());

      }

      return pOutBuf;
   }

   std::shared_ptr<MimeBody>
   IMAPFetch::GetMessagePartByPartNo_(std::shared_ptr<MimeBody> pBody, long iPartNo)
   {
      
      if (!pBody)
      {
         std::shared_ptr<MimeBody> pEmpty;
         return pEmpty;
      }
      
      // Fetch the first part of the message.
      std::shared_ptr<MimeBody> oPart = pBody->FindFirstPart();

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

      std::shared_ptr<MimeBody> pEmpty;
      return pEmpty;
   }

   String
   IMAPFetch::CreateEnvelopeStructure_(MimeHeader& oHeader)
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
            // Thu, 20 Jan 2005 10:58:55 -0200 (Hor�rio brasileiro de ver�o)
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
            sResult += " " + CreateEmailStructure_(sFrom);
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
            sResult += " " + CreateEmailStructure_(sSender);
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
            sResult += " " + CreateEmailStructure_(sReplyTo);

      }  //END REPLY-TO

      {  // BEGIN TO
         String sTo;

         pField = oHeader.GetField("To");
         if (pField)
            sTo = pField->GetValue();

         if (sTo.IsEmpty())
            sResult += " NIL";
         else 
            sResult += " " +  CreateEmailStructure_(sTo);

      }  //END TO

      {  // BEGIN CC
         
         String sCC;

         pField = oHeader.GetField("CC");
         if (pField)
            sCC = pField->GetValue();

         if (sCC.IsEmpty())
            sResult += " NIL";
         else         
            sResult += " " + CreateEmailStructure_(sCC);

      }  //END CC

      {  // BEGIN BCC
         String sBCC;

         pField = oHeader.GetField("BCC");
         if (pField)
            sBCC = pField->GetValue();

         if (sBCC.IsEmpty())
            sResult += " NIL";
         else
            sResult += " " + CreateEmailStructure_(sBCC);

      }  //END BCC

      {  // BEGIN IN-REPLY-TO
         String sInReplyTo;

         pField = oHeader.GetField("IN-REPLY-TO");
         if (pField)
            sInReplyTo = Charset::GetIMAPEncoded(pField->GetValue());

         if (sInReplyTo.IsEmpty())
            sResult += " NIL";
         else
            sResult += " \"" + sInReplyTo +"\"";

      }  //END IN-REPLY-TO

      {  // BEGIN MESSAGEID
         String sMessageID;

         pField = oHeader.GetField("Message-ID");
         if (pField)
            sMessageID = Charset::GetIMAPEncoded(pField->GetValue());

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
   IMAPFetch::IteratePartRecursive_(std::shared_ptr<MimeBody> oPart, bool includeExtensionData, int iRecursion)
   {
      iRecursion++;

      if (iRecursion > 100)
         return "";

      if (oPart->IsMultiPart())
      {
         // This part has many parts. Iterate those.


         String sResult = "(";

         std::shared_ptr<MimeBody> pSubPart = oPart->FindFirstPart();

         while (pSubPart)
         {
            sResult += IteratePartRecursive_(pSubPart, includeExtensionData, iRecursion);

            pSubPart = oPart->FindNextPart();
         }

         // Add subtype stamp.
			String sType = oPart->GetSubType();
			sType.ToUpper();

			String sTemp;
         sTemp.Format(_T(" \"%s\""), sType.c_str());
			sResult += sTemp;

			if (includeExtensionData)
			{
				String sBoundary = oPart->GetBoundary();
            sTemp.Format(_T(" (\"BOUNDARY\" \"%s\") NIL NIL"), sBoundary.c_str());
				sResult += sTemp;
			}
         
         sResult += ")";

         return sResult;

      }
      else
      {
         return GetPartStructure_(oPart, includeExtensionData, iRecursion);
      }
   }

   String 
   IMAPFetch::GetPartStructure_(std::shared_ptr<MimeBody> oPart, bool includeExtensionData, int iRecursion)   
   {
      String sMainType = oPart->GetMainType();
      sMainType.ToUpper();
      String sSubType = oPart->GetSubType();
      sSubType.ToUpper();

      String sBodyParams;

      if (oPart->IsAttachment())
         sBodyParams.Format(_T("(\"NAME\" \"%s\")"), oPart->GetRawFilename().c_str());
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

         sBodyParams.Format(_T("(\"CHARSET\" \"%s\")"), sCharset.c_str());
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
            std::shared_ptr<MimeBody> pBody = oPart->LoadEncapsulatedMessage();

            AnsiString sMessageHeader = pBody->GetHeaderContents();
            MimeHeader oHeader;
            oHeader.Load(sMessageHeader, sMessageHeader.GetLength());
            
            String sEnvelope = CreateEnvelopeStructure_(oHeader);
            String sBodyStructure = IteratePartRecursive_(pBody, includeExtensionData, iRecursion);

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

            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5061, "IMAPFetch::GetPartStructure_", sErrorMessage);
            
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
         sTemp.Format(_T("NIL (\"%s\" (\"FILENAME\" \"%s\"))"), sDisposition.c_str(), oPart->GetRawFilename().c_str());
         sResult += " " + sTemp;
      }

      sResult += ")";
      return sResult;
   }
   
   String 
   IMAPFetch::CreateEmailStructure_(const String &sField)
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
      
      std::shared_ptr<AddresslistParser> pParser = std::shared_ptr<AddresslistParser>(new AddresslistParser());
      std::vector<std::shared_ptr<Address> > vecAddresses = pParser->ParseList(sField);
      auto iterElement = vecAddresses.begin();

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

            sThisAddress.Format(_T("(\"%s\" NIL \"%s\" \"%s\")"), sOutName.c_str(), sOutMailboxName.c_str(), sOutDomainName.c_str());
         }

         if (sResult.GetLength() > 1)
            sResult += " ";

         sResult += sThisAddress;

         iterElement++;
      }

      sResult += ")";

      return sResult;

   }

   std::shared_ptr<MimeBody> 
   IMAPFetch::LoadMimeBody_(std::shared_ptr<IMAPFetchParser> pParser, const String &fileName)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Loads the part of a message needed. 
   //---------------------------------------------------------------------------()
   {
      std::shared_ptr<MimeBody> pMimeBody;
      
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

      bool bLoadFullMail = GetMessageBodyNeeded_(parser_);

      try
      {
         pMimeBody = std::shared_ptr<MimeBody>(new MimeBody);

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
         ReportCriticalError_(fileName, "ERROR HM10001 - IMAP FETCH: Error when loading MIME message.");
      }

      return pMimeBody;

   }

   bool 
   IMAPFetch::GetMessageBodyNeeded_(std::shared_ptr<IMAPFetchParser> pParser)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // This method determines whether we need to load the entire body, or if
   // the header contains enough information.
   //---------------------------------------------------------------------------()
   {  
      if (parser_->GetShowBodyStructure() ||
          parser_->GetShowBodyStructureNonExtensible())
         return true;

      // Check if we only got one part.
      std::vector<IMAPFetchParser::BodyPart> vecPartsToLookAt = parser_->GetPartsToLookAt();
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
   IMAPFetch::AppendOutput_(String &sOutput, const String &sAppend)
   {
      if (append_space_)
         sOutput += " ";

      sOutput += sAppend;

      append_space_ = true;
   }

   void
   IMAPFetch::SendAndReset_(std::shared_ptr<IMAPConnection> pConnection, String &sOutput)
   {
      pConnection->SendAsciiData(sOutput);
      sOutput = "";
   }

   void
   IMAPFetch::ReportCriticalError_(const String &messageFileName, const String &sMessage)
   // ----
   // Reports an error to the ERROR log and then throws an exception.
   // ---
   {
      String sErrorMessage = sMessage + " - " + messageFileName;
      ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5062, "IMAPFetch::ReportCriticalError_", sErrorMessage);
   }

}

