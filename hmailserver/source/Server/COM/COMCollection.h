// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

template <class T, class P>
class COMCollectionItem
{
public:

   void AttachItem(shared_ptr<T> pObject)
   {
      m_pObject = pObject;
   }

   void AttachParent(shared_ptr<P> pParentColl, bool bObjectIsInParent)
   {
      m_pParentCollection = pParentColl;
      m_bObjectIsInCollection = bObjectIsInParent;
   }

   void AddToParentCollection()
   {
      if (m_bObjectIsInCollection || !m_pParentCollection)
         return; 

      m_pParentCollection->AddItem(m_pObject);
      m_bObjectIsInCollection = true;
   }

   shared_ptr<P> m_pParentCollection;
   shared_ptr<T> m_pObject;


private:

   bool m_bObjectIsInCollection;
};

