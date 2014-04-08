// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"


#include "../../../hMailServer/hMailServer.h"


#include "ScriptServer.h"
#include "ScriptSite.h"
#include "ScriptObjectContainer.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ScriptServer::ScriptServer(void) :
      m_bHasOnClientConnect(false),
      m_bHasOnAcceptMessage(false),
      m_bHasOnDeliverMessage(false),
      m_bHasOnBackupCompleted(false),
      m_bHasOnBackupFailed(false),
      m_bHasOnDeliveryStart(false),
      m_bHasOnError(false),
      m_bHasOnDeliveryFailed(false),
      m_bHasOnExternalAccountDownload(false),
      m_bHasOnSMTPData(false)
   {
      
   }

   ScriptServer::~ScriptServer(void)
   {
      
   }

   String 
   ScriptServer::GetCurrentScriptFile() const
   {
      String sEventsDir = IniFileSettings::Instance()->GetEventDirectory();

      String sScriptLanguage = Configuration::Instance()->GetScriptLanguage();

      String sExtension;
      if (sScriptLanguage == _T("VBScript"))
         sExtension = "vbs";
      else if (sScriptLanguage == _T("JScript"))
         sExtension = "js";
      else
         sExtension = "";

      String sFileName = sEventsDir + "\\EventHandlers." + sExtension;

      return sFileName;
   }

   void
   ScriptServer::LoadScripts()
   {
      try
      {
         m_sScriptLanguage = Configuration::Instance()->GetScriptLanguage();

         if (m_sScriptLanguage == _T("VBScript"))
            m_sScriptExtension = "vbs";
         else if (m_sScriptLanguage == _T("JScript"))
            m_sScriptExtension = "js";
         else
            m_sScriptExtension = "";

         String sCurrentScriptFile = GetCurrentScriptFile();

         // Load the script file from disk
         m_sScriptContents = FileUtilities::ReadCompleteTextFile(sCurrentScriptFile);

         // Do a syntax check before loading it.
         String sMessage = ScriptServer::CheckSyntax();
         if (!sMessage.IsEmpty())
         {
            ErrorManager::Instance()->ReportError(ErrorManager::High, 5016, "ScriptServer::LoadScripts", sMessage);
            return;
         }
         
         // Determine which functions are available.
         m_bHasOnClientConnect = _DoesFunctionExist("OnClientConnect");
         m_bHasOnAcceptMessage = _DoesFunctionExist("OnAcceptMessage");
         m_bHasOnDeliverMessage = _DoesFunctionExist("OnDeliverMessage");
         m_bHasOnBackupCompleted = _DoesFunctionExist("OnBackupCompleted");
         m_bHasOnBackupFailed = _DoesFunctionExist("OnBackupFailed");
         m_bHasOnDeliveryStart = _DoesFunctionExist("OnDeliveryStart");
         m_bHasOnError = _DoesFunctionExist("OnError");
         m_bHasOnDeliveryFailed = _DoesFunctionExist("OnDeliveryFailed");
         m_bHasOnExternalAccountDownload = _DoesFunctionExist("OnExternalAccountDownload");
         m_bHasOnSMTPData = _DoesFunctionExist("OnSMTPData");

      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5017, "ScriptServer::LoadScripts", "An exception was thrown when loading scripts.");
      }
   }


   String
   ScriptServer::CheckSyntax()
   {
      String sEventsDir = IniFileSettings::Instance()->GetEventDirectory();      

      String sScriptLanguage = Configuration::Instance()->GetScriptLanguage();
      String sScriptExtension;
      if (sScriptLanguage == _T("VBScript"))
         sScriptExtension = "vbs";
      else if (sScriptLanguage == _T("JScript"))
         sScriptExtension = "js";
      else
         sScriptExtension = "";

      String sErrorMessage;

      // Compile the scripts.
      sErrorMessage = _Compile(sScriptLanguage, sEventsDir + "\\EventHandlers." + sScriptExtension);
      if (!sErrorMessage.IsEmpty())
         return sErrorMessage;

      return String("");
   }

   bool 
   ScriptServer::_DoesFunctionExist(const String &sProcedure)
   {
      // Create an instance of the script engine and execute the script.
      CComObject<CScriptSiteBasic>* pBasic;
      CComObject<CScriptSiteBasic>::CreateInstance(&pBasic);

      CComQIPtr<IActiveScriptSite> spUnk;

      if (!pBasic)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5017, "ScriptServer::FireEvent", "Failed to create instance of script site.");
         return false;
      }

      spUnk = pBasic; 
      pBasic->Initiate(m_sScriptLanguage, NULL);
      pBasic->AddScript(m_sScriptContents);
      pBasic->Run();
      bool bExists = pBasic->ProcedureExists(sProcedure);
      pBasic->Terminate();
      
      return bExists;
   }


   String
   ScriptServer::_Compile(const String &sLanguage, const String &sFilename)
   {
      String sContents = FileUtilities::ReadCompleteTextFile(sFilename);

      if (sContents.IsEmpty())
         return "";

      // Create an instance of the script engine and execute the script.
      CComObject<CScriptSiteBasic>* pBasic;
      CComObject<CScriptSiteBasic>::CreateInstance(&pBasic);

      CComQIPtr<IActiveScriptSite> spUnk;

      if (!pBasic)
         return "ScriptServer:: Failed to create instance of script site.";

      spUnk = pBasic; // let CComQIPtr tidy up for us
      pBasic->Initiate(sLanguage, NULL);
      // pBasic->SetObjectContainer(pObjects);
      pBasic->AddScript(sContents);
      pBasic->Run();
      pBasic->Terminate();

      String sErrorMessage = pBasic->GetLastError();

      if (!sErrorMessage.IsEmpty())
         sErrorMessage = "File: " + sFilename + "\r\n" + sErrorMessage;

      return sErrorMessage;

   }

   void 
   ScriptServer::FireEvent(Event e,  const String &sEventCaller, shared_ptr<ScriptObjectContainer> pObjects)
   {
      if (!Configuration::Instance()->GetUseScriptServer())
         return;

	  // JDR: stores the name of the method that is fired in the script. http://www.hmailserver.com/forum/viewtopic.php?f=2&t=25497
	  String m_sEventName = _T("Unknown");

      switch (e)
      {
      case EventOnClientConnect:
		 m_sEventName = _T("OnClientConnect");
         if (!m_bHasOnClientConnect)
            return;
         break;
      case EventOnAcceptMessage:
		 m_sEventName = _T("OnAcceptMessage");
         if (!m_bHasOnAcceptMessage)
            return;
         break;
      case EventOnMessageDeliver:
	     m_sEventName = _T("OnMessageDeliver");
         if (!m_bHasOnDeliverMessage)
            return;
         break;
      case EventOnBackupCompleted:
		  m_sEventName = _T("OnBackupCompleted");
         if (!m_bHasOnBackupCompleted)
            return;
         break;
      case EventOnBackupFailed:
		  m_sEventName = _T("OnBackupFailed");
         if (!m_bHasOnBackupFailed)
            return;
         break;
      case EventOnError:
		  m_sEventName = _T("OnError");
         if (!m_bHasOnError)
            return;
         break;
      case EventOnDeliveryStart:
		  m_sEventName = _T("OnDeliveryStart");
         if (!m_bHasOnDeliveryStart)
            return;
         break;
      case EventOnDeliveryFailed:
		  m_sEventName = _T("OnDeliveryFailed");
         if (!m_bHasOnDeliveryFailed)
            return;
         break;
      case EventOnExternalAccountDownload:
		  m_sEventName = _T("OnExternalAccountDownload");
         if (!m_bHasOnExternalAccountDownload)
            return;
         break;
      case EventOnSMTPData:
		  m_sEventName = _T("OnSMTPData");
         if (!m_bHasOnSMTPData)
            return;
         break;

      case EventCustom:
         break;
      default:
         {
            return;
         }
         
      }

	  // JDR: Added event name to the debug log. http://www.hmailserver.com/forum/viewtopic.php?f=2&t=25497
	  LOG_DEBUG("ScriptServer::FireEvent-" + m_sEventName);

      String sScript;

      // Build the script.
      if (m_sScriptLanguage == _T("VBScript"))
         sScript = m_sScriptContents + "\r\n\r\n" + "Call " + sEventCaller + "\r\n";
      else if (m_sScriptLanguage == _T("JScript"))
         sScript = m_sScriptContents + "\r\n\r\n" + sEventCaller + ";\r\n";

      CComObject<CScriptSiteBasic>* pBasic;
      CComObject<CScriptSiteBasic>::CreateInstance(&pBasic);
      CComQIPtr<IActiveScriptSite> spUnk;
      
      if (!pBasic)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5018, "ScriptServer::FireEvent", "Failed to create instance of script site.");
         return;
      }
      
      spUnk = pBasic; // let CComQIPtr tidy up for us
      pBasic->Initiate(m_sScriptLanguage, NULL);
      pBasic->SetObjectContainer(pObjects);
      pBasic->AddScript(sScript);
      pBasic->Run();
      pBasic->Terminate();

      LOG_DEBUG("ScriptServer:~FireEvent");
   }

}
