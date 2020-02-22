// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class Domain;
   class Account;
   class IMAPFolder;
   enum PersistenceMode;

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

      static std::shared_ptr<Message> CopyToQueue(std::shared_ptr<const Account> sourceAccount, std::shared_ptr<Message> sourceMessage);
      static std::shared_ptr<Message> CopyToIMAPFolder(std::shared_ptr<const Account> sourceAccount, std::shared_ptr<Message> sourceMessage, std::shared_ptr<IMAPFolder> destinationFolder);
      static std::shared_ptr<Message> CopyFromQueueToInbox(std::shared_ptr<Message> sourceMessage, std::shared_ptr<const Account> destinationAccount);

      static bool DeleteObject(std::shared_ptr<Message> pMessage);
      static bool SaveObject(std::shared_ptr<Message> pMessage);
      static bool SaveObject(std::shared_ptr<Message> pMessage, String &errorMessage, PersistenceMode mode);
      static bool AddObject(const std::shared_ptr<Message> pMessage);
      static bool LockObject(__int64 ObjectID);
      static bool LockObject(std::shared_ptr<Message> pMessage );
      static bool UnlockObject(std::shared_ptr<Message> pMessage);
      static bool UnlockAll();
      static bool DeleteFile(std::shared_ptr<const Account> account, std::shared_ptr<Message> pMessage);

      static bool GetMessageID(const String &fileName, __int64 &messageID, bool &isPartialFilename);
      static bool ReadObject(std::shared_ptr<DALRecordset> pRS, std::shared_ptr<Message> pMessage, bool bReadRecipients = true);
      static bool ReadObject(std::shared_ptr<Message> pMessage, __int64 ObjectID);
      static bool ReadObject(std::shared_ptr<Message> pMessage, const SQLCommand &command);

      static bool SetNextTryTime(__int64 iMessageID, bool bUpdateNoOfTries, long lNoOfMinutes);
      static void EnsureFileExistance(std::shared_ptr<const Account> account, std::shared_ptr<Message> pMessage);
      
      static bool MoveFileToPublicFolder(const String &sourceLocation, std::shared_ptr<Message> pMessage);
      static bool MoveFileToUserFolder(const String &sourceLocation, std::shared_ptr<Message>, std::shared_ptr<const Account> destinationAccount);

      static bool GetAllMessageFilesAreInDataFolder();
      static bool GetAllMessageFilesArePartialNames();

      static int GetTotalMessageCount();
      static int GetTotalMessageCountDelivered();
      
      static bool DeleteByAccountID(__int64 iAccountID);

      static AnsiString LoadHeader(const String &fileName);
      static AnsiString LoadHeader(const String &fileName, bool reportError);
      static AnsiString LoadBody(const String &fileName);

      static String GetFileName(std::shared_ptr<const Message> message);
      static String GetFileName(std::shared_ptr<const Message> message, FileLocation location);
      static String GetFileName(std::shared_ptr<const Account> account, std::shared_ptr<const Message> message);
      static String GetFileName(std::shared_ptr<const Account> account, std::shared_ptr<const Message> message, FileLocation location);
      static String GetFileName(const String &accountAddress, std::shared_ptr<const Message> message);
      static String GetFileName(const String &accountAddress, std::shared_ptr<const Message> message, FileLocation location);

      static bool GetPartialFilename(const String &fullPath, String &partialPath);

      static bool SaveFlags(std::shared_ptr<Message> message);

      static bool IsPartialPath(const String &path);
      int GetLatestMessageId();

   private:
      
      static std::shared_ptr<Message> CreateCopy_(std::shared_ptr<Message> sourceMessage, int destinationAccountID);

      // Recipient functions begin
      static bool ReadRecipients_(std::shared_ptr<Message> pMessage);
      static bool SaveRecipients_(std::shared_ptr<Message> pMessage);
      // Recipient functions end
   };

   typedef std::shared_ptr<Message> MessageSP;
}
