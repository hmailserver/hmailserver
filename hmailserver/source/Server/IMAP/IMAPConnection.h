// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


#include "IMAPNotificationClient.h"
#include "../common/TCPIP/TCPConnection.h"


using namespace std;

namespace HM
{

   class IMAPCommand;
   class IMAPFolders;
   class IMAPFolder;
   class IMAPCommandArgument;
   class Message;
   class IMAPMailboxChangeNotifier;
   class IMailboxChangeClient;

   class IMAPClientCommand
   {

   public:
      String Command;
      String Tag;
      String EntireLine;
      
      std::vector<String> vecLiteralData;
   };

   class IMAPConnection : public TCPConnection
   {
   public:
      IMAPConnection(ConnectionSecurity connection_security,
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context);
	   virtual ~IMAPConnection();
      void Initialize();

      enum eIMAPCommandType
      {
         IMAP_UNKNOWN = 0,
         IMAP_CAPABILITY = 101,
         IMAP_LOGIN = 102,
         IMAP_LIST = 103,
         IMAP_LSUB = 104,
         IMAP_SELECT = 105,
         IMAP_FETCH = 106,
         IMAP_UID = 107,
         IMAP_LOGOUT = 108,
         IMAP_NOOP = 109,
         IMAP_SUBSCRIBE = 110,
         IMAP_CREATE = 111,
         IMAP_EXPUNGE = 112,
         IMAP_DELETE = 113,
         IMAP_UNSUBSCRIBE = 114,
         IMAP_STATUS = 115,
         IMAP_CLOSE = 116,
         IMAP_APPEND = 117,
         IMAP_STORE = 118,
         IMAP_RENAME = 119,
         IMAP_COPY = 120,
         IMAP_EXAMINE = 121,
         IMAP_SEARCH = 122,
         IMAP_AUTHENTICATE = 123,
         IMAP_CHECK = 124,
         IMAP_GETQUOTAROOT = 125,
         IMAP_GETQUOTA = 126,
         IMAP_SORT = 127,
         IMAP_IDLE = 128,
         IMAP_MYRIGHTS = 129,
         IMAP_NAMESPACE = 130,
         IMAP_GETACL = 131,
         IMAP_SETACL = 132,
         IMAP_DELETEACL = 133,
         IMAP_LISTRIGHTS = 134,
         IMAP_STARTTLS = 135
      };

      void ParseData(const AnsiString &Request);
      void ParseData(std::shared_ptr<ByteBuffer> pByteBuffer);
      void SendAsciiData(const AnsiString & sData);
      
      std::shared_ptr<const Account> GetAccount() { return account_; }
      
      void RefreshIMAPFolders();
      void NotifyFolderChange(eIMAPCommandType active_command);
      
      std::shared_ptr<IMAPFolders> GetAccountFolders() const { return imap_folders_;}
      std::shared_ptr<IMAPFolders> GetPublicFolders() const { return public_imap_folders_;}
      
      std::shared_ptr<IMAPFolder> GetFolderByFullPath(const String &sFolderName);
      std::shared_ptr<IMAPFolder> GetFolderByFullPath(std::vector<String> &vecFolderPath);

      std::shared_ptr<IMAPFolder> GetCurrentFolder() { return current_folder_; }

      bool CheckPermission(std::shared_ptr<IMAPFolder> pFolder, int iPermission);
      void CheckFolderPermissions(std::shared_ptr<IMAPFolder> pFolder, bool &readAccess, bool &writeAccess);

      void CloseCurrentFolder();
      void SetCurrentFolder(std::shared_ptr<IMAPFolder> pFolder, bool readOnly);
   
      void SendResponseString(const String &sTag, const String &sResponse, const String &sMessage);

      bool GetIsIdling() const;
      void SetIsIdling(bool bNewVal);

      void SetDelayedChangeNotification(std::shared_ptr<ChangeNotification> pNotification);

      void Login(std::shared_ptr<const Account> account);
      void Logout(const String &goodbyeMessage);
  
      bool IsAuthenticated();
      bool GetCurrentFolderReadOnly() {return current_folder_read_only_; }

      std::shared_ptr<IMAPNotificationClient> GetNotificationClient() {return notification_client_;}

      void StartHandshake();
	 
	  void SetRecentMessages(const std::set<__int64> &messages);
      std::set<__int64>& GetRecentMessages();


      void SetCommandBuffer(const String &sval);


   protected:

      virtual void OnConnected();
      virtual void OnHandshakeCompleted();
      virtual void OnHandshakeFailed() {};
      virtual AnsiString GetCommandSeparator() const;

      void LogClientCommand_(const String &sClientData);
      
      virtual void OnExcessiveDataReceived();
      virtual void OnConnectionTimeout();
            
      eIMAPCommandType GetCommandType(String & sCommand);

      std::map<eIMAPCommandType, std::shared_ptr<IMAPCommand> > mapCommandHandlers;
      std::map<eIMAPCommandType, std::shared_ptr<IMAPCommand> > mapStaticHandlers;

   private:

      std::set<__int64> recent_messages_;

      bool InternalParseData(const AnsiString &Request);
      void SendBanner_();
      void SetAccount_(std::shared_ptr<const Account> account) { account_ = account; }

      void Disconnect_();
      bool IsReceivingLiteralDataForLoginCommand_() const;

      bool AskForLiteralData_(const String &sInput);

      void EndIdleMode_();
      int GetLiteralSize_(const String &sCommand);

      bool AnswerCommand(std::shared_ptr<IMAPClientCommand> command);
      std::shared_ptr<const Account> account_;

      std::shared_ptr<IMAPFolders> imap_folders_;
      std::shared_ptr<IMAPFolders> public_imap_folders_;

      std::shared_ptr<ChangeNotification> delayed_change_notification_;

      // Folder info
      std::shared_ptr<IMAPFolder> current_folder_;
      bool current_folder_read_only_;

      String command_buffer_;
      bool is_idling_;

      int literal_data_to_receive_;
      String literal_buffer_;

      bool pending_disconnect_;

      std::shared_ptr<IMAPNotificationClient> notification_client_;

      int log_level_;
   };
   
}
