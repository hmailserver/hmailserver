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

#pragma once

namespace HM
{

   struct id {};
   struct name {};
   struct timestamp {};

   template <typename T>
   class Cache : public Singleton<Cache<T>>
   {
   public:
      Cache();

      std::shared_ptr<T> GetObject(const String &sName);
      // Retrieves an object using the object name.

      std::shared_ptr<T> GetObject(__int64 iID);
      // Retrieves an object using the ID

      void RemoveObject(std::shared_ptr<T> pObject);
      void RemoveObject(const String &sName);
      void RemoveObject(__int64 iID);

      void SetTTL(int iNewVal);
      int GetHitRate();
      void SetEnabled(bool bEnabled);
      void Clear();

      void AdjustEstimatedSize(bool increase, size_t size_change);

      void SetMaxSize(size_t max_size);
      size_t GetMaxSize();
      size_t GetSize();

      void Add(std::shared_ptr<T> pObject);

   private:

      void ResetEstimatedSizeIfEmpty_();


      template<typename Tag, typename MultiIndexContainer, typename TagValue>
      std::shared_ptr<T> GetItemBy_(const MultiIndexContainer& s, TagValue value)
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
      void RemoveBy_(const MultiIndexContainer& s, TagValue value)
      {
         typedef index<container_type, Tag>::type items_by_tag;
         items_by_tag& items = get<Tag>(objects_);

         auto item_iter = items.find(value);

         if (item_iter != items.end())
         {
            auto item = (*item_iter);

            if (current_estimated_size_ >= item.GetEstimatedSize())
               current_estimated_size_ -= item.GetEstimatedSize();
            
            items.erase(item_iter);
         }

         ResetEstimatedSizeIfEmpty_();

      }
      
      bool GetObjectIsWithinTTL_(CachedObject<T> pObject);
      

      boost::recursive_mutex _mutex;
      
      typedef multi_index_container<
         CachedObject<typename T>,
         indexed_by<
            hashed_unique<
            tag<id>, BOOST_MULTI_INDEX_MEMBER(CachedObject<typename T>, __int64, id_)>,
            hashed_non_unique<
            tag<name>, BOOST_MULTI_INDEX_MEMBER(CachedObject<typename T>, std::wstring, name_)>,
            ordered_non_unique<
            tag<timestamp>, BOOST_MULTI_INDEX_MEMBER(CachedObject<typename T>, int, creation_time_)> >
      > container_type;

      container_type objects_;

      int no_of_misses_;
      int no_of_hits_;
      int ttl_;
      bool enabled_;
      size_t max_size_;
      size_t current_estimated_size_;
   };

   template <class T> 
   Cache<T>::Cache() :
      max_size_(0),
      current_estimated_size_(0),
      no_of_misses_(0),
      no_of_hits_(0),
      ttl_(0),
      enabled_(false)
   {
      
      
   }

   template <class T> 
   void
   Cache<T>::Clear()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      objects_.clear();
      no_of_misses_ = 0;
      no_of_hits_ = 0;
   }

   template <class T> 
   void
   Cache<T>::SetTTL(int iNewVal)
   {
      ttl_ = iNewVal;

      no_of_misses_ = 0;
      no_of_hits_ = 0;
   }


   template <class T> 
   void
   Cache<T>::SetEnabled(bool bEnabled)
   {
      enabled_ = bEnabled;

      if (!enabled_)
         Clear();
   }

   template <class T>
   void
   Cache<T>::SetMaxSize(size_t max_size)
   {
      max_size_ = max_size;
   }

   template <class T>
   size_t
   Cache<T>::GetMaxSize()
   {
      return max_size_;
   }


   template <class T>
   size_t
   Cache<T>::GetSize()
   {
      return current_estimated_size_;
   }


   template <class T> 
   int
   Cache<T>::GetHitRate()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      if (no_of_hits_ == 0)
         return 0;

      int iHitRate = (int) (((float) no_of_hits_ / (float) (no_of_hits_ + no_of_misses_)) * 100);

      return iHitRate;
   }

   template <class T> 
   void 
   Cache<T>::RemoveObject(std::shared_ptr<T> pObject)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      RemoveBy_<name>(objects_, pObject->GetName());
   }

   template <class T> 
   void 
   Cache<T>::RemoveObject(const String &sName)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      RemoveBy_<name>(objects_, sName);


   }

   template <class T> 
   void 
   Cache<T>::RemoveObject(__int64 iID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      RemoveBy_<id>(objects_, iID);
   }

   template <class T> 
   std::shared_ptr<T> 
   Cache<T>::GetObject(const String &sName)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      if (enabled_)
      {
         return GetItemBy_<name>(objects_, sName);
      }
      return nullptr;
   }

   template <class T> 
   std::shared_ptr<T> 
   Cache<T>::GetObject(__int64 iID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      if (enabled_)
      {
         return GetItemBy_<id>(objects_, iID);
      }

      return nullptr;
   }

   template <class T> 
   void 
   Cache<T>::Add(std::shared_ptr<T> pObject)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_mutex);

      if (!enabled_)
         return;

      // Object must be saved before it can be cached.
#ifdef DEBUG
      if (pObject->GetID() == 0)
      {
         assert(0);
      }
#endif

      typedef index<container_type, timestamp>::type items_by_timestamp;
      items_by_timestamp& items = get<timestamp>(objects_);

      CachedObject<T> object(pObject);

      if (max_size_ > 0 && current_estimated_size_ + object.GetEstimatedSize() > max_size_)
      {
         // We've reached the cache max size. Remove items until we're 10% free.
         size_t target_size = (size_t) (max_size_ * 0.9);

         while (current_estimated_size_ > target_size)
         {
            auto item_iter = items.begin();
            if (item_iter == items.end())
               break;

            auto item = (*item_iter);
            
            if (current_estimated_size_ >= item.GetEstimatedSize())
               current_estimated_size_ -= item.GetEstimatedSize();

            items.erase(item_iter);
         }

         ResetEstimatedSizeIfEmpty_();
      }

      no_of_misses_++;
      current_estimated_size_ += object.GetEstimatedSize();
      items.insert(object);
   }

   template <class T> 
   bool 
   Cache<T>::GetObjectIsWithinTTL_(CachedObject<T> pObject)
   {
      if (pObject.SecondsOld() < ttl_)
      {
         // A fresh object was found in the cache.
         no_of_hits_++;
         return true;
      }

      return false;
   }


   template <class T>
   void
   Cache<T>::ResetEstimatedSizeIfEmpty_()
   {
      if (objects_.size() == 0)
      {
         current_estimated_size_ = 0;
      }

   }

   template <class T>
   void
   Cache<T>::AdjustEstimatedSize(bool increase, size_t size_change)
   {
      if (increase)
      {
         current_estimated_size_ += size_change;
      }
      else
      {
         if (size_change > current_estimated_size_)
            current_estimated_size_ -= size_change;
         else
            current_estimated_size_ = 0;
      }

   }
}
