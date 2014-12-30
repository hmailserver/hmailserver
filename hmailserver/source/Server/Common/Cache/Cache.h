// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

using boost::multi_index_container;
using namespace boost::multi_index;

#include "../Util/VariantDateTime.h"

#include "CachedObject.h"

namespace HM
{

   struct id {};
   struct name {};
   struct timestamp {};

   template <typename T, typename P>
   class Cache : public Singleton<Cache<T,P>>
   {
   public:
      Cache();

      std::shared_ptr<const T> GetObject(const String &sName);
      // Retrieves an object using the object name.

      std::shared_ptr<const T> GetObject(__int64 iID);
      // Retrieves an object using the ID

      void RemoveObject(std::shared_ptr<T> pObject);
      void RemoveObject(const String &sName);
      void RemoveObject(__int64 iID);

      void SetTTL(int iNewVal);
      int GetHitRate();
      void SetEnabled(bool bEnabled);
      void Clear();

   private:


      template<typename Tag, typename MultiIndexContainer, typename TagValue>
      std::shared_ptr<T> GetItemBy(const MultiIndexContainer& s, TagValue value)
      {
         typedef index<container_type, Tag>::type items_by_tag;
         items_by_tag& items = get<Tag>(objects_);
         auto item = items.find(value);

         if (item != items.end())
         {
            CachedObject<T> cached_object = (*item);
            if (GetObjectIsWithinTTL_(cached_object))
            {
               return cached_object.object_;
            }

            items.erase(item);
         }

         std::shared_ptr<T> empty;
         return empty;
      }
      
      template<typename Tag, typename MultiIndexContainer, typename TagValue>
      void RemoveBy(const MultiIndexContainer& s, TagValue value)
      {
         typedef index<container_type, Tag>::type items_by_tag;
         items_by_tag& items = get<Tag>(objects_);

         auto item = items.find(value);

         if (item != items.end())
         {
            items.erase(item);
         }

      }
      
      bool GetObjectIsWithinTTL_(CachedObject<T> pObject);
      void AddToCache_(std::shared_ptr<T> pObject);

      int no_of_misses_;
      int no_of_hits_;
      int ttl_;
      bool enabled_;

      boost::recursive_mutex _mutex;
      
      typedef multi_index_container<
         CachedObject<typename T>,
         indexed_by<
            hashed_unique<
            tag<id>, BOOST_MULTI_INDEX_MEMBER(CachedObject<typename T>, __int64, id_)>,
            hashed_unique<
            tag<name>, BOOST_MULTI_INDEX_MEMBER(CachedObject<typename T>, std::wstring, name_)>,
            ordered_non_unique<
            tag<timestamp>, BOOST_MULTI_INDEX_MEMBER(CachedObject<typename T>, int, creation_time_)> >
      > container_type;

      container_type objects_;
      //std::map<String, std::shared_ptr<T> > objects_;
      // All the objects in the cache
   };

   template <class T, class P> 
   Cache<T,P>::Cache()
   {
      no_of_misses_ = 0;
      no_of_hits_ = 0;
      ttl_ = 0;
      enabled_ = false;
   }

   template <class T, class P> 
   void
   Cache<T,P>::Clear()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      objects_.clear();
      no_of_misses_ = 0;
      no_of_hits_ = 0;
   }

   template <class T, class P> 
   void
   Cache<T,P>::SetTTL(int iNewVal)
   {
      ttl_ = iNewVal;

      no_of_misses_ = 0;
      no_of_hits_ = 0;
   }


   template <class T, class P> 
   void
   Cache<T,P>::SetEnabled(bool bEnabled)
   {
      enabled_ = bEnabled;

      if (!enabled_)
         Clear();
   }


   template <class T, class P> 
   int
   Cache<T,P>::GetHitRate()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      if (no_of_hits_ == 0)
         return 0;

      int iHitRate = (int) (((float) no_of_hits_ / (float) (no_of_hits_ + no_of_misses_)) * 100);

      return iHitRate;
   }

   template <class T, class P> 
   void 
   Cache<T,P>::RemoveObject(std::shared_ptr<T> pObject)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      RemoveBy<name>(objects_, pObject->GetName());
   }

   template <class T, class P> 
   void 
   Cache<T,P>::RemoveObject(const String &sName)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      RemoveBy<name>(objects_, sName);


   }

   template <class T, class P> 
   void 
   Cache<T,P>::RemoveObject(__int64 iID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      RemoveBy<id>(objects_, iID);
   }

   template <class T, class P> 
   std::shared_ptr<const T> 
   Cache<T,P>::GetObject(const String &sName)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      if (enabled_)
      {
         auto object = GetItemBy<name>(objects_, sName);

         if (object != nullptr)
         {
            return object;
         }
      }

      // Load the object
      std::shared_ptr<T> pRetObject = std::shared_ptr<T>(new T);
      
      if (!P::ReadObject(pRetObject, sName))
      {
         std::shared_ptr<T> pEmpty;
         return pEmpty;
      }

      if (enabled_)
         AddToCache_(pRetObject);

      return pRetObject;
   }

   template <class T, class P> 
   std::shared_ptr<const T> 
   Cache<T,P>::GetObject(__int64 iID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      if (enabled_)
      {
         auto object = GetItemBy<id>(objects_, iID);

         if (object != nullptr)
            return object;
      }

      // Load the object
      std::shared_ptr<T> pRetObject = std::shared_ptr<T>(new T);
      if (!P::ReadObject(pRetObject, iID))
      {
         std::shared_ptr<T> pEmpty;
         return pEmpty;
      }

      if (enabled_)
         AddToCache_(pRetObject);
   
      return pRetObject;

   }

   template <class T, class P> 
   void 
   Cache<T,P>::AddToCache_(std::shared_ptr<T> pObject)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      // Object must be saved before it can be cached.
#ifdef DEBUG
      if (pObject->GetID() == 0)
      {
         assert(0);
      }
#endif

      CachedObject<T> object(pObject);

      no_of_misses_++;
      get<name>(objects_).insert(object);
      //objects_.insert(object);
   }

   template <class T, class P> 
   bool 
   Cache<T,P>::GetObjectIsWithinTTL_(CachedObject<T> pObject)
   {
      if (pObject.SecondsOld() < ttl_)
      {
         // A fresh object was found in the cache.
         no_of_hits_++;
         return true;
      }

      return false;
   }
}
