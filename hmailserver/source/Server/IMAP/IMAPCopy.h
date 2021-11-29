// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class IMAPSimpleCommandParser;
   class IMAPFolder;
   class Message;
   class IMAPCommandArgument;
   class IMAPConnection;

   class IMAPCopy 
   {
   public:
	   IMAPCopy();

      IMAPResult DoForMails(bool isUid, std::shared_ptr<IMAPConnection> pConnection, const String& sTag, const String& sMailNos, const String& sTargetFolder);

   private:

      static std::shared_ptr<IMAPFolder> GetTargetFolder_(bool isUid, std::shared_ptr<IMAPSimpleCommandParser> pParser, std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);
      IMAPResult CopyEmail_(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<Message> pOldMessage, const std::shared_ptr<IMAPFolder> pFolder, unsigned int& targetUid);
   };
}
