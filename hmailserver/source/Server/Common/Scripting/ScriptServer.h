// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "ScriptSite.h"

namespace HM
{
   class ScriptObjectContainer;

   class ScriptServer : public Singleton<ScriptServer>
   {
   public:
      
      enum Event
      {
         EventOnClientConnect = 1001,
         EventOnAcceptMessage = 1002,
         EventOnMessageDeliver = 1003,
         EventOnBackupCompleted = 1004,
         EventOnBackupFailed = 1005,
         EventOnDeliveryStart = 1006,
         EventOnError = 1007,
         EventOnDeliveryFailed = 1008,
         
         EventCustom = 1010,
         
         EventOnExternalAccountDownload = 1011,
         EventOnSMTPData = 1012,
      };

      ScriptServer(void);
      ~ScriptServer(void);

      // Fires an event (if the script engine has been turned
      // on in hMailAdmin. 
      void FireEvent(Event e, const String &sEventCaller, shared_ptr<ScriptObjectContainer> pObjects);

      // Checks the syntax of the scripts in the
      // event directory and return the result.
      String CheckSyntax();

      // Refreshes the scripts in the event directory
      void LoadScripts();
      
      String GetCurrentScriptFile() const;   
   
   private:

      bool _DoesFunctionExist(const String &sProcedure);
 
      String _Compile(const String &sLanguage, const String &sFilename);
      // Compiels the script in sFileName and returns the result. If
      // no compilation errors exists, the function returns an emtpy string.
 
      bool m_bHasOnClientConnect;
      bool m_bHasOnAcceptMessage;
      bool m_bHasOnDeliverMessage;
      bool m_bHasOnBackupCompleted;
      bool m_bHasOnBackupFailed;
      bool m_bHasOnDeliveryStart;
      bool m_bHasOnError;
      bool m_bHasOnDeliveryFailed;
      bool m_bHasOnExternalAccountDownload;
      bool m_bHasOnSMTPData;

      String m_sScriptContents;
      String m_sScriptExtension;
      String m_sScriptLanguage;

   };
}