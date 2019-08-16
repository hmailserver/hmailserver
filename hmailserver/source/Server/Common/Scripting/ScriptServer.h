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
         EventOnDeliverMessage = 1003,
         EventOnBackupCompleted = 1004,
         EventOnBackupFailed = 1005,
         EventOnDeliveryStart = 1006,
         EventOnError = 1007,
         EventOnDeliveryFailed = 1008,
         
         EventCustom = 1010,
         
         EventOnExternalAccountDownload = 1011,
         EventOnSMTPData = 1012,
         EventOnHELO = 1013,
         EventOnClientLogon = 1014
      };

      ScriptServer(void);
      ~ScriptServer(void);

      // Fires an event (if the script engine has been turned
      // on in hMailAdmin. 
      void FireEvent(Event e, const String &sEventCaller, std::shared_ptr<ScriptObjectContainer> pObjects);

      // Checks the syntax of the scripts in the
      // event directory and return the result.
      String CheckSyntax();

      // Refreshes the scripts in the event directory
      void LoadScripts();
      
      String GetCurrentScriptFile() const;   
   
   private:

      bool DoesFunctionExist_(const String &sProcedure);
 
      String Compile_(const String &sLanguage, const String &sFilename);
      // Compiels the script in sFileName and returns the result. If
      // no compilation errors exists, the function returns an emtpy string.
 
      bool has_on_client_connect_;
      bool has_on_accept_message_;
      bool has_on_deliver_message_;
      bool has_on_backup_completed_;
      bool has_on_backup_failed_;
      bool has_on_delivery_start_;
      bool has_on_error_;
      bool has_on_delivery_failed_;
      bool has_on_external_account_download_;
      bool has_on_smtpdata_;
      bool has_on_helo_;
      bool has_on_client_logon_;

      String script_contents_;
      String script_extension_;
      String script_language_;

   };
}