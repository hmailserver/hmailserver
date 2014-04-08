// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


#include "../Common/TCPIP/ProtocolParser.h"

#include "IMAPNotificationClient.h"


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

   class IMAPConnection : public ProtocolParser, 
                          public boost::enable_shared_from_this<IMAPConnection>
   {
   public:
	   IMAPConnection();
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
         IMAP_LISTRIGHTS = 134
      };

      void ParseData(const AnsiString &Request);
      void ParseData(shared_ptr<ByteBuffer> pByteBuffer);
      bool SendAsciiData(const AnsiString & sData);
      
      shared_ptr<const Account> GetAccount() { return _account; }
      
      void RefreshIMAPFolders();
      void NotifyFolderChange();
      
      shared_ptr<IMAPFolders> GetAccountFolders() const { return m_pIMAPFolders;}
      shared_ptr<IMAPFolders> GetPublicFolders() const { return m_pPublicIMAPFolders;}
      
      shared_ptr<IMAPFolder> GetFolderByFullPath(const String &sFolderName);
      shared_ptr<IMAPFolder> GetFolderByFullPath(std::vector<String> &vecFolderPath);

      shared_ptr<IMAPFolder> GetCurrentFolder() { return m_pCurrentFolder; }

      bool CheckPermission(shared_ptr<IMAPFolder> pFolder, int iPermission);
      void CheckFolderPermissions(shared_ptr<IMAPFolder> pFolder, bool &readAccess, bool &writeAccess);

      void CloseCurrentFolder();
      void SetCurrentFolder(shared_ptr<IMAPFolder> pFolder, bool readOnly);
   
      void SendResponseString(const String &sTag, const String &sResponse, const String &sMessage);

      bool GetIsIdling() const;
      void SetIsIdling(bool bNewVal);

      void SetDelayedChangeNotification(shared_ptr<ChangeNotification> pNotification);

      void Login(shared_ptr<const Account> account);
      void Logout(const String &goodbyeMessage);

      void SetReceiveBinary(bool binary);
   
      bool IsAuthenticated();
      bool GetCurrentFolderReadOnly() {return _currentFolderReadOnly; }

      IPAddress GetIPAddress()
      {
         return ProtocolParser::GetIPAddress();
      }

      shared_ptr<IMAPNotificationClient> GetNotificationClient() {return _notificationClient;}

   protected:

      virtual void OnConnected();
      virtual AnsiString GetCommandSeparator() const;

      void _LogClientCommand(const String &sClientData) const;
      
      virtual void OnExcessiveDataReceived();
      virtual void OnConnectionTimeout();
            
      eIMAPCommandType GetCommandType(String & sCommand);

      std::vector<shared_ptr<IMAPClientCommand> > vecIncoming;
      std::map<eIMAPCommandType, shared_ptr<IMAPCommand> > mapCommandHandlers;
      std::map<eIMAPCommandType, shared_ptr<IMAPCommand> > mapStaticHandlers;

   private:

      void _SetAccount(shared_ptr<const Account> account) { _account = account; }

      void _Disconnect();
      bool _IsReceivingLiteralDataForLoginCommand() const;

      void InternalParseData(const String &Request);

      bool _AskForLiteralData(const String &sInput);

      void _EndIdleMode();
      int _GetLiteralSize(const String &sCommand);

      void AnswerCommand();
      shared_ptr<const Account> _account;

      shared_ptr<IMAPFolders> m_pIMAPFolders;
      shared_ptr<IMAPFolders> m_pPublicIMAPFolders;

      shared_ptr<ChangeNotification> m_pDelayedChangeNotification;

      // Folder info
      shared_ptr<IMAPFolder> m_pCurrentFolder;
      bool _currentFolderReadOnly;

      String m_sCommandBuffer;
      bool m_bIsIdling;

      int m_iLiteralDataToReceive;
      String m_sLiteralBuffer;

      bool m_bPendingDisconnect;

      shared_ptr<IMAPNotificationClient> _notificationClient;

      int  m_iLogLevel;      

   };
   
}
