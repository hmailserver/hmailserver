// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   
   template <typename T, typename P, typename SearchType>
   class CacheReaderWithDbFallback
   {
   public:
   
      std::shared_ptr<const T> GetObject(const SearchType &sName)
      {
         auto cache = Cache<T>::Instance();

         std::shared_ptr<T> object = cache->GetObject(sName);
         if (object == nullptr)
         {
            // Load the object
            object = std::shared_ptr<T>(new T);

            if (!P::ReadObject(object, sName))
            {
               return nullptr;
            }

            cache->Add(object);
         }

         return object;
      }

   };
}