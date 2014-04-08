// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\Events.h"

#include "..\..\SMTP\SMTPConfiguration.h"

#include "../BO/Account.h"
#include "../BO/Message.h"

#include "../Application/ScriptingHost/ScriptServer.h"
#include "../Application/ScriptingHost/ScriptObjectContainer.h"
#include "../Application/ScriptingHost/Result.h"

#include "../Persistence/PersistentMessage.h"

#include "../Util/AWStats.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Events::Events(void)
   {

   }

   Events::~Events(void)
   {
   }

   bool 
   Events::FireOnDeliveryStart(shared_ptr<Message> pMessage)
   {
      if (!Configuration::Instance()->GetUseScriptServer())
         return true;

      shared_ptr<ScriptObjectContainer> pContainer = shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
      shared_ptr<Result> pResult = shared_ptr<Result>(new Result);
      pContainer->AddObject("HMAILSERVER_MESSAGE", pMessage, ScriptObject::OTMessage);
      pContainer->AddObject("Result", pResult, ScriptObject::OTResult);
      String sEventCaller = "OnDeliveryStart(HMAILSERVER_MESSAGE)";
      ScriptServer::Instance()->FireEvent(ScriptServer::EventOnDeliveryStart, sEventCaller, pContainer);

      switch (pResult->GetValue())
      {
      case 1:
         {
            String sMessage;
            sMessage.Format(_T("SMTPDeliverer - Message %I64d: ")
               _T("Message deleted. Action was taken by script subscribing to OnDeliveryStart."),
               pMessage->GetID());

            LOG_APPLICATION(sMessage);

            PersistentMessage::DeleteObject(pMessage);             

            return false;
         }
      }

      return true;
   }

   bool 
   Events::FireOnDeliverMessage(shared_ptr<Message> pMessage)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Executes the OnDeliverMessage event.
   //---------------------------------------------------------------------------()
   {
      if (Configuration::Instance()->GetUseScriptServer())
      {
         shared_ptr<ScriptObjectContainer> pContainer = shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         shared_ptr<Result> pResult = shared_ptr<Result>(new Result);
         pContainer->AddObject("HMAILSERVER_MESSAGE", pMessage, ScriptObject::OTMessage);
         pContainer->AddObject("Result", pResult, ScriptObject::OTResult);
         String sEventCaller = "OnDeliverMessage(HMAILSERVER_MESSAGE)";
         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnMessageDeliver, sEventCaller, pContainer);

         switch (pResult->GetValue())
         {
         case 1:
            {
               String sMessage;
               sMessage.Format(_T("SMTPDeliverer - Message %I64d: ")
                  _T("Message deleted. Action was taken by script subscribing to OnDeliverMessage."),
                  pMessage->GetID());

               LOG_APPLICATION(sMessage);

               PersistentMessage::DeleteObject(pMessage);             

               return false;
            }
         }
      }      

      return true;
   }

   void
   Events::FireOnDeliveryFailed(shared_ptr<Message> pMessage, const String &sSendersIP, const String &sRecipient, const String &sErrorMessage)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Called once each time delivery to a recipient failed. It may be called
   // several times for a single message.
   //---------------------------------------------------------------------------()
   {
      AWStats::LogDeliveryFailure(sSendersIP, pMessage->GetFromAddress(), sRecipient,  550);

      // Send an event
      if (Configuration::Instance()->GetUseScriptServer())
      {
         String sEventCaller;

         String sRecipientCopy = sRecipient;
         String sErrorMessageCopy = sErrorMessage;

         String sScriptLanguage = Configuration::Instance()->GetScriptLanguage();

         if (sScriptLanguage == _T("VBScript"))
         {
            sRecipientCopy.Replace(_T("\""), _T(""));
            sErrorMessageCopy.Replace(_T("\""), _T(""));

            sErrorMessageCopy.Replace(_T("\r"), _T(""));
            sErrorMessageCopy.Replace(_T("\n"), _T(""));

            sErrorMessageCopy.TrimLeft();
            sErrorMessageCopy.TrimRight();

            sEventCaller.Format(_T("OnDeliveryFailed(HMAILSERVER_MESSAGE, \"%s\", \"%s\")"), sRecipientCopy, sErrorMessageCopy);
         }
         else if (sScriptLanguage == _T("JScript"))
         {
            sRecipientCopy.Replace(_T("'"), _T("\\'"));
            sErrorMessageCopy.Replace(_T("'"), _T("\\'"));

            sErrorMessageCopy.Replace(_T("\r"), _T(""));
            sErrorMessageCopy.Replace(_T("\n"), _T(""));

            sErrorMessageCopy.TrimLeft();
            sErrorMessageCopy.TrimRight();

            sEventCaller.Format(_T("OnDeliveryFailed(HMAILSERVER_MESSAGE, '%s', '%s')"), sRecipientCopy, sErrorMessageCopy);
         }

         shared_ptr<ScriptObjectContainer> pContainer  = shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
         pContainer->AddObject("HMAILSERVER_MESSAGE", pMessage, ScriptObject::OTMessage);

         ScriptServer::Instance()->FireEvent(ScriptServer::EventOnDeliveryFailed, sEventCaller, pContainer);
      }      

   }

   shared_ptr<Result>
   Events::FireOnExternalAccountDownload(shared_ptr<FetchAccount> fetchAccount, shared_ptr<Message> pMessage, const String &sRemoteUID)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Called once each time delivery to a recipient failed. It may be called
   // several times for a single message.
   //---------------------------------------------------------------------------()
   {
      // Send an event
      shared_ptr<Result> pResult = shared_ptr<Result>(new Result);
      if (!Configuration::Instance()->GetUseScriptServer())
         return pResult;
      
      String sEventCaller;

      String sRemoteUIDCopy = sRemoteUID;

      String sScriptLanguage = Configuration::Instance()->GetScriptLanguage();

      if (sScriptLanguage == _T("VBScript"))
      {
         sRemoteUIDCopy.Replace(_T("\""), _T("\"\""));

         if (pMessage)
            sEventCaller.Format(_T("OnExternalAccountDownload(HMAILSERVER_FETCHACCOUNT, HMAILSERVER_MESSAGE, \"%s\")"), sRemoteUIDCopy);
         else
            sEventCaller.Format(_T("OnExternalAccountDownload(HMAILSERVER_FETCHACCOUNT, Nothing, \"%s\")"), sRemoteUIDCopy);
      }
      else if (sScriptLanguage == _T("JScript"))
      {
         sRemoteUIDCopy.Replace(_T("'"), _T("\'"));

         if (pMessage)
            sEventCaller.Format(_T("OnExternalAccountDownload(HMAILSERVER_FETCHACCOUNT, HMAILSERVER_MESSAGE, '%s')"), sRemoteUIDCopy);
         else
            sEventCaller.Format(_T("OnExternalAccountDownload(HMAILSERVER_FETCHACCOUNT, null, '%s')"), sRemoteUIDCopy);
      }

      shared_ptr<ScriptObjectContainer> pContainer  = shared_ptr<ScriptObjectContainer>(new ScriptObjectContainer);
      
      pContainer->AddObject("HMAILSERVER_FETCHACCOUNT", fetchAccount, ScriptObject::OTFetchAccount);

      if (pMessage)
         pContainer->AddObject("HMAILSERVER_MESSAGE", pMessage, ScriptObject::OTMessage);
      pContainer->AddObject("Result", pResult, ScriptObject::OTResult);

      ScriptServer::Instance()->FireEvent(ScriptServer::EventOnExternalAccountDownload, sEventCaller, pContainer);

      return pResult;
   }

}