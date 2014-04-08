// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class MessageData;
   class MessageRecipients;

   class Message : public BusinessObject<Message>
   {
   public:

      enum State
      {
         Created = 0,
         Delivering = 1,
         Delivered = 2,
      };

      enum Flags
      {
         FlagSeen = 1,
         FlagDeleted = 2,
         FlagFlagged = 4,
         FlagAnswered = 8,
         FlagDraft = 16,
         FlagRecent = 32,
         FlagVirusScan = 64
      };

	   Message(bool generateFileName);
      Message(const Message& other);
      Message();
      virtual ~Message();

      void Initialize(bool generateFileName);

      String GetName() {return m_sFilename; }

      static String GenerateFileName();

      String GetPartialFileName() const;
      void SetPartialFileName(const String &FileName);

      String GetFromAddress() const { return m_sFromAddress; }
      void SetFromAddress(const String &FromAddress) { m_sFromAddress = FromAddress; }

      unsigned int GetUID() const { return _uid; }
      void SetUID(unsigned int  uid) { _uid = uid; }

      __int64 GetAccountID() const { return m_iMessageAccountID; }
      void SetAccountID(__int64 MsgAccountID) { m_iMessageAccountID = (int) MsgAccountID; }
   
      State GetState() const { return (State) m_iMessageState; }
      void SetState(State MessageState) { m_iMessageState = MessageState; }

      int GetSize() const { return m_iMessageSize; }
      void SetSize(int MessageSize) { m_iMessageSize = MessageSize; }

      __int64 GetFolderID() const { return m_iMessageFolderID; }
      void SetFolderID(__int64 iFolderID) { m_iMessageFolderID = (int) iFolderID; }

      short GetFlags() {return m_iFlags; }
      void SetFlags(short iNewVal) {m_iFlags = iNewVal; }

      bool GetFlagSeen() const;
      void SetFlagSeen(bool bNewVal);
      bool GetFlagDeleted() const;
      void SetFlagDeleted(bool bNewVal);
      bool GetFlagDraft() const;
      void SetFlagDraft(bool bNewVal);
      bool GetFlagAnswered() const;
      void SetFlagAnswered(bool bNewVal);
      bool GetFlagFlagged() const;
      void SetFlagFlagged(bool bNewVal);
      bool GetFlagRecent() const;
      void SetFlagRecent(bool bNewVal);
      bool GetFlagVirusScan() const;
      void SetFlagVirusScan(bool bNewVal);


      void SetNoOfRetries(unsigned short lNewVal) {m_iNoOfRetries = lNewVal; }
      unsigned short GetNoOfRetries() const { return m_iNoOfRetries;}


      void SetCreateTime(const String &sCreateTime) {m_sCreateTime = sCreateTime; }
      String GetCreateTime() const {return m_sCreateTime; }

      shared_ptr<MessageRecipients> GetRecipients();

      bool XMLStore(XNode *pParentNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions) {return true;}

   protected:

      int m_iMessageSize;

      AnsiString m_sCreateTime;
      AnsiString m_sFilename;
      AnsiString m_sFromAddress;
      
      int m_iMessageAccountID;
      int m_iMessageFolderID;

      short m_iMessageState;
      short m_iNoOfRetries;
      short m_iFlags;

      unsigned int _uid;
      
   private:

      bool _GetFlag(int iFlag) const;
      void _SetFlag(int iFlag, bool bSet);

      shared_ptr<MessageRecipients> m_pRecipients;

   };
}
