// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "../../../hMailServer/hMailServer.h"   

#include "../../../COM/InterfaceResult.h"
#include "../../../COM/InterfaceMessage.h"
#include "../../../COM/InterfaceClient.h"
#include "../../../COM/InterfaceEventLog.h"
#include "../../../COM/InterfaceFetchAccount.h"

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
      shared_ptr<void> pDummy;
      AddObject("EventLog", pDummy, ScriptObject::OTEventLog);
   }

   ScriptObjectContainer::~ScriptObjectContainer(void)
   {
   }

   void 
   ScriptObjectContainer::AddObject(const String &sName, ScriptObject::ObjectType type)
   {
      shared_ptr<ScriptObject> pObject = shared_ptr<ScriptObject>(new ScriptObject);
      pObject->eType = type;
      pObject->sName = sName;

      m_mapObjects[sName] = pObject;
   }

   void 
   ScriptObjectContainer::AddObject(const String &sName, shared_ptr<void> pObj, ScriptObject::ObjectType type)
   {
      shared_ptr<ScriptObject> pObject = shared_ptr<ScriptObject>(new ScriptObject);
      pObject->eType = type;
      pObject->sName = sName;
      pObject->pObject = pObj;
      m_mapObjects[sName] = pObject;
   }

   bool
   ScriptObjectContainer::GetObjectByName(const String &sName, LPUNKNOWN* ppunkItem) const
   {
      map<String, shared_ptr<ScriptObject> >::const_iterator iterPos = m_mapObjects.find(sName);
      if (iterPos == m_mapObjects.end())
         return false;

      shared_ptr<ScriptObject> pObj = (*iterPos).second;
      switch (pObj->eType)
      {
      case ScriptObject::OTResult:
         {
            CComObject<InterfaceResult> *pResultInt = new CComObject<InterfaceResult>();
            shared_ptr<Result> pResult = shared_static_cast<Result>(pObj->pObject);
            pResultInt->AttachItem(pResult);
            pResultInt->QueryInterface(ppunkItem);
            return true;
         }
      case ScriptObject::OTMessage:
         {
            CComObject<InterfaceMessage> *pInterface = new CComObject<InterfaceMessage>();
            shared_ptr<Message> pObject = shared_static_cast<Message>(pObj->pObject);
            pInterface->AttachItem(pObject);
            pInterface->QueryInterface(ppunkItem);
           
            return true;
         }
      case ScriptObject::OTClient:
         {
            CComObject<InterfaceClient> *pInterface = new CComObject<InterfaceClient>();
            shared_ptr<ClientInfo> pObject = shared_static_cast<ClientInfo>(pObj->pObject);
            
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
            shared_ptr<FetchAccount> pObject = shared_static_cast<FetchAccount>(pObj->pObject);
            pInterface->AttachItem(pObject);
            pInterface->QueryInterface(ppunkItem);

            return true;
         }
      default:
         return true;
      }

      return true;
   }

   vector<String> 
   ScriptObjectContainer::GetObjectNames()
   {
      vector<String> vecNames;
      map<String, shared_ptr<ScriptObject> >::iterator iterPos = m_mapObjects.begin();     
      
      while (iterPos != m_mapObjects.end())
      {
         vecNames.push_back((*iterPos).second->sName);
         iterPos++;
      }

      return vecNames;
   }
}