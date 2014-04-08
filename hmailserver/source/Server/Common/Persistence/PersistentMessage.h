// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class Domain;
   class Account;
   class IMAPFolder;

   class PersistentMessage 
                     
   {
   public:
	   PersistentMessage();
	   virtual ~PersistentMessage();

      enum FileLocation
      {  
         QueueFolder = 1,
         PublicFolder = 2,
         AccountFolder = 3
      };

      static shared_ptr<Message> CopyToQueue(shared_ptr<const Account> sourceAccount, shared_ptr<Message> sourceMessage);
      static shared_ptr<Message> CopyToIMAPFolder(shared_ptr<const Account> sourceAccount, shared_ptr<Message> sourceMessage, shared_ptr<IMAPFolder> destinationFolder);
      static shared_ptr<Message> CopyFromQueueToInbox(shared_ptr<Message> sourceMessage, shared_ptr<const Account> destinationAccount);

      static bool DeleteObject(shared_ptr<Message> pMessage);
      static bool SaveObject(shared_ptr<Message> pMessage);
      static bool SaveObject(shared_ptr<Message> pMessage, String &errorMessage);
      static bool AddObject(const shared_ptr<Message> pMessage);
      static bool LockObject(__int64 ObjectID);
      static bool LockObject(shared_ptr<Message> pMessage );
      static bool UnlockObject(shared_ptr<Message> pMessage);
      static bool UnlockAll();
      static bool DeleteFile(shared_ptr<const Account> account, shared_ptr<Message> pMessage);

      static bool GetMessageID(const String &fileName, __int64 &messageID, bool &isPartialFilename);
      static bool ReadObject(shared_ptr<DALRecordset> pRS, shared_ptr<Message> pMessage, bool bReadRecipients = true);
      static bool ReadObject(shared_ptr<Message> pMessage, __int64 ObjectID);
      static bool ReadObject(shared_ptr<Message> pMessage, const SQLCommand &command);

      static bool SetNextTryTime(__int64 iMessageID, bool bUpdateNoOfTries, long lNoOfMinutes);
      static void EnsureFileExistance(shared_ptr<const Account> account, shared_ptr<Message> pMessage);
      
      static bool MoveFileToPublicFolder(const String &sourceLocation, shared_ptr<Message> pMessage);
      static bool MoveFileToUserFolder(const String &sourceLocation, shared_ptr<Message>, shared_ptr<const Account> destinationAccount);

      static bool GetAllMessageFilesAreInDataFolder();
      static bool GetAllMessageFilesArePartialNames();

      static int GetTotalMessageCount();
      static int GetTotalMessageCountDelivered();
      
      static __int64 GetTotalMessageSize();
      // Returns the total size of all messages, calculated in megabytes

      static bool DeleteByAccountID(__int64 iAccountID);

      static AnsiString LoadHeader(const String &fileName);
      static AnsiString LoadHeader(const String &fileName, bool reportError);
      static AnsiString LoadBody(const String &fileName);

      static String GetFileName(shared_ptr<const Message> message);
      static String GetFileName(shared_ptr<const Message> message, FileLocation location);
      static String GetFileName(shared_ptr<const Account> account, shared_ptr<const Message> message);
      static String GetFileName(shared_ptr<const Account> account, shared_ptr<const Message> message, FileLocation location);
      static String GetFileName(const String &accountAddress, shared_ptr<const Message> message);
      static String GetFileName(const String &accountAddress, shared_ptr<const Message> message, FileLocation location);

      static bool GetPartialFilename(const String &fullPath, String &partialPath);

      static bool SaveFlags(shared_ptr<Message> message);

      static bool IsPartialPath(const String &path);
      int GetLatestMessageId();

   private:
      
      static shared_ptr<Message> _CreateCopy(shared_ptr<Message> sourceMessage, int destinationAccountID);

      static bool _MoveMessageFileToFolder(const String &sourceLocation, shared_ptr<Message> pMessage, shared_ptr<const Account> destinationAccount);

      // Recipient functions begin
      static bool _ReadRecipients(shared_ptr<Message> pMessage);
      static bool _SaveRecipients(shared_ptr<Message> pMessage);
      // Recipient functions end
   };

   typedef shared_ptr<Message> MessageSP;
}
