// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "MessageData.h"

#include "../Mime/Mime.h"
#include "../Mime/MimeCode.h"
#include "../BO/Message.h"
#include "../BO/Attachments.h"
#include "../Util/Time.h"
#include "../Util/GUIDCreator.h"
#include "../Util/Utilities.h"

// Test
#include "../Persistence/PersistentMessage.h"
#include "../../SMTP/RecipientParser.h"

#define XHMAILSERVER_LOOPCOUNT _T("X-hMailServer-LoopCount")

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MessageData::MessageData()
   {
      m_bEncodeFields = true;
      _unfoldWithSpace = true;

      m_pMimeMail = shared_ptr<MimeBody>(new MimeBody);
   }

   bool
   MessageData::LoadFromMessage(shared_ptr<const Account> account, shared_ptr<Message> pMessage)
   {
      String fileName = PersistentMessage::GetFileName(account, pMessage);

      return LoadFromMessage(fileName, pMessage);
   }
   

   bool
   MessageData::LoadFromMessage(const String &fileName, shared_ptr<Message> pMessage)
   {
      m_pMessage = pMessage;
      _messageFileName = fileName;

      m_pMimeMail = shared_ptr<MimeBody>(new MimeBody);

      const int MaxSize = 1024*1024 * 80; // we'll ignore messages larger than 80MB.
      if (FileUtilities::FileSize(_messageFileName) > MaxSize)
         return false;

      bool bNewMessage = false;
      try
      {

         if (!m_pMimeMail->LoadFromFile(_messageFileName))
         {
            bNewMessage = true;
         }
      }
      catch (...)
      {
         try
         {
            String sFileNameExclPath = FileUtilities::GetFileNameFromFullPath(_messageFileName);

            String sMessageBackupPath = IniFileSettings::Instance()->GetLogDirectory() + "\\Problematic messages\\" + sFileNameExclPath;
            FileUtilities::Copy(_messageFileName, sMessageBackupPath, true);

            String sErrorMessage;
            sErrorMessage.Format(_T("An unknown error occurred while loading message. File: %s. Backuped to: %s"), _messageFileName, sMessageBackupPath); 

            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4218, "MessageData::LoadFromMessage", sErrorMessage);
         }
         catch (...)
         {
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4218, "MessageData::LoadFromMessage", "An unknown error occurred while loading message.");
         }

         return false;
      }

      if (bNewMessage)
      {
         // For new messages, we default to UTF-8. This way client
         // can put any values into headers without having to care
         // about setting the correct character set first.
         SetCharset("utf-8");
         SetFieldValue(CMimeConst::MimeVersion(), "1.0");
      }

      return true;
   }


   bool 
   MessageData::RefreshFromMessage()
   {
      if (!m_pMessage)
         return false;

      return LoadFromMessage(_messageFileName, m_pMessage);
   }


   MessageData::~MessageData()
   {

   }

   void 
   MessageData::DeleteField(const AnsiString &headerName)
   {
      MimeField *pField = m_pMimeMail->GetField(headerName);
      
      while (pField)
      {
         m_pMimeMail->DeleteField(pField);
         pField = m_pMimeMail->GetField(headerName);
      }
   }

   String
   MessageData::GetSubject()  const
   {
      return GetFieldValue("Subject");
   }

   void 
   MessageData::SetSubject(const String &sSubject)
   {
      SetFieldValue("Subject", sSubject);
   }

   String
   MessageData::GetCharset()  const
   {
      return m_pMimeMail->GetCharset();
   }

   void 
   MessageData::SetCharset(const String &sCharset)
   {
      AnsiString sCharsetStr = sCharset;
      m_pMimeMail->SetCharset(sCharsetStr);
   }

   String
   MessageData::GetHeader()  const
   {
      if (m_bEncodeFields)
         return m_pMimeMail->GetUnicodeHeaderContents();
      else
         return m_pMimeMail->GetHeaderContents();
   }

   String
   MessageData::GetSentTime()  const
   {
      return GetFieldValue("Date");
   }   

   void
   MessageData::SetSentTime(const String &sSentTime)
   {
      String sValue = sSentTime;
      if (sValue.IsEmpty())
      {
         // Use default value
         sValue = Time::GetCurrentMimeDate();
      }

      SetFieldValue("Date", sValue);
   }

   void 
   MessageData::SetFieldValue(const String &sField, const String &sValue)
   {
      if (m_bEncodeFields)
         m_pMimeMail->SetUnicodeFieldValue(sField, sValue, "");
      else
         m_pMimeMail->SetRawFieldValue(sField, sValue, "");
   }

   String
   MessageData::GetTo() const
   {
      return GetFieldValue("To");
   } 

   String
   MessageData::GetReturnPath() const
   {
      return GetFieldValue("Return-Path");
   } 

   void 
   MessageData::SetReturnPath(const String &sReturnPath)
   {
      SetFieldValue("Return-Path", "<" + sReturnPath + ">");
   }

   String
   MessageData::GetCC() const
   {
      return GetFieldValue("CC");
   } 

   void 
   MessageData::SetCC(const String &sCC)
   {
      SetFieldValue("CC", sCC);
   }

   String
   MessageData::GetBCC() const
   {
      return GetFieldValue("BCC");
   } 

   void 
   MessageData::SetBCC(const String &sBCC)
   {
      SetFieldValue("BCC", sBCC);
   }

   String
   MessageData::GetFrom() const
   {
      return GetFieldValue("From");
   } 

   void 
   MessageData::SetFrom(const String &sFrom)
   {
      SetFieldValue("From", sFrom);
   }

   void 
   MessageData::SetTo(const String &sTo)
   {
      SetFieldValue("To", sTo);
   }


   String
   MessageData::GetFieldValue(const String &sName) const
   {
      String sRetVal;
      if (m_bEncodeFields)
         sRetVal = m_pMimeMail->GetUnicodeFieldValue(sName);
      else
         sRetVal = m_pMimeMail->GetRawFieldValue(sName);

      return sRetVal;
   }

   int 
   MessageData::GetSize() const
   {
      return m_pMessage->GetSize();
   }

   shared_ptr<Attachments>
   MessageData::GetAttachments()
   {
      if (!m_pAttachments)
      {
         m_pAttachments = shared_ptr<Attachments>(new Attachments(m_pMimeMail, this));
         
         // Load attachments.
         m_pAttachments->Load();
      }

      return m_pAttachments;
   }

   String 
   MessageData::GetBody() const
   {
      shared_ptr<MimeBody> pPart = FindPart("text/plain");

      if (pPart)
      {
         if (m_bEncodeFields)
            return pPart->GetUnicodeText();
         else
            return pPart->GetRawText();
      }
      else
         return "";
    
   }

   void
   MessageData::SetBody(const String &sBody)
   {
      shared_ptr<MimeBody> pPart = FindPart("text/plain");

      if (!pPart)
         pPart = CreatePart("text/plain");

      String sModifiedBody = sBody;
      if (sModifiedBody.Right(2) != _T("\r\n"))
      {
         // Add carriage return.
         sModifiedBody += "\r\n";
      }

      // Set the text to the part
      if (m_bEncodeFields)
         pPart->SetUnicodeText(sModifiedBody);
      else
         pPart->SetRawText(sModifiedBody);
   }

   String 
   MessageData::GetHTMLBody() const
   {
      shared_ptr<MimeBody> pPart = FindPart("text/html");

      if (pPart)
      {
         if (m_bEncodeFields)
            return pPart->GetUnicodeText();
         else
            return pPart->GetRawText();
      }

      return "";
   }

   void
   MessageData::SetHTMLBody(const String &sNewVal)
   {
      shared_ptr<MimeBody> pHTMLPart = FindPart("text/html");

      if (!pHTMLPart)
      {
         // Create a new part.
         pHTMLPart = CreatePart("text/html");
      }

      String sModifiedBody = sNewVal;
      if (sModifiedBody.Right(2) != _T("\r\n"))
      {
         // Add carriage return.
         sModifiedBody += "\r\n";
      }

      // Set the text to the part
      if (m_bEncodeFields)
         pHTMLPart->SetUnicodeText(sModifiedBody);
      else
         pHTMLPart->SetRawText(sModifiedBody);
   }

   shared_ptr<MimeBody> 
   MessageData::FindPartNoRecurse(shared_ptr<MimeBody> parent, const AnsiString &sType) const
   {
      shared_ptr<MimeBody> pPart = parent->FindFirstPart();

      while (pPart)
      {
         AnsiString sContentType = pPart->GetCleanContentType();
         sContentType.MakeLower();

         if (sContentType.CompareNoCase(sType) == 0)
         {
            // Create a new part in the end of the message. We have already
            // looked for a part with the proper type without success. This
            // is probably a new attachment.
            return pPart;
         }

         pPart = parent->FindNextPart();
      }
      
      shared_ptr<MimeBody> empty;
      return empty;
   }

   shared_ptr<MimeBody> 
   MessageData::CreatePart(const String &sContentType)
   {
      // Step 1: Extract all parts.
      // Step 2: Delete everything
      // Step 3: Create the new type.
      // Step 4: Insert the new type and all others.

      // Create a new part by rebuilding the message more or less from scratch.
      AnsiString sMainBodyType = m_pMimeMail->GetCleanContentType();
      AnsiString sMainBodyCharset = m_pMimeMail->GetCharset();
      sMainBodyType.MakeLower();
      
      shared_ptr<MimeBody> textPart = FindPartNoRecurse(m_pMimeMail, "text/plain");
      shared_ptr<MimeBody> htmlPart = FindPartNoRecurse(m_pMimeMail, "text/html");

      shared_ptr<MimeBody> retValue;

      shared_ptr<MimeBody> alternativeNode = FindPartNoRecurse(m_pMimeMail, "multipart/alternative");
      if (alternativeNode)
      {
         if (!textPart) 
         {
            textPart = FindPartNoRecurse(alternativeNode, "text/plain");
            if (textPart)
               alternativeNode->ErasePart(textPart);
         }

         if (!htmlPart)
         {
            htmlPart = FindPartNoRecurse(alternativeNode, "text/html");

            if (htmlPart)
               alternativeNode->ErasePart(htmlPart);
         }

         m_pMimeMail->ErasePart(alternativeNode);
      }

      if (!textPart && !htmlPart)
      {
         // We don't have any text or HMTL part. Copy the main content
         // of the message to a new part, if the main content isn't empty.
         if (sMainBodyType == "" || sMainBodyType == "text/plain")
         {
            if (m_pMimeMail->GetRawText().size() > 0)
            {
               textPart = shared_ptr<MimeBody>(new MimeBody);
               textPart->SetRawText(m_pMimeMail->GetRawText());
               textPart->SetContentType("text/plain", "");
               
               if (!sMainBodyCharset.IsEmpty())
                  textPart->SetCharset(sMainBodyCharset);

               AnsiString originalTransferEncoding = m_pMimeMail->GetTransferEncoding();
               if (!originalTransferEncoding.IsEmpty())
                  textPart->SetTransferEncoding(originalTransferEncoding);
            }
         }
         else if (sMainBodyType == "text/html")
         {
            if (m_pMimeMail->GetRawText().size() > 0)
            {
               htmlPart = shared_ptr<MimeBody>(new MimeBody);
               htmlPart->SetRawText(m_pMimeMail->GetRawText());
               htmlPart->SetContentType("text/html", "");
               
               if (!sMainBodyCharset.IsEmpty())
                  htmlPart->SetCharset(sMainBodyCharset);

               AnsiString originalTransferEncoding = m_pMimeMail->GetTransferEncoding();
               if (!originalTransferEncoding.IsEmpty())
                  htmlPart->SetTransferEncoding(originalTransferEncoding);
            }
         }
      }

      // Locate the other parts which are not text or html.
      //
      // When we get here, any alternative, text or html parts
      // should have been removed from the message already.
      //
      shared_ptr<MimeBody> part = m_pMimeMail->FindFirstPart();
      set<shared_ptr<MimeBody> > setAttachments;
      while (part)
      {
         AnsiString subContentType = part->GetCleanContentType();
         if (!IsTextType(subContentType) && !IsHTMLType(subContentType))
            setAttachments.insert(part);

         part = m_pMimeMail->FindNextPart();
      }

      // Remove all parts so that we can rebuild it again.
      m_pMimeMail->DeleteAll();

      // Create the brand new part...
      if (sContentType.CompareNoCase(_T("text/plain")) == 0)
      {
         assert (textPart == 0);

         if (setAttachments.size() == 0 && !htmlPart)
         {
            // Reuse the main part. There's no need to add a new one.
            textPart = m_pMimeMail;
            textPart->SetContentType("text/plain", "");
         }
         else
         {
            textPart = shared_ptr<MimeBody>(new MimeBody);
            textPart->SetContentType("text/plain", "");

            AnsiString transferEncoding = m_pMimeMail->GetTransferEncoding();
            if (!transferEncoding.IsEmpty())
               textPart->SetTransferEncoding(transferEncoding);

            if (!sMainBodyCharset.IsEmpty())
               textPart->SetCharset(sMainBodyCharset);

         }

         retValue = textPart;
      }
      else if (sContentType.CompareNoCase(_T("text/html")) == 0)
      {
         assert (htmlPart == 0);

         if (setAttachments.size() == 0 && !textPart)
         {
            // Reuse the main part. There's no need to add a new one.

            htmlPart = m_pMimeMail;
            htmlPart->SetContentType("text/html", "");
         }
         else
         {
            htmlPart = shared_ptr<MimeBody>(new MimeBody);
            htmlPart->SetContentType("text/html", "");

            AnsiString transferEncoding = m_pMimeMail->GetTransferEncoding();
            if (!transferEncoding.IsEmpty())
               htmlPart->SetTransferEncoding(transferEncoding);

            if (!sMainBodyCharset.IsEmpty())
               htmlPart->SetCharset(sMainBodyCharset);
         }

         retValue = htmlPart;
         
      }
      else
      {
         // create a new item. treat as an attachment.
         retValue = shared_ptr<MimeBody>(new MimeBody);
         setAttachments.insert(retValue);
      }

      AnsiString mainBodyType;
      if (setAttachments.size() > 0)
         mainBodyType = "multipart/mixed";
      else if (textPart && htmlPart)
         mainBodyType = "multipart/alternative";
      else if (htmlPart)
         mainBodyType = "text/html";
      else
         mainBodyType = "text/plain";

      if (textPart && htmlPart)
      {
         if (mainBodyType == "multipart/mixed")
         {
            shared_ptr<MimeBody> alternativePart = shared_ptr<MimeBody>(new MimeBody);
            alternativePart->SetContentType("multipart/alternative", "");
            alternativePart->SetRawText("This is a multi-part message.\r\n\r\n");

            alternativePart->AddPart(textPart);
            alternativePart->AddPart(htmlPart);
            alternativePart->SetBoundary(NULL);

            m_pMimeMail->AddPart(alternativePart);
         }
         else
         {
            if (m_pMimeMail != textPart)
               m_pMimeMail->AddPart(textPart);

            if (m_pMimeMail != htmlPart)
               m_pMimeMail->AddPart(htmlPart);
         }
         
      }
      else if (textPart)
      {
         if (mainBodyType == "multipart/mixed")
         {
            if (m_pMimeMail != textPart)
               m_pMimeMail->AddPart(textPart);
         }
      }
      else if (htmlPart)
      {
         if (mainBodyType == "multipart/mixed")
         {
            if (m_pMimeMail != htmlPart)
               m_pMimeMail->AddPart(htmlPart);
         }
      }

      boost_foreach(shared_ptr<MimeBody> pAttachment, setAttachments)
      {
         m_pMimeMail->AddPart(pAttachment);
      }

      m_pMimeMail->SetContentType(mainBodyType, ""); 
      if (!sMainBodyCharset.IsEmpty())
         m_pMimeMail->SetCharset(sMainBodyCharset);

      if (m_pMimeMail->GetPartCount() > 0)
      {
         m_pMimeMail->DeleteField(CMimeConst::TransferEncoding());
         m_pMimeMail->SetRawText("This is a multi-part message.\r\n\r\n");
         m_pMimeMail->SetBoundary(NULL);
      }

      return retValue;
   }

   shared_ptr<MimeBody>
   MessageData::FindPart(const String &sType) const
   {
      String sPartType = m_pMimeMail->GetCleanContentType();

      if (sPartType.CompareNoCase(sType) == 0)
         return m_pMimeMail;

      shared_ptr<MimeBody> pPart = m_pMimeMail->FindFirstPart();

      if (!pPart)
      {
         shared_ptr<MimeBody> pEmpty;
         return pEmpty;
      }

      while (pPart)
      {
         String sContentType = pPart->GetCleanContentType();

         if (sContentType.CompareNoCase(sType) == 0)
            return pPart;
         
         if (pPart->IsMultiPart())
         {
            shared_ptr<MimeBody> pSubPart = pPart->FindFirstPart();

            while (pSubPart)
            {
               String sSubContentType = pSubPart->GetCleanContentType();
               
               if (sSubContentType.CompareNoCase(sType) == 0)
                  return pSubPart;

            
               pSubPart = pPart->FindNextPart();
            }

         }
         
         pPart = m_pMimeMail->FindNextPart();
      }

      shared_ptr<MimeBody> pEmpty;
      return pEmpty;     

   }

   bool 
   MessageData::Write(const String &fileName)
   {
      const HM::String directoryName = HM::FileUtilities::GetFilePath(fileName);
      if (!HM::FileUtilities::Exists(directoryName))
         HM::FileUtilities::CreateDirectoryRecursive(directoryName);

      bool result = m_pMimeMail->SaveAllToFile(fileName);

      if (m_pMessage)
      {
         m_pMessage->SetSize(FileUtilities::FileSize(fileName));
      }

      return result;
   }

   bool 
   MessageData::GetHasBodyType(const String &sBodyType)
   {
      shared_ptr<MimeBody> pPart = FindPart(sBodyType);      

      return pPart ? true : false;
   }

   int
   MessageData::GetRuleLoopCount()
   {
      String sRulesProcessed = m_pMimeMail->GetRawFieldValue(XHMAILSERVER_LOOPCOUNT);
      if (sRulesProcessed.IsEmpty())
         return 0;

      return _ttoi(sRulesProcessed);
   }

   void
   MessageData::IncreaseRuleLoopCount()
   {
      int iCurrentNo = GetRuleLoopCount() + 1;
      
      SetRuleLoopCount(iCurrentNo);
   }

   void 
   MessageData::SetRuleLoopCount(int iLoopCount)
   {
      m_pMimeMail->SetRawFieldValue(XHMAILSERVER_LOOPCOUNT, StringParser::IntToString(iLoopCount), "");
   }


   void 
   MessageData::GenerateMessageID()
   {
      String sGUID = GUIDCreator::GetGUID();
      sGUID.Replace(_T("{"), _T(""));
      sGUID.Replace(_T("}"), _T(""));
      
      String sMsgID;
      sMsgID.Format(_T("<%s@%s>"), sGUID , Utilities::ComputerName());

      SetFieldValue("Message-ID", sMsgID);
   }

   bool 
   MessageData::IsTextType(const String &sContentType)
   {
      return sContentType.CompareNoCase(_T("text/plain")) == 0;
   }

   bool 
   MessageData::IsHTMLType(const String &sContentType)
   {
      return sContentType.CompareNoCase(_T("text/html")) == 0;
   }

   shared_ptr<MimeBody> 
   MessageData::GetMimeMessage()
   {
      return m_pMimeMail;
   }

   void
   MessageData::SetAutoReplied()
   {
	   SetFieldValue("Auto-Submitted", "auto-replied");
   }

   bool
   MessageData::IsAutoSubmitted()
   {
		String autoSubmitted = GetFieldValue("Auto-Submitted");
		if (autoSubmitted.IsEmpty())
			return false;

		if (autoSubmitted.CompareNoCase(_T("no")) == 0)
			return false;

		return true;
   }

   void
   MessageDataTester::Test()
   {
      return;
      // Following tests are only made in DEBUG.
      #ifndef _DEBUG
         return;
      #endif

      shared_ptr<Message> pMessage = shared_ptr<Message>(new Message);

      // Add recipient
      bool recipientOK = false;
      RecipientParser recipientParser;
      recipientParser.CreateMessageRecipientList("test@test.com", pMessage->GetRecipients(), recipientOK);

      shared_ptr<Account> account;

      // Create message data structure
      shared_ptr<MessageData> pMsgData = shared_ptr<MessageData>(new MessageData());
      pMsgData->LoadFromMessage(account, pMessage);
      pMsgData->SetTo("test@test.com");
      pMsgData->SetFrom("test@test.com");
      pMsgData->SetSubject("Hejsan");
      pMsgData->SetFieldValue("MIME-Version", "1.0");
      
      // Date was not specified. Specify it now.
      String sDate = Time::GetCurrentMimeDate();
      pMsgData->SetSentTime(sDate);


      // Add an attachment.
      pMsgData->GetAttachments()->Add("C:\\windows\\notepad.exe");
      pMsgData->GetAttachments()->Add("C:\\windows\\system32\\calc.exe");

      // Set body contents
      pMsgData->SetHTMLBody("Min <b>HTML</b> body");
      pMsgData->SetHTMLBody("Min <b>HTML</b> body");
      pMsgData->SetBody("Min plaintext body");

      String fileName = PersistentMessage::GetFileName(account, pMessage);

      // Write it
      pMsgData->Write(fileName);
      

      // Save it
      PersistentMessage::SaveObject(pMessage);

      Application::Instance()->SubmitPendingEmail();

      Sleep(50000);
   }



}
