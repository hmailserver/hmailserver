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
	  short GetFlagsByUsr(int usr, int msgid)
	  {
		  SQLCommand command2("select * from hm_flags where MsgID = @MSGID and UsrID = @USERID ");
		  command2.AddParameter("@MSGID", msgid);
		  command2.AddParameter("@USERID", usr);
		  std::shared_ptr<DALRecordset> pRS2 = Application::Instance()->GetDBManager()->OpenRecordset(command2);
		  return (short)pRS2->GetLongValue("Flag");
	  }
      void SetFlags(short iNewVal) {flags_ = iNewVal; }
	  void SetFlagsByUsr(int user, short iNewVal, int msgid)
	  {
		  SQLCommand com("select * from hm_flags where MsgID=@MSGID and UsrID=@USRID");
		  com.AddParameter("@MSGID", msgid);
		  com.AddParameter("@USRID", user);
		  std::shared_ptr<DALRecordset> pRS2;
		  pRS2 = Application::Instance()->GetDBManager()->OpenRecordset(com);
		  int MessageID = pRS2->GetLongValue("MsgID");
		  if (MessageID == 0)
		  {
			  SQLStatement oStatement;
			  oStatement.SetStatementType(SQLStatement::STInsert);
			  oStatement.SetTable("hm_flags");
			  oStatement.SetIdentityColumn("DataID");
			  oStatement.AddColumn("MsgID", msgid);
			  oStatement.AddColumn("UsrID", user);

			  oStatement.AddColumn("Flag", flags_);
			  Application::Instance()->GetDBManager()->Execute(oStatement);
		  }
		  else
		  {
			  SQLCommand komanda("update hm_flags set Flag = @FLAGS where MsgID=@MSGID and UsrID=@USRID");
			  komanda.AddParameter("@FLAGS", flags_);
			  komanda.AddParameter("@MSGID", msgid);
			  komanda.AddParameter("@USRID", user);
			  Application::Instance()->GetDBManager()->Execute(komanda);
		  }
	  }

      bool GetFlagSeen() const;
	  bool GetFlagSeenByUsr(int usr, int msgid) const;
      void SetFlagSeen(bool bNewVal);
	  void SetFlagSeenByUsr(int usr, bool nVal, int msgid);
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
	  bool GetFlagByUsr_(int iFlag, int usr, int msgid) const;
	  void SetFlagByUsr_(int iFlag, bool bSet, int iUsr, int msgid);

      std::shared_ptr<MessageRecipients> recipients_;

   };
}
