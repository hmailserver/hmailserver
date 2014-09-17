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

      String GetName() {return filename_; }

      static String GenerateFileName();

      String GetPartialFileName() const;
      void SetPartialFileName(const String &FileName);

      String GetFromAddress() const { return from_address_; }
      void SetFromAddress(const String &FromAddress) { from_address_ = FromAddress; }

      unsigned int GetUID() const { return uid_; }
      void SetUID(unsigned int  uid) { uid_ = uid; }

      __int64 GetAccountID() const { return message_account_id_; }
      void SetAccountID(__int64 MsgAccountID) { message_account_id_ = (int) MsgAccountID; }
   
      State GetState() const { return (State) message_state_; }
      void SetState(State MessageState) { message_state_ = MessageState; }

      int GetSize() const { return message_size_; }
      void SetSize(int MessageSize) { message_size_ = MessageSize; }

      __int64 GetFolderID() const { return message_folder_id_; }
      void SetFolderID(__int64 iFolderID) { message_folder_id_ = (int) iFolderID; }

      short GetFlags() {return flags_; }
      void SetFlags(short iNewVal) {flags_ = iNewVal; }

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


      void SetNoOfRetries(unsigned short lNewVal) {no_of_retries_ = lNewVal; }
      unsigned short GetNoOfRetries() const { return no_of_retries_;}


      void SetCreateTime(const String &sCreateTime) {create_time_ = sCreateTime; }
      String GetCreateTime() const {return create_time_; }

      std::shared_ptr<MessageRecipients> GetRecipients();

      bool XMLStore(XNode *pParentNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions) {return true;}

   protected:

      int message_size_;

      AnsiString create_time_;
      AnsiString filename_;
      AnsiString from_address_;
      
      int message_account_id_;
      int message_folder_id_;

      short message_state_;
      short no_of_retries_;
      short flags_;

      unsigned int uid_;
      
   private:

      bool GetFlag_(int iFlag) const;
      void SetFlag_(int iFlag, bool bSet);

      std::shared_ptr<MessageRecipients> recipients_;

   };
}
