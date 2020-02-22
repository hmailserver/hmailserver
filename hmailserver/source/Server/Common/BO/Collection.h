// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Persistence/PersistenceMode.h"

namespace HM
{

   template <class T, class P>
   class Collection
   {
   public:

      virtual ~Collection<T, P>() {};

      virtual void AddItem(std::shared_ptr<T> pObject)
      {
         vecObjects.push_back(pObject);
      }

      bool XMLStore(XNode *pParentNode, int iBackupOptions);
      bool XMLLoad(XNode *pBackupNode, int iRestoreOptions);

      std::shared_ptr<T> GetItem(unsigned int Index) const;
      std::shared_ptr<T> GetItemByDBID(unsigned __int64 DBID) const;
      std::shared_ptr<T> GetItemByDBID(unsigned __int64 DBID, int &foundIndex) const;
      std::shared_ptr<T> GetItemByName(const String &sName) const;

      bool DeleteItemByDBID(__int64 iDBID);
      bool DeleteItem(unsigned int Index);
      virtual bool DeleteAll();

      std::vector<std::shared_ptr<T> > &GetVector() {return vecObjects; }
      const std::vector<std::shared_ptr<T> > &GetConstVector() const {return vecObjects; }

      int GetCount() const {return (int) vecObjects.size(); }


   protected:

      virtual String GetCollectionName() const = 0;

      // Called before saved to XML
      virtual void PostStoreObject(std::shared_ptr<T> pObject, XNode *node) {};

      // Called before save in DB
      virtual bool PreSaveObject(std::shared_ptr<T> pObject, XNode *node) {return true; }
      
      bool DBLoad_(const String &sSQL);
      bool DBLoad_(const SQLCommand &command);

      mutable boost::recursive_mutex _mutex;

      std::vector<std::shared_ptr<T> > vecObjects;
   };


   template <class T, class P> 
   bool Collection<T,P>::XMLStore(XNode *pParentNode, int iBackupOptions)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      if (vecObjects.size() == 0)
         return true;

      XNode *pCollNode = pParentNode->AppendChild(GetCollectionName());

      for (std::shared_ptr<T> pItem : vecObjects)
      {
         if (!pItem->XMLStore(pCollNode, iBackupOptions))
            return false;
         
         unsigned int objectCount = pCollNode->GetChildCount();
         XNode *pElem = pCollNode->GetChild(objectCount-1);

         PostStoreObject(pItem, pElem);
      }

      return true;
   }

   template <class T, class P> 
   bool Collection<T,P>::XMLLoad(XNode *pBackupNode, int iRestoreOptions)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      // First delete the currently existing items.
      if (!DeleteAll())
         return true;

      XNode *pCollNode = pBackupNode->GetChild(GetCollectionName());

      if (pCollNode)
      {
         for (int i = 0; i < pCollNode->GetChildCount(); i++)
         {
            XNode *pChildNode = pCollNode->GetChild(i);

            std::shared_ptr<T> pItem = std::shared_ptr<T>(new T);
            if (!pItem->XMLLoad(pChildNode, iRestoreOptions))
               return false;
            
            // Fire the PreSave event. In this event, parent collections can set
            // properties to the child objects. This is done if the child object
            // isn't aware of some property of itself.
            if (PreSaveObject(pItem, pChildNode))
            {
               String result; 
               if (!P::SaveObject(pItem, result, PersistenceModeRestore))
               {
                  // Handle failure..
                  String message;
                  message.Format(_T("Failed to save object %s. Error: %s"), pItem->GetName().c_str(), result);

                  ErrorManager::Instance()->ReportError(ErrorManager::Critical, 5212, "Collection::XMLLoad", message);
                  return false;
               }

               // Load sub items to this object. Needs to be done
               // after the parent object has been saved in the
               // database, so that the child items can have their
               // database ParentID's properly assigned.
               if (!pItem->XMLLoadSubItems(pChildNode, iRestoreOptions))
                  return false;
            }

            // Add it to the collection.
            vecObjects.push_back(pItem);
         }
      }

      return true;
   }

   template <class T, class P> 
   std::shared_ptr<T> Collection<T,P>::GetItem(unsigned int Index) const
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      std::shared_ptr<T> pRet;

      if (Index >= 0 && Index < vecObjects.size())
         pRet = vecObjects[Index];

      return pRet;
   }

   template <class T, class P> 
   std::shared_ptr<T> Collection<T,P>::GetItemByDBID(unsigned __int64 DBID) const
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      int index = 0;
      return GetItemByDBID(DBID, index);
   }  

   template <class T, class P> 
   std::shared_ptr<T> Collection<T,P>::GetItemByDBID(unsigned __int64 DBID, int &foundIndex) const
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      foundIndex = 0;

      for(std::shared_ptr<T> item : vecObjects)
      {
         foundIndex++;
         if (item->GetID() == DBID)
         {
            return item;
         }
      }

      foundIndex = 0;
      std::shared_ptr<T> EmptyObject;
      return EmptyObject;
   }  

   template <class T, class P> 
   bool Collection<T,P>::DeleteItemByDBID(__int64 DBID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      auto iter = vecObjects.begin();
      auto iterEnd = vecObjects.end();
      
      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<T> pObject = (*iter);
         if (pObject->GetID() == DBID)
         {
            P::DeleteObject(pObject);
            vecObjects.erase(iter);
            return true;
         }
      }

      return false;
   }  

   template <class T, class P> 
   bool Collection<T,P>::DeleteAll()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      for (std::shared_ptr<T> object : vecObjects)
      {
         if (!P::DeleteObject(object))
            return false;
      }

      vecObjects.clear();
      return true;
   } 

   template <class T, class P> 
   bool Collection<T,P>::DeleteItem(unsigned int index)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      if (index >= vecObjects.size())
         return false;

      auto iter = vecObjects.begin() + index;
      P::DeleteObject(*iter);
      vecObjects.erase(iter);
      
      return true;
   }  

   template <class T, class P> 
   std::shared_ptr<T> Collection<T,P>::GetItemByName(const String &sName) const
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      std::vector<std::shared_ptr<T> >::const_iterator iter = vecObjects.begin();
      
      for(std::shared_ptr<T> pObject : vecObjects)
      {
         if (pObject->GetName().CompareNoCase(sName) == 0)
            return pObject;
      }

      std::shared_ptr<T> EmptyObject;
      return EmptyObject;
   }  

   template <class T, class P>  
   bool Collection<T,P>::DBLoad_(const String &sSQL)
   {
      return DBLoad_(SQLCommand(sSQL));
   }

   template <class T, class P>  
   bool Collection<T,P>::DBLoad_(const SQLCommand &command)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      vecObjects.clear();

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return false;

      while (!pRS->IsEOF())
      {
         std::shared_ptr<T> pItem = std::shared_ptr<T>(new T);
         if (!P::ReadObject(pItem, pRS))
            return false;

         vecObjects.push_back(pItem);
         pRS->MoveNext();
      }

      return true;
   }

}