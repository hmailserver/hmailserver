// Copyright (c) 2006 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Time.h"

namespace HM
{
   class MessageData;

   class MailImporter
   {
   public:
      MailImporter(void);
      ~MailImporter(void);

      static bool Import (const String &originalFullPath, int iAccountID, const String &sIMAPFolder);
      // Imports a message from file to the database.

   private:

      static bool _ReplaceMessagePath(__int64 messageID, const String &messagePath);
      static String _GetInternalDate(shared_ptr<MessageData> pMessageData);
      static String _GetInternalDateDatePlusTimeZone(DateTime dtTime);
      static bool _MoveToNamedSubdirectory(const String &sourceFile, String &resultFile, String &newFullPath);
      static bool _GetRootLevelDirectory(const String &fullPath, String &rootLevel);
   };

#ifdef _DEBUG
   class MailImporterTester
   {
   public :
      MailImporterTester () {};
      ~MailImporterTester () {};      

      void Test();
   };

#endif 
}