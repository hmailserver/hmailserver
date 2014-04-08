// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class MimeHeader;
   class Attachments;

   class MimeBody;

   class MessageData  
   {
   public:
      MessageData();
	   virtual ~MessageData();

      bool LoadFromMessage(const String &fileName, shared_ptr<Message> pMessage);
      bool LoadFromMessage(shared_ptr<const Account> account, shared_ptr<Message> pMessage);

      bool RefreshFromMessage();

      String GetHeader() const;
      
      String GetSubject() const;
      void SetSubject(const String &sSubject);

      String GetCharset() const;
      void SetCharset(const String &sCharset);

      void DeleteField(const AnsiString &sHeaderName);

      String GetSentTime() const;
      void SetSentTime(const String &sSentTime);
      
      String GetTo() const;
      void SetTo(const String &sTo);
      
      String GetReturnPath() const;
      void SetReturnPath(const String &sReturnPath);

      String GetCC() const;
      void SetCC(const String &sCC);
      
      String GetBCC() const;
      void SetBCC(const String &sBCC);

      String GetFrom() const;
      void SetFrom(const String &sFrom);

      String GetHTMLBody() const;
      void SetHTMLBody(const String &sHTMLBody);
      
      String GetBody() const;
      void SetBody(const String &sBody);

      void SetFieldValue(const String &sField, const String &sValue);
      String GetFieldValue(const String &sName) const;

      int GetRuleLoopCount();
      void SetRuleLoopCount(int iLoopCount);
      void IncreaseRuleLoopCount();
      
      
      bool GetHasBodyType(const String &sBodyType);

      bool Write(const String &fileName);

      int GetSize() const;

      shared_ptr<Attachments> GetAttachments();

      shared_ptr<Message> GetMessage() {return m_pMessage; }
   
      bool GetEncodeFields() {return m_bEncodeFields; }
      void SetEncodeFields(bool bNewVal) {m_bEncodeFields = bNewVal; }

      void GenerateMessageID();

      shared_ptr<MimeBody> CreatePart(const String &sContentType);

      shared_ptr<MimeBody> GetMimeMessage();

	  void SetAutoReplied();
	  bool IsAutoSubmitted();

   private:

      bool IsTextType(const String &sContentType);
      bool IsHTMLType(const String &sContentType);

      shared_ptr<MimeBody> FindPart(const String &sType) const;
      shared_ptr<MimeBody> FindPartNoRecurse(shared_ptr<MimeBody> parent, const AnsiString &sType) const;

      shared_ptr<Message> m_pMessage;
      shared_ptr<MimeBody> m_pMimeMail;
      shared_ptr<Attachments> m_pAttachments;

      String _messageFileName;

      bool m_bEncodeFields;
      bool _unfoldWithSpace;
   };

   class MessageDataTester
   {
   public:
      MessageDataTester() {};
      void Test();
   };

}
