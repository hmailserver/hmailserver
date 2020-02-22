// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

//#include "../../hMailServer/hMailServer.h"   

#include "../../COM/InterfaceResult.h"
#include "../../COM/InterfaceMessage.h"
#include "../../COM/InterfaceClient.h"
#include "../../COM/InterfaceEventLog.h"
#include "../../COM/InterfaceFetchAccount.h"

//#include "IScriptObject.h"
#include "ScriptObjectContainer.h"

#include "Result.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ScriptObjectContainer::ScriptObjectContainer(void)
   {
      // Default objects
      std::shared_ptr<void> pDummy;
      AddObject("EventLog", pDummy, ScriptObject::OTEventLog);
   }

   ScriptObjectContainer::~ScriptObjectContainer(void)
   {
   }

   void 
   ScriptObjectContainer::AddObject(const String &sName, ScriptObject::ObjectType type)
   {
      std::shared_ptr<ScriptObject> pObject = std::shared_ptr<ScriptObject>(new ScriptObject);
      pObject->eType = type;
      pObject->sName = sName;

      objects_[sName] = pObject;
   }

   void 
   ScriptObjectContainer::AddObject(const String &sName, std::shared_ptr<void> pObj, ScriptObject::ObjectType type)
   {
      std::shared_ptr<ScriptObject> pObject = std::shared_ptr<ScriptObject>(new ScriptObject);
      pObject->eType = type;
      pObject->sName = sName;
      pObject->pObject = pObj;
      objects_[sName] = pObject;
   }

   bool
   ScriptObjectContainer::GetObjectByName(const String &sName, LPUNKNOWN* ppunkItem) const
   {
      std::map<String, std::shared_ptr<ScriptObject> >::const_iterator iterPos = objects_.find(sName);
      if (iterPos == objects_.end())
         return false;

      std::shared_ptr<ScriptObject> pObj = (*iterPos).second;
      switch (pObj->eType)
      {
      case ScriptObject::OTResult:
         {
            CComObject<InterfaceResult> *pResultInt = new CComObject<InterfaceResult>();
            std::shared_ptr<Result> pResult = std::static_pointer_cast<Result>(pObj->pObject);
            pResultInt->AttachItem(pResult);
            pResultInt->QueryInterface(ppunkItem);
            return true;
         }
      case ScriptObject::OTMessage:
         {
            CComObject<InterfaceMessage> *pInterface = new CComObject<InterfaceMessage>();
            std::shared_ptr<Message> pObject = std::static_pointer_cast<Message>(pObj->pObject);
            pInterface->AttachItem(pObject);
            pInterface->QueryInterface(ppunkItem);
           
            return true;
         }
      case ScriptObject::OTClient:
         {
            CComObject<InterfaceClient> *pInterface = new CComObject<InterfaceClient>();
            std::shared_ptr<ClientInfo> pObject = std::static_pointer_cast<ClientInfo>(pObj->pObject);
            
            pInterface->AttachItem(pObject);
            pInterface->QueryInterface(ppunkItem);
            return true;
         }
      case ScriptObject::OTEventLog:
         {
            CComObject<InterfaceEventLog> *pInterface = new CComObject<InterfaceEventLog>();
            pInterface->QueryInterface(ppunkItem);
            return true;
         }
      case ScriptObject::OTFetchAccount:
         {
            CComObject<InterfaceFetchAccount> *pInterface = new CComObject<InterfaceFetchAccount>();
            std::shared_ptr<FetchAccount> pObject = std::static_pointer_cast<FetchAccount>(pObj->pObject);
            pInterface->AttachItem(pObject);
            pInterface->QueryInterface(ppunkItem);

            return true;
         }
      default:
         return true;
      }

      return true;
   }

   std::vector<String> 
   ScriptObjectContainer::GetObjectNames()
   {
      std::vector<String> vecNames;
      auto iterPos = objects_.begin();     
      
      while (iterPos != objects_.end())
      {
         vecNames.push_back((*iterPos).second->sName);
         iterPos++;
      }

      return vecNames;
   }
}