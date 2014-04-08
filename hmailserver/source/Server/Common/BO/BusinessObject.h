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

     __int64 m_iID;
   };

   template <class T>
   BusinessObject<T>::BusinessObject()
   {
      m_iID = 0;
   }

   template <class T>
   __int64 BusinessObject<T>::GetID() const
   {
      return m_iID;
   }

   template <class T> 
   void BusinessObject<T>::SetID(__int64 iNewID)
   {
      m_iID = iNewID;
   }  

}  