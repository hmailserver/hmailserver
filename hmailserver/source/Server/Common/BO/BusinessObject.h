// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   template <class T>
   class BusinessObject
   {
   public:
      BusinessObject<T>::BusinessObject();
      virtual ~BusinessObject<T>() {};

		__int64 GetID() const;
		void SetID(__int64 iNewVal);

   protected:

     __int64 id_;
   };

   template <class T>
   BusinessObject<T>::BusinessObject()
   {
      id_ = 0;
   }

   template <class T>
   __int64 BusinessObject<T>::GetID() const
   {
      return id_;
   }

   template <class T> 
   void BusinessObject<T>::SetID(__int64 iNewID)
   {
      id_ = iNewID;
   }  

}  