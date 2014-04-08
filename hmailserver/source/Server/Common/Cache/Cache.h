// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

namespace HM
{
   template <class T, class P>
   class Cache : public Singleton<Cache<T,P>>
   {
   public:
      Cache();

      shared_ptr<const T> GetObject(const String &sName);
      // Retrieves an object using the object name.

      shared_ptr<const T> GetObject(__int64 iID);
      // Retrieves an object using the ID

      void RemoveObject(shared_ptr<T> pObject);
      void RemoveObject(const String &sName);
      void RemoveObject(__int64 iID);

      void SetTTL(int iNewVal);
      int GetHitRate();
      void SetEnabled(bool bEnabled);
      void Clear();

   private:

      bool _GetObjectIsWithinTTL(shared_ptr<T> pObject);
      void _AddToCache(shared_ptr<T> pObject);

      int m_iNoOfMisses;
      int m_iNoOfHits;
      int m_iTTL;
      bool m_bEnabled;


      // Properties used to determine how long objects 
      // should be stored and the current statistics

      CriticalSection m_oAccessCritSec;
      // All access to the container is restricted by
      // a critical section
      
      std::map<String, shared_ptr<T> > m_mapObjects;
      // All the objects in the cache
   };

   template <class T, class P> 
   Cache<T,P>::Cache()
   {
      m_iNoOfMisses = 0;
      m_iNoOfHits = 0;
      m_iTTL = 0;
      m_bEnabled = false;
   }

   template <class T, class P> 
   void
   Cache<T,P>::Clear()
   {
      CriticalSectionScope scope(m_oAccessCritSec);

      m_mapObjects.clear();
      m_iNoOfMisses = 0;
      m_iNoOfHits = 0;
   }

   template <class T, class P> 
   void
   Cache<T,P>::SetTTL(int iNewVal)
   {
      m_iTTL = iNewVal;

      m_iNoOfMisses = 0;
      m_iNoOfHits = 0;
   }


   template <class T, class P> 
   void
   Cache<T,P>::SetEnabled(bool bEnabled)
   {
      m_bEnabled = bEnabled;

      if (!m_bEnabled)
         Clear();
   }


   template <class T, class P> 
   int
   Cache<T,P>::GetHitRate()
   {
      CriticalSectionScope scope(m_oAccessCritSec);

      if (m_iNoOfHits == 0)
         return 0;

      int iHitRate = (int) (((float) m_iNoOfHits / (float) (m_iNoOfHits + m_iNoOfMisses)) * 100);

      return iHitRate;
   }

   template <class T, class P> 
   void 
   Cache<T,P>::RemoveObject(shared_ptr<T> pObject)
   {
      CriticalSectionScope scope(m_oAccessCritSec);

      std::map<String, shared_ptr<T> >::iterator iterObject = m_mapObjects.find(pObject->GetName());
   
      if (iterObject != m_mapObjects.end())
         m_mapObjects.erase(iterObject);

   }

   template <class T, class P> 
   void 
   Cache<T,P>::RemoveObject(const String &sName)
   {
      CriticalSectionScope scope(m_oAccessCritSec);

      std::map<String, shared_ptr<T> >::iterator iterObject = m_mapObjects.find(sName);

      if (iterObject != m_mapObjects.end())
         m_mapObjects.erase(iterObject);

   }

   template <class T, class P> 
   void 
   Cache<T,P>::RemoveObject(__int64 iID)
   {
      CriticalSectionScope scope(m_oAccessCritSec);

      // Find the domain using the ID
      std::map<String, shared_ptr<T> >::iterator iterObject = m_mapObjects.begin();
      std::map<String, shared_ptr<T> >::iterator iterEnd = m_mapObjects.end();

      for (; iterObject != iterEnd; iterObject++)
      {
         shared_ptr<T> pObject = (*iterObject).second;

         if (pObject->GetID() == iID)
         {
            m_mapObjects.erase(iterObject);
            return;
         }

      }
   }

   template <class T, class P> 
   shared_ptr<const T> 
   Cache<T,P>::GetObject(const String &sName)
   {
      CriticalSectionScope scope(m_oAccessCritSec);

      if (m_bEnabled)
      {
         std::map<String, shared_ptr<T> >::iterator iterObject = m_mapObjects.find(sName);

         if (iterObject != m_mapObjects.end())
         {
            shared_ptr<T> pObject = (*iterObject).second;

            if (_GetObjectIsWithinTTL(pObject))
               return pObject;
         
            // Object has passed TTL
            m_mapObjects.erase(iterObject);
         }
      }

      // Load the object
      shared_ptr<T> pRetObject = shared_ptr<T>(new T);
      
      if (!P::ReadObject(pRetObject, sName))
      {
         shared_ptr<T> pEmpty;
         return pEmpty;
      }

      if (m_bEnabled)
         _AddToCache(pRetObject);

      return pRetObject;
   }

   template <class T, class P> 
   shared_ptr<const T> 
   Cache<T,P>::GetObject(__int64 iID)
   {
      CriticalSectionScope scope(m_oAccessCritSec);

      if (m_bEnabled)
      {
         // Find the domain using the ID
         std::map<String, shared_ptr<T> >::iterator iterObject = m_mapObjects.begin();
         std::map<String, shared_ptr<T> >::iterator iterEnd = m_mapObjects.end();

         for (; iterObject != iterEnd; iterObject++)
         {
            shared_ptr<T> pObject = (*iterObject).second;

            if (pObject->GetID() == iID)
            {
               if (_GetObjectIsWithinTTL(pObject))
                  return pObject;

               m_mapObjects.erase(iterObject);

               break;
            }
         }
      }

      // Load the object
      shared_ptr<T> pRetObject = shared_ptr<T>(new T);
      if (!P::ReadObject(pRetObject, iID))
      {
         shared_ptr<T> pEmpty;
         return pEmpty;
      }

      if (m_bEnabled)
         _AddToCache(pRetObject);
   
      return pRetObject;
   }

   template <class T, class P> 
   void 
   Cache<T,P>::_AddToCache(shared_ptr<T> pObject)
   {
      CriticalSectionScope scope(m_oAccessCritSec);

      // Object must be saved before it can be cached.
#ifdef DEBUG
      if (pObject->GetID() == 0)
      {
         assert(0);
      }
#endif

      m_iNoOfMisses++;
      m_mapObjects[pObject->GetName()] = pObject;
   }

   template <class T, class P> 
   bool 
   Cache<T,P>::_GetObjectIsWithinTTL(shared_ptr<T> pObject)
   {
      if (pObject)
      {
         if (pObject->Seconds() < m_iTTL)
         {
            // A fresh object was found in the cache.
            m_iNoOfHits++;
            return true;
         }
      }

      return false;
   }
}
